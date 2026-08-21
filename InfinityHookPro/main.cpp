#include "hook.hpp"
typedef ULONG MINIPOWER_ACTION;
// ============================================================================
// NtInitiatePowerAction 相关类型定义
// ============================================================================

typedef NTSTATUS(NTAPI* PNT_INITIATE_POWER_ACTION)(
    IN POWER_ACTION SystemAction,
    IN MINIPOWER_ACTION LightestSystemState,
    IN ULONG Flags,
    IN BOOLEAN Asynchronous
);

// ============================================================================
// 全局变量
// ============================================================================
PNT_INITIATE_POWER_ACTION g_OriginalNtInitiatePowerAction = NULL;

// ============================================================================
// 前向声明
// ============================================================================
VOID DriverUnload(PDRIVER_OBJECT DriverObject);

// ============================================================================
// Fake 函数：拦截 NtInitiatePowerAction
// ============================================================================
NTSTATUS NTAPI FakeNtInitiatePowerAction(
    IN POWER_ACTION SystemAction,
    IN MINIPOWER_ACTION LightestSystemState,
    IN ULONG Flags,
    IN BOOLEAN Asynchronous
)
{
    // 拦截关机/重启/断电，其他放行
    if (SystemAction == PowerActionShutdown ||
        SystemAction == PowerActionShutdownReset ||
        SystemAction == PowerActionShutdownOff)
    {
        DbgPrintEx(0, 0,
                   "[IHP] Shutdown denied (action=%d, pid=%lu)\n",
                   SystemAction,
                   HandleToUlong(PsGetCurrentProcessId()));
        return STATUS_ACCESS_DENIED;
    }

    // 休眠/睡眠/其他 → 放行
    return g_OriginalNtInitiatePowerAction(
        SystemAction, LightestSystemState, Flags, Asynchronous);
}

// ============================================================================
// InfinityHookPro 回调：每次系统调用时触发，决定是否替换函数指针
// ============================================================================
void __fastcall InfinityCallback(unsigned long nCallIndex, PVOID* pCallAddress)
{
    UNREFERENCED_PARAMETER(nCallIndex);

    if (pCallAddress &&
        *pCallAddress == (PVOID)g_OriginalNtInitiatePowerAction)
    {
        *pCallAddress = (PVOID)FakeNtInitiatePowerAction;
    }
}

// ============================================================================
// 驱动卸载
// ============================================================================
VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);

    // 停止拦截
    KHook::Stop();

    // 等待所有执行点离开当前驱动
    for (ULONG i = 10; i > 0; i--)
    {
        DbgPrintEx(0, 0, "[DriverUnload] Countdown: %lu\n", i);
        LARGE_INTEGER interval;
        interval.QuadPart = -1000 * 10000;
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

    // 解析 NtInitiatePowerAction 地址
    UNICODE_STRING name;
    RtlInitUnicodeString(&name, L"NtInitiatePowerAction");
    g_OriginalNtInitiatePowerAction =
        (PNT_INITIATE_POWER_ACTION)MmGetSystemRoutineAddress(&name);

    if (!g_OriginalNtInitiatePowerAction)
    {
        DbgPrintEx(0, 0, "[DriverEntry] Failed to resolve NtInitiatePowerAction\n");
        return STATUS_UNSUCCESSFUL;
    }

    // 初始化 hook 框架
    if (!KHook::Initialize(InfinityCallback) || !KHook::Start())
    {
        DbgPrintEx(0, 0, "[DriverEntry] KHook Initialize/Start failed\n");
        return STATUS_UNSUCCESSFUL;
    }

    DbgPrintEx(0, 0, "[DriverEntry] NtInitiatePowerAction hook installed\n");
    return STATUS_SUCCESS;
}