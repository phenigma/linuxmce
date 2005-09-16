del c:\$Installers\$OrbiterCE\*.* /Q

cd
cd ..\..\bin
copy OrbiterCE.exe "c:\$Installers\$OrbiterCE\"
cd "..\Orbiter\CENet\Misc files"

cd Misc
copy AYGSHELL.DLL c:\$Installers\$OrbiterCE\
copy PthreadsCE.dll c:\$Installers\$OrbiterCE\
copy logo.gif c:\$Installers\$OrbiterCE\
cd ..\..\CENet_PocketFrog

"C:\Program Files\Windows CE Tools\wce420\POCKET PC 2003\Tools\Cabwiz.exe" OrbiterCE.inf

move OrbiterCE.cab "..\..\bin"