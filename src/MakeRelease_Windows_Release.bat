SET PATH="c:\Program Files\Microsoft Visual Studio .NET 2003\Common7\IDE"

CALL SET CONFIGURATION=release
CALL SET INPUT_PATH=c:\pluto.makerelease\trunk
CALL SET MAKERELEASE_PATH=C:\pluto\trunk\src
CALL SET LOG_PATH=c:

del %LOG_PATH%\Build.log

ren %INPUT_PATH%\src\pluto.sln pluto.sln.temp
%MAKERELEASE_PATH%\bin\MakeRelease.exe -h 10.0.0.170 -o 7 -s %INPUT_PATH% -n / -c -k 119,138
ren %INPUT_PATH%\src\pluto.sln.temp pluto.sln

echo Updating Orbiter release binary for OrbiterInstaller
copy C:\pluto.makerelease\trunk\src\release\Orbiter.exe C:\pluto.makerelease\trunk\installers\Installers\Orbiter\Files\

cd C:\pluto.makerelease\trunk\installers\Installers\Orbiter
CALL WinMake.bat








