#export SNR_CPPFLAGS="-DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES -I/opt/libxine1-pluto/include -I/opt/libsdl1.2-1.2.7+1.2.8cvs20041007/include -I/opt/libsdl1.2-1.2.7+1.2.8cvs20041007/include/SDL"
#export SNR_LDFLAGS="-L/opt/libxine1-pluto/lib -L/opt/libsdl1.2-1.2.7+1.2.8cvs20041007/lib"

export SNR_CPPFLAGS="-DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES -I/opt/libxine/include -I/opt/libsdl/include -I/opt/libsdl/include/SDL"
export SNR_LDFLAGS="-L/opt/libxine/lib -L/opt/libsdl/lib"

make clean bin
#rr
#sleep 3
#cp VDRPlugin.so /usr/pluto/bin/.

