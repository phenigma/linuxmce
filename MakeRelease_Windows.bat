SET PATH="c:\Program Files\Microsoft Visual Studio .NET 2003\Common7\IDE"

CALL SET CONFIGURATION=debug
CALL SET INPUT_PATH=c:\pluto.makerelease\trunk
CALL SET MAKERELEASE_PATH=C:\pluto\trunk
CALL SET LOG_PATH=c:

del %LOG_PATH%\Build_Windows.log

ren %INPUT_PATH%\pluto.sln pluto.sln.temp
%MAKERELEASE_PATH%\bin\MakeRelease.exe -h 10.0.0.150 -o 7 -s %INPUT_PATH% -n / -v 2
ren %INPUT_PATH%\pluto.sln.temp pluto.sln








