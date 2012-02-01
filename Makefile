TARGET = compiler_practice
OBJS = .checker.o .main.o .misc.o .parser.o .source.o .target.o .token.o
CC = g++
CPPFLAGS = -Wall `llvm-config --cppflags`

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CPPFLAGS) -o $@ $(OBJS)

.%.o: %.cpp
	$(CC) $(CPPFLAGS) -c -MMD -o $@ $<; \
		sed -i -e 's/^\(.*\)\.o:/\1.o $(@:.o=.d):/g' $(@:.o=.d)

clean :
	rm -f $(TARGET) $(OBJS) $(OBJS:.o=.d)

-include $(OBJS:.o=.d)

.PHONY : all clean
