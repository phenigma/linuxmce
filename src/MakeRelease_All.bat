@echo on

echo Deleting old pluto.makerelease folder...
rmdir "/pluto.makerelease" /S /Q

echo Checking out pluto sources...
cd "c:\Program Files\Subversion\bin"
svn co http://10.0.0.170/pluto/trunk/src /pluto.makerelease/trunk/src
svn co http://10.0.0.170/pluto/trunk/libs /pluto.makerelease/trunk/libs
svn co http://10.0.0.170/pluto/trunk/installers/Installers /pluto.makerelease/trunk/installers/Installers

cd "c:\pluto.makerelease\trunk\src"

echo Building pluto(windows ce) sources...
call "MakeRelease_Symbian_S60.bat"

echo Building pluto(windows) sources...
call "MakeRelease_Windows.bat"

echo Building pluto(symbian) sources...
call "MakeRelease_WindowsCE.bat"

rem echo Building pluto(windows) sources, release version... (for now, only Orbiter)
rem call "MakeRelease_Windows_Release.bat"

echo Misc actions...
cd "c:\pluto.makerelease\trunk\src\bin"
copy OrbiterCE.exe "c:\pluto.makerelease\trunk\src\release"
copy OrbiterCE_SDL.exe "c:\pluto.makerelease\trunk\src\release"
copy *.sis "c:\pluto.makerelease\trunk\src\release"

cd "c:\pluto.makerelease\trunk\src\release"
copy Orbiter.exe Orbiter_Win32.dat
copy OrbiterCE.exe Orbiter_WinCE.dat

echo Copying EXE files...
copy *.exe "\\10.0.0.150\builds\Windows_Output\src\release"
echo Copying DLL files...
copy *.dll "\\10.0.0.150\builds\Windows_Output\src\release"
echo Copying DAT files...
copy *.dat "\\10.0.0.150\builds\Windows_Output\src\release"
echo Copying SIS files...
copy *.sis "\\10.0.0.150\builds\Windows_Output\src\release"

echo Copying LIB files...
cd "c:\pluto.makerelease\trunk\src\lib"
copy *.lib "\\10.0.0.150\builds\Windows_Output\src\lib"

echo Copying MSI files...
cd C:\pluto.makerelease\trunk\installers\Installers\Orbiter\Release\
copy *.msi "\\10.0.0.150\builds\Windows_Output\src\release"


