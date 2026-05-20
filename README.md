# InfinityHookPro
InfinityHook 支持Win7 到 Win11 最新版本，虚拟机环境及物理机环境

最近学习InfinityHook,在FiYHer的版本上进行了一定的修改，以支持Win11 22621以上系统

在参考的大多数版本中，都只支持虚拟机环境，本源码进行了修改以支持物理机环境

代码里有详细的注释，同时可以结合分析文章 [《InfinityHook》](https://bbs.kanxue.com/) 

**关于PatchGuard**
* Windows 11 22621 物理机上测试了 24 小时未触发
* Windows 11 22631 物理机上测试了 48 小时未触发
* Windows 11 26016 预览版 物理机上测试了 48 小时未触发
* Windows 11 24H2  物理机上测试了 48 小时未触发

**PS** 

关于使用时的一个Bug，由于代码中修改了系统的性能计数器相关逻辑，导致开启Hook后系统睡眠较长时间再唤醒进入系统后，系统时间会错误，目前未找到合适的修正方法。

**再次接力,Make InfinityHook Great Again Again 💪💪💪**

### 参考博客
* https://bbs.kanxue.com/thread-266207.htm (带逆向分析)
* https://bbs.kanxue.com/thread-266136.htm	(挂钩失效问题)
* https://bbs.kanxue.com/thread-260962.htm	(19041)
* https://bbs.kanxue.com/thread-275070.htm (实践纪录武侠版 )
* https://bbs.kanxue.com/thread-253450.htm	(雪碧)
* https://www.anquanke.com/post/id/206288#h2-1	(19041)
* https://www.freebuf.com/articles/system/278857.html	(让InfinityHook再次伟大)

###	参考源码
* https://github.com/FiYHer/InfinityHookPro (FiYHer)
* https://github.com/huoji120/MakeInfinityHookGreatAgain	(2004)  
 

### 以下为各系统在物理机器上的测试
 <h4 align="center">
	Win7 x64 7600
	<img src="ScreenShot\Win7_7600.jpg" alt="Win7 x64 7600"/>
	<p>
		&nbsp;
	</p>
	Win7 x64 7601
	<img src="ScreenShot\Win7_X64.jpg" alt="Win7 x64 7601"/>
	<p>
		&nbsp;
	</p>
	Win8.1
	<img src="ScreenShot\Win_8.1.jpg" alt="Win8.1"/>
	<p>
		&nbsp;
	</p>
	Win10 1909
	<img src="ScreenShot\Win10_1909.jpg" alt="Win10 1909"/>
	<p>
		&nbsp;
	</p>
	Win10 20H2
	<img src="ScreenShot\Win10_20H2.jpg" alt="Win10 20H2"/>
	<p>
		&nbsp;
	</p>
	Win10 21H1
	<img src="ScreenShot\Win10_21H1.jpg" alt="Win10 21H1"/>
	<p>
		&nbsp;
	</p>
	Win10 21H2
	<img src="ScreenShot\Win10_21H2.jpg" alt="Win10 21H2"/>
	<p>
		&nbsp;
	</p>
	Win10 22H2
	<img src="ScreenShot\Win10_22H2.jpg" alt="Win10 22H2"/>
	<p>
		&nbsp;
	</p>
	Win11 22000
	<img src="ScreenShot\Win11_22000.jpg" alt="Win10 22000"/>
	<p>
		&nbsp;
	</p>
	Win11 22621
	<img src="ScreenShot\Win11_22621.jpg" alt="Win10 22621"/>
	<p>
		&nbsp;
	</p>
	Win11 22631
	<img src="ScreenShot\Win11_22631.jpg" alt="Win11 22631"/>
	<p>
		&nbsp;
	</p>
	Win11 26016
	<img src="ScreenShot\Win11_26016.jpg" alt="Win11 26016"/>
	<p>
		&nbsp;
	</p>
	Win11 24H2
	<img src="ScreenShot\Win11_24H2.jpg" alt="Win11 24H2"/>
</h4>
