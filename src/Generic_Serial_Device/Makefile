#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread

MYNAME = Generic_Serial_Device
VERSION = 2.0.0
MYDEVTEMPLATEID = 69mplateID

Gen_Dev_Files = $(GENDEV)/Generic_Serial_DeviceBase.cpp
sources = Main.cpp Generic_Serial_Device.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/lib/ruby/1.8/i386-linux
LDFLAGS +=
XTRALDLIBS = -lruby1.8 -lSerial -lpluto_main -lrt -lMessageTranslation

Gen_Dev_Files +=
sources += $(ROOT)/Serial/GenericIODevice.cpp FileCodeSupplier.cpp IOEvent.cpp IOMutex.cpp IOPool.cpp IOThread.cpp LoopStateMachine.cpp RubyEmbeder.cpp RubyEmbededClass.cpp RubyExceptions.cpp RubyStdStreamRedirector.cpp SerialInterceptorCodeSupplier.cpp SerialIOConnection.cpp NetworkIOConnection.cpp SerialIOPool.cpp NetworkIOPool.cpp StateMachine.cpp TestEmbededClass.cpp TestRubyEmbederCodeSupplier.cpp RubySerialIOWrapper.cpp RubyDeviceWrapper.cpp RubyDCECodeSupplier.cpp RubyDCEEmbededClass.cpp RubyIOManager.cpp RubyIOPool.cpp RubyCommandWrapper.cpp BufferedIOConnection.cpp RubySerialIOConnectionWrapper.cpp IOUtils.cpp

#<-dceag-mf-b->!
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Generic_Serial_Device ruby

so: Generic_Serial_Device.so ruby

ruby: Ruby_$(MYNAME).so

Generic_Serial_Device.so: $(sources:.cpp=.o) 
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Generic_Serial_Device: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@


Ruby_$(MYNAME).so: Ruby_$(MYNAME)_wrap.o 
	ln -s /usr/lib/libtcl8.4.so.0 /usr/lib/libtcl8.4.so | true
	$(CXX) -shared -o Ruby_$(MYNAME).so $(LDFLAGS) $(LDLIBS) Ruby_$(MYNAME)_wrap.o RubySerialIOWrapper.o RubyDeviceWrapper.o RubyCommandWrapper.o RubySerialIOConnectionWrapper.o BufferedIOConnection.o -ltcl8.4 -lstdc++
	cp $@ ../bin
	
Ruby_$(MYNAME)_wrap.o: Ruby_$(MYNAME)_wrap.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) Ruby_$(MYNAME)_wrap.cxx RubySerialIOWrapper.cpp RubyDeviceWrapper.cpp RubyCommandWrapper.cpp RubySerialIOConnectionWrapper.cpp BufferedIOConnection.cpp -I/usr/include/tcl8.4 

Ruby_Generic_Serial_Device_wrap.cxx:
	swig -ruby -c++ Ruby_Generic_Serial_Device.i

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Generic_Serial_Device.bin Generic_Serial_Device.so *.cxx >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

