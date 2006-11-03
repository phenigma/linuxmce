SET PATH="c:\Program Files\Microsoft Visual Studio 8\Common7\IDE"

devenv.exe Orbiter.sln /rebuild DEBUG /out %LOG_PATH%\Build_WM2005.log 

rem Temporary use the debug version
copy "..\..\Debug\OrbiterTreo.exe" "..\..\bin"

SET PATH="c:\Program Files\Microsoft eMbedded C++ 4.0\Common\EVC\Bin"


