<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: PthreadsCE - Win32 (WCE ARM) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\aaron\LOCALS~1\Temp\RSP12.tmp" with contents
[
/nologo /W3 /Zi /O2 /I "../../Libraries/pthreads-snap-2002-11-04" /I "../.." /I "../../Libraries/Pocketfrog_0_6_0" /I "../../Libraries/STLport-4.5.3/stlport" /I "../../Libraries/zlib" /I "../../Libraries/jpgsrc6b" /I "../../Libraries/libmng" /I "../../Libraries/lcms/include" /D "_USRDLL" /D "PTHREADSCE_EXPORTS" /D "WINCE" /D "DEBUG" /D _WIN32_WCE=300 /D "UNKNOWN" /D "ARM" /D "_ARM_" /D UNDER_CE=300 /D "UNICODE" /D "_UNICODE" /D "HAVE_CONFIG_H" /Fp"ARMDbg/PthreadsCE.pch" /YX /Fo"ARMDbg/" /Fd"ARMDbg/" /MC /c 
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\attr.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\cleanup.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\condvar.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\create.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\dll.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\errno.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\fork.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\global.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\misc.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\mutex.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\nonportable.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\private.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sched.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\semaphore.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\signal.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sync.c"
"C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\tsd.c"
]
Creating command line "clarm.exe @C:\DOCUME~1\aaron\LOCALS~1\Temp\RSP12.tmp" 
Creating temporary file "C:\DOCUME~1\aaron\LOCALS~1\Temp\RSP13.tmp" with contents
[
commctrl.lib coredll.lib /nologo /base:"0x00100000" /stack:0x10000,0x1000 /entry:"_DllMainCRTStartup" /dll /incremental:yes /pdb:"ARMDbg/PthreadsCE.pdb" /debug /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"ARMDbg/PthreadsCE.dll" /implib:"ARMDbg/PthreadsCE.lib" /subsystem:windowsce,3.00 /align:"4096" /MACHINE:ARM 
".\ARMDbg\attr.obj"
".\ARMDbg\cleanup.obj"
".\ARMDbg\condvar.obj"
".\ARMDbg\create.obj"
".\ARMDbg\dll.obj"
".\ARMDbg\errno.obj"
".\ARMDbg\fork.obj"
".\ARMDbg\global.obj"
".\ARMDbg\misc.obj"
".\ARMDbg\mutex.obj"
".\ARMDbg\nonportable.obj"
".\ARMDbg\private.obj"
".\ARMDbg\sched.obj"
".\ARMDbg\semaphore.obj"
".\ARMDbg\signal.obj"
".\ARMDbg\sync.obj"
".\ARMDbg\tsd.obj"
]
Creating command line "link.exe @C:\DOCUME~1\aaron\LOCALS~1\Temp\RSP13.tmp"
<h3>Output Window</h3>
Compiling...
attr.c
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_init.c(67) : warning C4273: 'pthread_attr_init' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_destroy.c(65) : warning C4273: 'pthread_attr_destroy' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_getdetachstate.c(79) : warning C4273: 'pthread_attr_getdetachstate' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_setdetachstate.c(78) : warning C4273: 'pthread_attr_setdetachstate' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_getstackaddr.c(82) : warning C4273: 'pthread_attr_getstackaddr' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_setstackaddr.c(82) : warning C4273: 'pthread_attr_setstackaddr' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_getstacksize.c(83) : warning C4273: 'pthread_attr_getstacksize' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_setstacksize.c(83) : warning C4273: 'pthread_attr_setstacksize' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_getscope.c(47) : warning C4273: 'pthread_attr_getscope' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_setscope.c(47) : warning C4273: 'pthread_attr_setscope' : inconsistent dll linkage.  dllexport assumed.
cleanup.c
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\cleanup.c(75) : warning C4273: 'ptw32_pop_cleanup' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\cleanup.c(141) : warning C4273: 'ptw32_push_cleanup' : inconsistent dll linkage.  dllexport assumed.
condvar.c
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_condattr_init.c(72) : warning C4273: 'pthread_condattr_init' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_condattr_destroy.c(69) : warning C4273: 'pthread_condattr_destroy' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_condattr_getpshared.c(82) : warning C4273: 'pthread_condattr_getpshared' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_condattr_setpshared.c(84) : warning C4273: 'pthread_condattr_setpshared' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_cond_init.c(70) : warning C4273: 'pthread_cond_init' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_cond_destroy.c(116) : warning C4273: 'pthread_cond_destroy' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_cond_wait.c(513) : warning C4273: 'pthread_cond_wait' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_cond_wait.c(570) : warning C4273: 'pthread_cond_timedwait' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_cond_signal.c(310) : warning C4273: 'pthread_cond_signal' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_cond_signal.c(349) : warning C4273: 'pthread_cond_broadcast' : inconsistent dll linkage.  dllexport assumed.
create.c
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\create.c(80) : warning C4273: 'pthread_create' : inconsistent dll linkage.  dllexport assumed.
dll.c
errno.c
fork.c
global.c
misc.c
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_once.c(77) : warning C4273: 'pthread_once' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_self.c(62) : warning C4273: 'pthread_self' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_equal.c(65) : warning C4273: 'pthread_equal' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_setconcurrency.c(43) : warning C4273: 'pthread_setconcurrency' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_getconcurrency.c(43) : warning C4273: 'pthread_getconcurrency' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\w32_CancelableWait.c(153) : warning C4273: 'pthreadCancelableWait' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\w32_CancelableWait.c(159) : warning C4273: 'pthreadCancelableTimedWait' : inconsistent dll linkage.  dllexport assumed.
mutex.c
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_mutex_init.c(43) : warning C4273: 'pthread_mutex_init' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_mutex_destroy.c(43) : warning C4273: 'pthread_mutex_destroy' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_mutexattr_init.c(67) : warning C4273: 'pthread_mutexattr_init' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_mutexattr_destroy.c(67) : warning C4273: 'pthread_mutexattr_destroy' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_mutexattr_getpshared.c(81) : warning C4273: 'pthread_mutexattr_getpshared' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_mutexattr_setpshared.c(84) : warning C4273: 'pthread_mutexattr_setpshared' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_mutexattr_settype.c(121) : warning C4273: 'pthread_mutexattr_settype' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_mutexattr_gettype.c(44) : warning C4273: 'pthread_mutexattr_gettype' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_mutex_lock.c(43) : warning C4273: 'pthread_mutex_lock' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_mutex_timedlock.c(196) : warning C4273: 'pthread_mutex_timedlock' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_mutex_unlock.c(43) : warning C4273: 'pthread_mutex_unlock' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_mutex_trylock.c(43) : warning C4273: 'pthread_mutex_trylock' : inconsistent dll linkage.  dllexport assumed.
nonportable.c
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_mutexattr_setkind_np.c(42) : warning C4273: 'pthread_mutexattr_setkind_np' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_mutexattr_getkind_np.c(42) : warning C4273: 'pthread_mutexattr_getkind_np' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_getw32threadhandle_np.c(51) : warning C4273: 'pthread_getw32threadhandle_np' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_delay_np.c(83) : warning C4273: 'pthread_delay_np' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_num_processors_np.c(47) : warning C4273: 'pthread_num_processors_np' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_win32_attach_detach_np.c(47) : warning C4273: 'pthread_win32_process_attach_np' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_win32_attach_detach_np.c(106) : warning C4273: 'pthread_win32_process_detach_np' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_win32_attach_detach_np.c(138) : warning C4273: 'pthread_win32_thread_attach_np' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_win32_attach_detach_np.c(144) : warning C4273: 'pthread_win32_thread_detach_np' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_timechange_handler_np.c(90) : warning C4273: 'pthread_timechange_handler_np' : inconsistent dll linkage.  dllexport assumed.
private.c
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\ptw32_throw.c(120) : warning C4273: 'ptw32_get_exception_services_code' : inconsistent dll linkage.  dllexport assumed.
sched.c
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_setschedpolicy.c(44) : warning C4273: 'pthread_attr_setschedpolicy' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_getschedpolicy.c(44) : warning C4273: 'pthread_attr_getschedpolicy' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_setschedparam.c(44) : warning C4273: 'pthread_attr_setschedparam' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_getschedparam.c(44) : warning C4273: 'pthread_attr_getschedparam' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_setinheritsched.c(44) : warning C4273: 'pthread_attr_setinheritsched' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_attr_getinheritsched.c(44) : warning C4273: 'pthread_attr_getinheritsched' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_setschedparam.c(44) : warning C4273: 'pthread_setschedparam' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_getschedparam.c(44) : warning C4273: 'pthread_getschedparam' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sched_get_priority_max.c(56) : warning C4273: 'sched_get_priority_max' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sched_get_priority_min.c(56) : warning C4273: 'sched_get_priority_min' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sched_setscheduler.c(43) : warning C4273: 'sched_setscheduler' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sched_getscheduler.c(43) : warning C4273: 'sched_getscheduler' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sched_yield.c(67) : warning C4273: 'sched_yield' : inconsistent dll linkage.  dllexport assumed.
semaphore.c
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sem_init.c(82) : warning C4273: 'sem_init' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sem_destroy.c(74) : warning C4273: 'sem_destroy' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sem_trywait.c(78) : warning C4273: 'sem_trywait' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sem_wait.c(79) : warning C4273: 'sem_wait' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sem_timedwait.c(92) : warning C4273: 'sem_timedwait' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sem_post.c(74) : warning C4273: 'sem_post' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sem_post_multiple.c(77) : warning C4273: 'sem_post_multiple' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sem_getvalue.c(77) : warning C4273: 'sem_getvalue' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sem_open.c(55) : warning C4273: 'sem_open' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sem_close.c(55) : warning C4273: 'sem_close' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\sem_unlink.c(55) : warning C4273: 'sem_unlink' : inconsistent dll linkage.  dllexport assumed.
signal.c
sync.c
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_detach.c(70) : warning C4273: 'pthread_detach' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_join.c(76) : warning C4273: 'pthread_join' : inconsistent dll linkage.  dllexport assumed.
tsd.c
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_key_create.c(77) : warning C4273: 'pthread_key_create' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_key_delete.c(68) : warning C4273: 'pthread_key_delete' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_setspecific.c(67) : warning C4273: 'pthread_setspecific' : inconsistent dll linkage.  dllexport assumed.
C:\ha\LIBRARIES\PTHREADS-SNAP-2002-11-04\pthread_getspecific.c(65) : warning C4273: 'pthread_getspecific' : inconsistent dll linkage.  dllexport assumed.
Linking...
   Creating library ARMDbg/PthreadsCE.lib and object ARMDbg/PthreadsCE.exp
