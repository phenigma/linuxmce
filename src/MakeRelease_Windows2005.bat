SET PATH="c:\Program Files\Microsoft Visual Studio 8\Common7\IDE"

CALL SET CONFIGURATION=release
CALL SET INPUT_PATH=c:\pluto.makerelease\trunk
CALL SET MAKERELEASE_PATH=C:\pluto\trunk\src
CALL SET LOG_PATH=c:\@build_log

del %LOG_PATH%\Build_WM2005.log

%MAKERELEASE_PATH%\Debug\MakeRelease.exe -h 10.0.0.170 -o 12 -s %INPUT_PATH% -n / -c -k 119,138

cd "Orbiter\CENet_Smartphone_Treo"
call build_cab.bat












