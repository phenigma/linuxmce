SET PATH="c:\Program Files\Microsoft Visual Studio .NET 2003\Common7\IDE"

CALL SET CONFIGURATION=debug
CALL SET INPUT_PATH=c:\pluto.makerelease\trunk
CALL SET MAKERELEASE_PATH=C:\pluto\trunk\src
CALL SET LOG_PATH=c:

del %LOG_PATH%\Build_Windows.log

ren %INPUT_PATH%\pluto.sln pluto.sln.temp
%MAKERELEASE_PATH%\bin\MakeRelease.exe -h 10.0.0.150 -o 7 -s %INPUT_PATH% -n / -c -k 192,193,213,212,201,237,255,254,277,278,116,135,126,145,226,227,119,138,124,143,241,242,154,155,159,160,202,203,118,137,303,304
ren %INPUT_PATH%\pluto.sln.temp pluto.sln








