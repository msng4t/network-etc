
TARGET = ./libbdb.a
all: $(TARGET)

LIBS+= -lrt -llua -l../libmina.a
CXXFLAGS += -I /usr/local/BerkeleyDB.6.0/include
CXXFLAGS += -I ../lib/

DEPLIBS += /usr/local/BerkeleyDB.6.0/lib/libdb_cxx-6.0.a
#CXX = purify -follow-child-processes -max_threads=500 g++
CXX+= -O0 -g -Wall -pipe -DDEBUG
LINKER = g++

#####################################################################################################
MAIN_SRCS = $(shell ls *.cpp )
MAIN_OBJS = $(subst .cpp,.o,$(MAIN_SRCS))
target_SRCS = $(shell ls *.c )
target_OBJS = $(subst .c,.o,$(target_SRCS))

####################################################################################################
${MAIN_OBJS}: %.o:./%.cpp
	${CXX} ${CXXFLAGS} -c ./${subst .o,.cpp,$@} -o $@

$(TARGET): $(MAIN_OBJS) $(target_OBJS) $(DPLIBS)
	@echo "[============= LINK ==============]"
	ar cr ./../$(TARGET) *.o

#####################################################################################################
clean:;
	@$(RM) *.o ../$(TARGET)

check:
	$(LINT) $(CXXFLAGS) $(MAIN_SRCS) 1>check.rst

depend:
	makedepend $(CXXFLAGS) $(MAIN_SRCS) $(COMMON_SRCS)


# DO NOT DELETE

