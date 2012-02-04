TARGET = compiler_practice
OBJS = token.o main.o misc.o parser.o source.o target.o
CC = clang++
COMPILE_FLAGS = -Wall -Qunused-arguments -ggdb $(shell llvm-config --cppflags --ldflags --libs core) 

all : $(TARGET)

$(TARGET) : $(OBJS)
	@$(CC) -o $@ $(OBJS) $(COMPILE_FLAGS)

$(OBJS) :
	@$(CC) -c -MMD $(@:.o=.cpp) $(COMPILE_FLAGS)

clean :
	rm -f $(TARGET) $(OBJS) $(OBJS:.o=.d)

-include $(OBJS:.o=.d)

.PHONY : all clean
