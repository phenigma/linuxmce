@echo on

echo Checking out pluto sources...
cd "c:/Program Files/Subversion/bin"
svn co svn://10.0.0.150/pluto2/trunk  /pluto.makerelease/trunk --username cmiron --password cmiron

cd "c:/pluto.makerelease/trunk"

echo Building pluto(windows) sources...
call "MakeRelease_Windows.bat"

echo Building pluto(symbian) sources...
call "MakeRelease_WindowsCE.bat"

echo Building pluto(windows ce) sources...
call "MakeRelease_Symbian_S60.bat"

