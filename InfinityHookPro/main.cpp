#include "hook.hpp"

// ============================================================================
// NtShutdownSystem 相关类型定义
// ============================================================================
typedef enum _SHUTDOWN_ACTION {
    ShutdownNoReboot = 0,
    ShutdownReboot   = 1,
    ShutdownPowerOff = 2
} SHUTDOWN_ACTION;

typedef NTSTATUS(NTAPI* PNT_SHUTDOWN_SYSTEM)(IN SHUTDOWN_ACTION ShutdownAction);

// ============================================================================
// 全局变量
// ============================================================================
PNT_SHUTDOWN_SYSTEM g_OriginalNtShutdownSystem = NULL;

// ============================================================================
// 前向声明
// ============================================================================
VOID DriverUnload(PDRIVER_OBJECT DriverObject);

// ============================================================================
// Fake 函数：拦截 NtShutdownSystem
// ============================================================================
NTSTATUS NTAPI FakeNtShutdownSystem(IN SHUTDOWN_ACTION ShutdownAction)
{
    // ---- 安全检查：以下情况直接放行，避免影响系统自身运行 ----
    // 1) 非被动级别不能做太多事，直接调用原函数
    if (KeGetCurrentIrql() != PASSIVE_LEVEL)
        return g_OriginalNtShutdownSystem(ShutdownAction);

    // 2) 内核模式发起的关机（如系统自身流程）放行
    //    如果你想连内核模式的关机也阻止，注释掉下面这行
    if (ExGetPreviousMode() == KernelMode)
        return g_OriginalNtShutdownSystem(ShutdownAction);

    // 3) 会话0（系统服务/系统进程）发起的关机放行
    //    如果你想阻止所有会话，注释掉下面这行
    if (PsGetProcessSessionId(IoGetCurrentProcess()) == 0)
        return g_OriginalNtShutdownSystem(ShutdownAction);

    // ---- 拦截用户模式发起的关机/重启 ----
    DbgPrintEx(0, 0,
               "[IHP] Shutdown denied (action=%d, pid=%lu)\n",
               ShutdownAction,
               HandleToUlong(PsGetCurrentProcessId()));

    return STATUS_ACCESS_DENIED;
}

// ============================================================================
// InfinityHookPro 回调：每次系统调用时触发，决定是否替换函数指针
// 注意：这不是修改 SSDT，而是修改栈上即将执行的系统调用指针
// ============================================================================
void __fastcall InfinityCallback(unsigned long nCallIndex, PVOID* pCallAddress)
{
    UNREFERENCED_PARAMETER(nCallIndex);

    if (pCallAddress &&
        *pCallAddress == (PVOID)g_OriginalNtShutdownSystem)
    {
        *pCallAddress = (PVOID)FakeNtShutdownSystem;
    }
}

// ============================================================================
// 驱动卸载
// ============================================================================
VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);

    // 停止拦截（恢复所有系统调用走原始函数）
    KHook::Stop();

    // 关键：等待所有执行点离开当前驱动，避免卸载后蓝屏
    // 官方示例做法：倒计时 10 秒
    for (ULONG i = 10; i > 0; i--)
    {
        DbgPrintEx(0, 0, "[DriverUnload] Countdown: %lu\n", i);
        LARGE_INTEGER interval;
        interval.QuadPart = -1000 * 10000;  // 1 秒（单位：100ns，负数表示相对时间）
        KeDelayExecutionThread(KernelMode, FALSE, &interval);
    }

    DbgPrintEx(0, 0, "[DriverUnload] Completed!\n");
}

// ============================================================================
// 驱动入口
// ============================================================================
EXTERN_C NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    DriverObject->DriverUnload = DriverUnload;

    // 解析 NtShutdownSystem 地址
    UNICODE_STRING name;
    RtlInitUnicodeString(&name, L"NtShutdownSystem");
    g_OriginalNtShutdownSystem =
        (PNT_SHUTDOWN_SYSTEM)MmGetSystemRoutineAddress(&name);

    if (!g_OriginalNtShutdownSystem)
    {
        DbgPrintEx(0, 0, "[DriverEntry] Failed to resolve NtShutdownSystem\n");
        return STATUS_UNSUCCESSFUL;
    }

    // 初始化 hook 框架（必须传入回调指针），然后开始拦截
    if (!KHook::Initialize(InfinityCallback) || !KHook::Start())
    {
        DbgPrintEx(0, 0, "[DriverEntry] KHook Initialize/Start failed\n");
        return STATUS_UNSUCCESSFUL;
    }

    DbgPrintEx(0, 0, "[DriverEntry] NtShutdownSystem hook installed\n");
    return STATUS_SUCCESS;
}