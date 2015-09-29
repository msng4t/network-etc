
TARGET = ./UDPTest
all: $(TARGET)
OBJA :=../lib/Channel.o        ../lib/DaemonService.o  ../lib/Exception.o    ../lib/IoAcceptor.o   ../lib/Log.o           ../lib/Poll.o     ../lib/SessionManager.o  ../lib/SockDgram.o  ../lib/Thread.o          ../lib/TimeValue.o ../lib/common.o         ../lib/Dir.o            ../lib/FilterChain.o  ../lib/IoConnector.o  ../lib/Memblock.o      ../lib/Process.o  ../lib/Session.o         ../lib/SocketOpt.o  ../lib/ThreadPool.o      ../lib/TSKipList.o ../lib/Configuration.o  ../lib/EventDo.o        ../lib/Filter.o       ../lib/IoEvent.o      ../lib/MessageTable.o  ../lib/Queue.o    ../lib/SharedMutex.o     ../lib/SockPair.o   ../lib/TimeoutHandler.o ../lib/Cycle.o          ../lib/EventPosted.o    ../lib/GetOpt.o       ../lib/IoService.o    ../lib/OsAction.o      ../lib/RBTree.o   ../lib/SockAddr.o        ../lib/Synch.o      ../lib/Timer.o
LIBS+= -lrt
#CXX = purify -follow-child-processes -max_threads=500 g++
CXX+= -O0 -g -Wall -pipe -DDEBUG
CXXFLAGS += -I /usr/local/BerkeleyDB.6.0/include -I../lib
LIBS += /usr/local/BerkeleyDB.6.0/lib/libdb_cxx-6.0.a ../libmina.a -lrt
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
	$(LINKER) $^ $(LIBS) -o $(TARGET);

#####################################################################################################
clean:;
	@$(RM) ../*.o ../bin/* $(TARGET_OBJS) ../*/*.o $(TARGET)

