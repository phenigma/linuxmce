<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: Orbiter - Win32 (WCE ARMV4) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\Race\LOCALS~1\Temp\RSP438.tmp" with contents
[
/nologo /Zp4 /Zi /Od /I "." /I ".." /I "../.." /I "../../DCE" /I "../../Libraries/pthreads-snap-2002-11-04" /I "../../Libraries/pthreads-win32/include" /I "../../CrossPlatform-Libs/sge030809-w32/src" /I "../../CrossPlatform-Libs/SDL_ttf-2.0.6" /I "../../CrossPlatform-Libs/SDL_image-1.2.3" /I "../../CrossPlatform-Libs/SDL-1.2.7/include" /I "../../Splitter" /I "../../CrossPlatform-Libs/zlib" /I "../../CrossPlatform-Libs/lpng127" /I "../../Libraries/STLport-4.5.3/stlport" /I "../../Libraries/STLport-4.5.3/src" /I "../../PlutoUtils" /D "DEBUG" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=420 /D _WIN32_WCE=420 /D "WIN32_PLATFORM_PSPC=400" /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "WINCE" /D "ORBITER" /D "__STL_NO_IOSTREAMS" /D "LOAD_PNG" /D "PROFILING" /D "CACHE_IMAGES" /Fo"ARMV4Dbg/" /Fd"ARMV4Dbg/" /I /pluto/trunk" /MC " /c 
"C:\pluto\trunk\Orbiter\CENet\MainDialog.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\Race\LOCALS~1\Temp\RSP438.tmp" 
Creating temporary file "C:\DOCUME~1\Race\LOCALS~1\Temp\RSP439.tmp" with contents
[
commctrl.lib coredll.lib aygshell.lib PthreadsCE.lib winsock.lib sdl.lib sdl_image_wince.lib sdl_ttf_wince.lib freetype_wince.lib zlib_wince.lib ccrtrtti.lib sge_wince.lib lpng_wince.lib /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:yes /pdb:"ARMV4Dbg/Orbiter.pdb" /debug /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"ARMV4Dbg/Orbiter.exe" /libpath:"Ritti/ArmV4" /libpath:"../../CrossPlatform-Libs/zlib/zlib_wince/ARMV4Dbg" /libpath:"../../CrossPlatform-Libs/sge030809-w32/sge_wince/ARMV4Dbg" /libpath:"../../CrossPlatform-Libs/lpng127/lpng_wince/ARMV4Dbg\\" /libpath:""../../CrossPlatform-Libs/SDL_image-1.2.3/WINCE/ARMV4Dbg"" /libpath:""../../CrossPlatform-Libs/SDL_ttf-2.0.6/WinCE/ARMV4Dbg"" /libpath:""c:\pluto\trunk\CrossPlatform-Libs\freetype-2.1.9\freetype_wince\ARMV4Dbg\"" /libpath:""../../CrossPlatform-Libs/SDL-1.2.7/VisualCE/SDL/ARMV4Dbg"" /libpath:""../../Libraries/pthreads-snap-2002-11-04/PthreadsCE/ARMDbg"" /libpath:""C:\Program /libpath:""../../CrossPlatform-Libs/SDL-1.2.7/VisualC7/SDL/Debug"" /libpath:""../../CrossPlatform-Libs/SDL_image-1.2.3/VisualC/Debug"" /libpath:""../../CrossPlatform-Libs/SDL_ttf-2.0.6/VisualC/Debug"" /libpath:""../../CrossPlatform-Libs/sge030809-w32/sge-msvc/sge/VisualC/Debug"" /libpath:""../../CrossPlatform-Libs/lpng127/projects/visualc6/Win32_DLL_Debug"" /libpath:""../../CrossPlatform-Libs/zlib/projects/visualc6/Win32_DLL_Debug"" /subsystem:windowsce,4.20 /align:"4096" /MACHINE:THUMB 
.\ARMV4Dbg\ClientSocket.obj
.\ARMV4Dbg\Command_Impl.obj
.\ARMV4Dbg\DataGrid.obj
.\ARMV4Dbg\DesignObj_Data.obj
.\ARMV4Dbg\DesignObj_Orbiter.obj
.\ARMV4Dbg\DeviceData_Base.obj
.\ARMV4Dbg\DeviceData_Impl.obj
.\ARMV4Dbg\Event_Impl.obj
.\ARMV4Dbg\FileUtils.obj
.\ARMV4Dbg\HandleRequestSocket.obj
.\ARMV4Dbg\Logger.obj
.\ARMV4Dbg\main.obj
.\ARMV4Dbg\MainDialog.obj
.\ARMV4Dbg\Message.obj
.\ARMV4Dbg\minilzo.obj
.\ARMV4Dbg\MultiThreadIncludes.obj
.\ARMV4Dbg\Orbiter.obj
.\ARMV4Dbg\OrbiterBase.obj
.\ARMV4Dbg\OrbiterSDL.obj
.\ARMV4Dbg\OrbiterSDL_WinCE.obj
.\ARMV4Dbg\Renderer.obj
.\ARMV4Dbg\SerializeClass.obj
.\ARMV4Dbg\Socket.obj
.\ARMV4Dbg\StartOrbiterCE.obj
.\ARMV4Dbg\StringUtils.obj
.\ARMV4Dbg\TextSplitter.obj
.\ARMV4Dbg\TextWrapper.obj
.\ARMV4Dbg\WinCE.obj
.\ARMV4Dbg\Orbiter.res
]
Creating command line "link.exe @C:\DOCUME~1\Race\LOCALS~1\Temp\RSP439.tmp"
<h3>Output Window</h3>
Compiling...
MainDialog.cpp
c:\pluto\trunk\libraries\stlport-4.5.3\stlport\stl\_prolog.h(16) : warning C4103: 'c:\pluto\trunk\libraries\stlport-4.5.3\stlport\stl\_prolog.h' : used #pragma pack to change alignment
c:\pluto\trunk\libraries\stlport-4.5.3\stlport\ctype.h(18) : warning C4103: 'c:\pluto\trunk\libraries\stlport-4.5.3\stlport\ctype.h' : used #pragma pack to change alignment
c:\pluto\trunk\libraries\stlport-4.5.3\stlport\stl\_epilog.h(10) : warning C4103: 'c:\pluto\trunk\libraries\stlport-4.5.3\stlport\stl\_epilog.h' : used #pragma pack to change alignment
c:\pluto\trunk\libraries\stlport-4.5.3\stlport\ctype.h(27) : warning C4103: 'c:\pluto\trunk\libraries\stlport-4.5.3\stlport\ctype.h' : used #pragma pack to change alignment
c:\pluto\trunk\orbiter\cenet\maindialog.cpp(25) : warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify -GX
Linking...
   Creating library ARMV4Dbg/Orbiter.lib and object ARMV4Dbg/Orbiter.exp
Orbiter.obj : warning LNK1166: cannot adjust code at offset=0x00001000, rva=0x00033DD4
freetype_wince.lib(ftbase.obj) : warning LNK1166: cannot adjust code at offset=0x00001000, rva=0x00082014
freetype_wince.lib(sfnt.obj) : warning LNK1166: cannot adjust code at offset=0x00001000, rva=0x0009C8E0
freetype_wince.lib(cff.obj) : warning LNK1166: cannot adjust code at offset=0x00001000, rva=0x000C6A6C
freetype_wince.lib(truetype.obj) : warning LNK1166: cannot adjust code at offset=0x00001000, rva=0x000D7D7C
sge_wince.lib(sge_primitives.obj) : warning LNK1166: cannot adjust code at offset=0x00001000, rva=0x00100408
lpng_wince.lib(pngrtran.obj) : warning LNK1166: cannot adjust code at offset=0x00001000, rva=0x00114CB0




<h3>Results</h3>
Orbiter.exe - 0 error(s), 12 warning(s)
</pre>
</body>
</html>
