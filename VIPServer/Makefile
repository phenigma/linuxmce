ROOT = ..
CXXFLAGS = -Wall -g 
CPPFLAGS = -I$(ROOT)/ -I$(ROOT)/DCE -I$(ROOT)/VIPShared -I/usr/include/mysql -D_REENTRANT -D_GNU_SOURCE -DDEBUG -DGENERATOR
LDFLAGS = -L$(ROOT)/lib 
LDLIBS = -lDCECommon -lPlutoUtils -lpthread -lmysqlclient_r -lattr
CXX = g++

sources = TableInfo.cpp TableInfo_Generator.cpp DatabaseInfo.cpp CommonFunctions.cpp FieldInfo.cpp
sqlCVS = $(sources) sqlCVS.cpp
new_thing = $(sources) TableInfo_SCVS.cpp

all: sqlCVS
#all: newthing

sqlCVS: $(sqlCVS:.cpp=.o)
	$(CXX) -o $@ $+  $(LDFLAGS) $(LDLIBS)
	cp $@ $(ROOT)/bin

newthing: $(new_thing:.cpp=.o)
	@echo "*** Something to do here ***"

clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) sqlCVS >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
