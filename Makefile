TARGET = compiler_practice
OBJS = checker.o main.o misc.o parser.o source.o target.o token.o
CC = clang++
CPPFLAGS = -Wall `llvm-config --cppflags` -Qunused-arguments

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CPPFLAGS) -o $@ $(OBJS)

$(OBJS) :
	$(CC) $(CPPFLAGS) -c -MMD $(@:.o=.cpp); \
	sed -i -e 's/^\(.*\)\.o:/\1.o $(@:.o=.d):/g' $(@:.o=.d)

clean :
	rm -f $(TARGET) $(OBJS) $(OBJS:.o=.d)

-include $(OBJS:.o=.d)

.PHONY : all clean
