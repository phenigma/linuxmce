#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -DDEBUG
LDFLAGS = -L$(ROOT)/lib
LDLIBS =  -lpthread -lSerializeClass -lPlutoUtils -lDCECommon -lmysqlclient_r

MYNAME = Generic_Input_Ouput
VERSION = 2.0.0
MYDEVTEMPLATEID = 43mplateID

Gen_Dev_Files = $(GENDEV)/Generic_Input_OuputBase.cpp
sources = Main.cpp Generic_Input_Ouput.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
LDFLAGS +=
LDLIBS +=

Gen_Dev_Files +=
sources +=

#<-dceag-mf-b->
whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Generic_Input_Ouput

so: Generic_Input_Ouput.so

Generic_Input_Ouput.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Generic_Input_Ouput: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) Generic_Input_Ouput.bin Generic_Input_Ouput.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
#<-dceag-mf-e->

