CXXFLAGS = -Wall -g
CPPFLAGS = -I../../level4 -I../../../level4 -I../

sources = SerialPort.cpp

all: libOCSerial.so

libOCSerial.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $+
	cp $@ ../../lib

clean:
	rm -f *.o *.d libOCSerial.a >/dev/null

include ../make.rules
-include $(sources:.cpp=.d)
