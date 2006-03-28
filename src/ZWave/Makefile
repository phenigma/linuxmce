#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread

MYNAME = ZWave
VERSION = 2.0.0
MYDEVTEMPLATEID = 1754mplateID

Gen_Dev_Files = $(GENDEV)/ZWaveBase.cpp
sources = Main.cpp ZWave.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
LDFLAGS +=
XTRALDLIBS = -lSerial

Gen_Dev_Files +=
sources += \
	PlutoZWSerialAPI.cpp ZWJobGetInitData.cpp ZWJobGetVersion.cpp ZWJobSwitchBinaryGet.cpp \
	ZWaveNode.cpp SerialConnection.cpp ZWJobGetNodeProtocolInfo.cpp ZWJobInitialize.cpp \
	ZWJobSwitchChangeLevel.cpp ZWJobGetID.cpp ZWJobGetSUC.cpp ZWJobLightLevel.cpp \
	ZWaveJob.cpp ZWJobReset.cpp ZWJobReceive.cpp ZWJobPool.cpp \
	ZWJobSetLearnNodeState.cpp ZWJobAddNode.cpp ZWJobRemoveNode.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: ZWave

so: ZWave.so

ZWave.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
ZWave: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) ZWave.bin ZWave.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