cleanup.obj : warning LNK4217: locally defined symbol pthread_setspecific imported in function ptw32_pop_cleanup
misc.obj : warning LNK4049: locally defined symbol pthread_setspecific imported
nonportable.obj : warning LNK4049: locally defined symbol pthread_setspecific imported
private.obj : warning LNK4049: locally defined symbol pthread_setspecific imported
cleanup.obj : warning LNK4217: locally defined symbol pthread_getspecific imported in function ptw32_pop_cleanup
misc.obj : warning LNK4049: locally defined symbol pthread_getspecific imported
nonportable.obj : warning LNK4049: locally defined symbol pthread_getspecific imported
private.obj : warning LNK4049: locally defined symbol pthread_getspecific imported
condvar.obj : warning LNK4217: locally defined symbol sem_destroy imported in function pthread_cond_init
mutex.obj : warning LNK4049: locally defined symbol sem_destroy imported
condvar.obj : warning LNK4217: locally defined symbol pthread_mutex_init imported in function pthread_cond_init
condvar.obj : warning LNK4217: locally defined symbol sem_init imported in function pthread_cond_init
mutex.obj : warning LNK4049: locally defined symbol sem_init imported
condvar.obj : warning LNK4217: locally defined symbol pthread_mutex_destroy imported in function pthread_cond_destroy
private.obj : warning LNK4049: locally defined symbol pthread_mutex_destroy imported
tsd.obj : warning LNK4049: locally defined symbol pthread_mutex_destroy imported
tsd.obj : warning LNK4049: locally defined symbol pthread_mutex_unlock imported
condvar.obj : warning LNK4217: locally defined symbol pthread_mutex_unlock imported in function pthread_cond_destroy
misc.obj : warning LNK4049: locally defined symbol pthread_mutex_unlock imported
nonportable.obj : warning LNK4049: locally defined symbol pthread_mutex_unlock imported
private.obj : warning LNK4049: locally defined symbol pthread_mutex_unlock imported
condvar.obj : warning LNK4217: locally defined symbol sem_post imported in function pthread_cond_destroy
mutex.obj : warning LNK4049: locally defined symbol sem_post imported
condvar.obj : warning LNK4217: locally defined symbol pthread_mutex_trylock imported in function pthread_cond_destroy
condvar.obj : warning LNK4217: locally defined symbol sem_wait imported in function pthread_cond_destroy
mutex.obj : warning LNK4049: locally defined symbol sem_wait imported
condvar.obj : warning LNK4217: locally defined symbol ptw32_pop_cleanup imported in function ptw32_cond_timedwait
private.obj : warning LNK4049: locally defined symbol ptw32_pop_cleanup imported
condvar.obj : warning LNK4217: locally defined symbol sem_timedwait imported in function ptw32_cond_timedwait
condvar.obj : warning LNK4217: locally defined symbol ptw32_push_cleanup imported in function ptw32_cond_timedwait
tsd.obj : warning LNK4049: locally defined symbol pthread_mutex_lock imported
condvar.obj : warning LNK4217: locally defined symbol pthread_mutex_lock imported in function ptw32_cond_wait_cleanup
misc.obj : warning LNK4049: locally defined symbol pthread_mutex_lock imported
nonportable.obj : warning LNK4049: locally defined symbol pthread_mutex_lock imported
private.obj : warning LNK4049: locally defined symbol pthread_mutex_lock imported
condvar.obj : warning LNK4217: locally defined symbol sem_post_multiple imported in function ptw32_cond_unblock
dll.obj : warning LNK4217: locally defined symbol pthread_win32_process_detach_np imported in function DllMain
dll.obj : warning LNK4217: locally defined symbol pthread_win32_thread_detach_np imported in function DllMain
private.obj : warning LNK4049: locally defined symbol pthread_win32_thread_detach_np imported
dll.obj : warning LNK4217: locally defined symbol pthread_win32_thread_attach_np imported in function DllMain
dll.obj : warning LNK4217: locally defined symbol pthread_win32_process_attach_np imported in function DllMain
sync.obj : warning LNK4049: locally defined symbol pthread_self imported
tsd.obj : warning LNK4049: locally defined symbol pthread_self imported
errno.obj : warning LNK4217: locally defined symbol pthread_self imported in function _errno
mutex.obj : warning LNK4049: locally defined symbol pthread_self imported
nonportable.obj : warning LNK4049: locally defined symbol pthread_self imported
private.obj : warning LNK4049: locally defined symbol pthread_self imported
mutex.obj : warning LNK4217: locally defined symbol pthread_equal imported in function pthread_mutex_destroy
sync.obj : warning LNK4049: locally defined symbol pthread_equal imported
nonportable.obj : warning LNK4217: locally defined symbol pthread_mutexattr_settype imported in function pthread_mutexattr_setkind_np
nonportable.obj : warning LNK4217: locally defined symbol pthread_mutexattr_gettype imported in function pthread_mutexattr_getkind_np
nonportable.obj : warning LNK4217: locally defined symbol pthread_cond_broadcast imported in function pthread_timechange_handler_np
private.obj : warning LNK4217: locally defined symbol pthread_key_create imported in function ptw32_processInitialize
private.obj : warning LNK4217: locally defined symbol pthread_key_delete imported in function ptw32_processTerminate
semaphore.obj : warning LNK4217: locally defined symbol pthreadCancelableWait imported in function sem_wait
sync.obj : warning LNK4049: locally defined symbol pthreadCancelableWait imported
semaphore.obj : warning LNK4217: locally defined symbol pthreadCancelableTimedWait imported in function sem_timedwait



<h3>Results</h3>
PthreadsCE.dll - 0 error(s), 140 warning(s)
</pre>
</body>
</html>
