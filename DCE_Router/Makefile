#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
MYNAME = DCE_Router
CXXFLAGS = -Wall -g -fPIC
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -DDEBUG
LDFLAGS = -L$(ROOT)/lib
LDLIBS =  -lpthread -lDCECommon -lmysqlclient_r

VERSION = 2.0.0

Gen_Dev_Files = $(GENDEV)/DCE_RouterBase.cpp
sources = Main.cpp DCE_Router.cpp $(Gen_Dev_Files)
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

bin: DCE_Router

so: DCE_Router.so

DCE_Router.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@
		
DCE_Router: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) DCE_Router.bin DCE_Router.so >/dev/null

package:
	./make_package.sh $(MYNAME) $(VERSION)

include $(ROOT)/DCE/make.rules
-include $(sources:.cpp=.d)
#<-dceag-mf-e->

