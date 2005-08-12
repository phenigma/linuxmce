SET PATH="c:\Program Files\Microsoft eMbedded C++ 4.0\Common\EVC\Bin"

CALL SET CONFIGURATION=release
CALL SET INPUT_PATH=c:\pluto.makerelease\trunk
CALL SET MAKERELEASE_PATH=C:\pluto\trunk\src
CALL SET LOG_PATH=c:

del %LOG_PATH%\BuildCE.log

%MAKERELEASE_PATH%\Debug\MakeRelease.exe -h 10.0.0.170 -o 12 -s %INPUT_PATH% -n / -c -k 119,138

cd "Orbiter\CENet_Smartphone"
call build_cab.bat










