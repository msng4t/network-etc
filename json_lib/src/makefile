

INC  = -I./ -I../ 
FLAG = -g -Wno-deprecated -Wformat=long -fPIC -std=c++0x

SRC_COMM = $(wildcard ./*.cpp )

OBJECT = $(SRC_COMM:.cpp=.o)

.cpp.o:
	g++ $(FLAG) $(INC) -c -o $@ $<

EXTLIB = ../libjson.a

all:$(EXTLIB)

$(EXTLIB):$(OBJECT)
	$(AR) rcs $@ $^

clean:
	rm $(EXTLIB) $(OBJECT)  







