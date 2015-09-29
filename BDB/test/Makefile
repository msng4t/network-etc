

LIBS+= -lrt -llua
CXXFLAGS += -I /usr/local/BerkeleyDB.6.0/include
CXXFLAGS += -I ../

DEPLIBS += /usr/local/BerkeleyDB.6.0/lib/libdb_cxx-6.0.a
#CXX = purify -follow-child-processes -max_threads=500 g++
CXX+= -O0 -g -Wall -pipe -DDEBUG
LINKER = g++

#####################################################################################################
MAIN_SRCS = Bulk.cpp
MAIN_OBJS = $(subst .cpp,.o,$(MAIN_SRCS))

TARGET = $(subst .cpp,,$(MAIN_SRCS))
all: $(TARGET)

####################################################################################################
${MAIN_OBJS}: %.o:./%.cpp
	${CXX} ${CXXFLAGS} -c ./${subst .o,.cpp,$@} -o $@

#$(TARGET): $(MAIN_OBJS) $(DPLIBS)
$(TARGET):  %:./%.o
	@echo "[============= LINK ==============]"
	$(LINKER) $^ $(DEPLIBS) $(LIBS) -o $@;

#####################################################################################################
clean:;
	@$(RM) *.o $(TARGET)


