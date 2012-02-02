TARGET = compiler_practice
OBJS = checker.o main.o misc.o parser.o source.o target.o token.o
CC = clang++
CFLAGS = -Wall `llvm-config --cppflags`
LFLAGS = -WALL `llvm-config --ldflags --libs core`

all : $(TARGET)

$(TARGET) : $(OBJS)
	@$(CC) -o $@ $(OBJS) $(LFLAGS)

$(OBJS) :
	@$(CC) $(CFLAGS) -c -MMD $(@:.o=.cpp); \
	sed -i -e 's/^\(.*\)\.o:/\1.o $(@:.o=.d):/g' $(@:.o=.d)

clean :
	rm -f $(TARGET) $(OBJS) $(OBJS:.o=.d)

-include $(OBJS:.o=.d)

.PHONY : all clean
