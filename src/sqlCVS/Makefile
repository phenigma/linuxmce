ROOT = ..
CXXFLAGS = -Wall -g 
CPPFLAGS = -I$(ROOT)/ -I$(ROOT)/DCE -I$(ROOT)/VIPShared -I/usr/include/mysql -D_REENTRANT -D_GNU_SOURCE -DDEBUG -DGENERATOR -DLOG_ALL_QUERIES
LDFLAGS = -L$(ROOT)/lib 
LDLIBS = -lRACommon -lDCECommon -lPlutoUtils -lSerializeClass -lpthread -lmysqlclient_r -lattr 
CXX = g++

sources = A_UpdateRow.cpp ChangedRow.cpp CommonFunctions.cpp Database.cpp Field.cpp ../sql2cpp/FieldInfo.cpp \
	R_CommitChanges.cpp R_CommitRow.cpp R_CommitTable.cpp R_GetAll_psc_id.cpp R_UpdateRepository.cpp \
	R_UpdateTable.cpp sqlCVSrequests.cpp Table.cpp Repository.cpp ValidateUser.cpp sqlCVSprocessor.cpp \
	R_CloseTransaction.cpp R_ApproveBatch.cpp
sqlCVS = $(sources) sqlCVS.cpp
new_thing = $(sources) TableInfo_SCVS.cpp

all: sqlCVS
#all: newthing

sqlCVS: $(sqlCVS:.cpp=.o)
	$(CXX) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ $(ROOT)/bin

newthing: $(new_thing:.cpp=.o)
	@echo "*** Something to do here ***"

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) sqlCVS >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
