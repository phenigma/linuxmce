<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: zlib_wince - Win32 (WCE ARMV4) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\Race\LOCALS~1\Temp\RSP1D6.tmp" with contents
[
/nologo /W3 /Zi /Od /I "c:\pluto\trunk\CrossPlatform-Libs\zlib\\" /D "DEBUG" /D _WIN32_WCE=420 /D "WIN32_PLATFORM_PSPC=400" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=420 /D "UNICODE" /D "_UNICODE" /D "_LIB" /D "NO_ERRNO_H" /Fp"ARMV4Dbg/zlib_wince.pch" /YX /Fo"ARMV4Dbg/" /Fd"ARMV4Dbg/" /MC /c 
"C:\pluto\trunk\CrossPlatform-Libs\zlib\zutil.c"
]
Creating command line "clarm.exe @C:\DOCUME~1\Race\LOCALS~1\Temp\RSP1D6.tmp" 
Creating temporary file "C:\DOCUME~1\Race\LOCALS~1\Temp\RSP1D7.tmp" with contents
[
/nologo /out:"ARMV4Dbg\zlib_wince.lib" 
".\ARMV4Dbg\adler32.obj"
".\ARMV4Dbg\crc32.obj"
".\ARMV4Dbg\deflate.obj"
".\ARMV4Dbg\gvmat32c.obj"
".\ARMV4Dbg\gzio.obj"
".\ARMV4Dbg\inffast.obj"
".\ARMV4Dbg\inflate.obj"
".\ARMV4Dbg\inftrees.obj"
".\ARMV4Dbg\trees.obj"
".\ARMV4Dbg\uncompr.obj"
".\ARMV4Dbg\zutil.obj"
]
Creating command line "link.exe -lib @C:\DOCUME~1\Race\LOCALS~1\Temp\RSP1D7.tmp"
<h3>Output Window</h3>
Compiling...
zutil.c
Creating library...




<h3>Results</h3>
zlib_wince.lib - 0 error(s), 0 warning(s)
</pre>
</body>
</html>
