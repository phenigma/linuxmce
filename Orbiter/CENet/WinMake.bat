evc Orbiter.vcp /OUT %LOG_PATH%\BuildCE.log /make /rebuild 

cd ARMV4Dbg
copy Orbiter.exe "../../../bin/OrbiterCE.exe"

