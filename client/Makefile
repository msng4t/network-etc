
TARGET = ./Client
all: $(TARGET)

LIBS+= -lrt
#CXX = purify -follow-child-processes -max_threads=500 g++
CXX+= -O0 -g -Wall -pipe -DDEBUG
CXXFLAGS += -I /usr/local/BerkeleyDB.6.0/include -I../lib -I../BDB
LIBS += /usr/local/BerkeleyDB.6.0/lib/libdb_cxx-6.0.a ../libmina.a
LINKER = g++
ECHO = echo

#####################################################################################################
MAIN_SRCS = $(shell ls *.cpp )
MAIN_OBJS = $(subst .cpp,.o,$(MAIN_SRCS))

####################################################################################################
${MAIN_OBJS}: %.o:./%.cpp
	${CXX} ${CXXFLAGS} -c ./${subst .o,.cpp,$@} -o $@

$(TARGET): $(MAIN_OBJS)
	@echo "[============= LINK ==============]"
	$(LINKER) $^ $(LIBS) -o $(TARGET) -lrt;

#####################################################################################################
clean:;
	@$(RM) ../*.o ../bin/* $(TARGET_OBJS) ../*/*.o $(TARGET)

