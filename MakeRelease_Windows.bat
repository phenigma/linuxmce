SET PATH="c:\Program Files\Microsoft Visual Studio .NET 2003\Common7\IDE"

del %LOG_PATH%\Build_Windows.log

CALL SET CONFIGURATION=debug
CALL SET INPUT_PATH=c:\pluto\trunk
CALL SET LOG_PATH=c:

ren %INPUT_PATH%\pluto.sln pluto.sln.temp

%INPUT_PATH%\bin\MakeRelease.exe -h 10.0.0.150 -o 7 -s %INPUT_PATH% -n / -v 2

ren %INPUT_PATH%\pluto.sln.temp pluto.sln








