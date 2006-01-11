ROOT = ..
CXX = g++
CXXFLAGS = -Wall -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I$(ROOT)/Serial -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
EXTERNAL_LIBS_NAMES = pthread mysqlclient_r
INTERNAL_LIBS_NAMES = SerializeClass DCECommon Serial 

MYNAME = ZWaveTest
VERSION = 1.0.0
MYDEVTEMPLATEID = 5mplateID

CXXFLAGS += 
CPPFLAGS +=
LDFLAGS += -L$(ROOT)/lib -L/usr/lib/
EXTERNAL_LIBS_NAMES += SerializeClass DCECommon Serial PlutoUtils

sources =  PlutoZWSerialAPI.cpp ZWJobGetInitData.cpp ZWJobGetVersion.cpp ZWJobSwitchBinaryGet.cpp \
	ZWaveNode.cpp SerialConnection.cpp ZWJobGetNodeProtocolInfo.cpp ZWJobInitialize.cpp \
	ZWJobSwitchChangeLevel.cpp ZWJobGetID.cpp ZWJobGetSUC.cpp ZWJobLightLevel.cpp \
	ZWaveJob.cpp ZWaveTest.cpp  ZWJobReset.cpp ZWJobReceive.cpp \
	ZWJobSetLearnNodeState.cpp ZWJobAddNode.cpp ZWJobRemoveNode.cpp

LDLIBS_NAMES = $(EXTERNAL_LIBS_NAMES)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: ZWaveTest

so: ZWaveTest.so

ZWaveTest.so: $(sources:.cpp=.o) $(INTERNAL_LIBS_NAMES:%=$(ROOT)/lib/lib%.so) Makefile
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$(sources:.cpp=.o)) $(LDFLAGS) $(LDLIBS_NAMES:%=-l%)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@

ZWaveTest: $(sources:.cpp=.o) $(INTERNAL_LIBS_NAMES:%=$(ROOT)/lib/lib%.so) Makefile
	$(CXX) -o $(MYNAME) $(call cpath,$(sources:.cpp=.o)) $(LDFLAGS) $(LDLIBS_NAMES:%=-l%)
#	mv ../bin/$@ ../bin/$@.bak
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) ZWaveTest.bin ZWaveTest.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
