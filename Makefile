TARGET = compiler_practice
OBJS = lexer.o main.o misc.o parser.o source.o target.o
CC = clang++
COMPILE_FLAGS = -Wall -ggdb -Qunused-arguments -DNDEBUG -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS
LINK_FLAGS = -L/usr/lib/llvm -lpthread -lffi -ldl -lm -lLLVMCore -lLLVMSupport

all : $(TARGET)

$(TARGET) : $(OBJS)
	@$(CC) -o $@ $^ $(LINK_FLAGS)

$(OBJS) :
	@$(CC) -c -MMD -o $@ $(@:.o=.cpp) $(COMPILE_FLAGS)

clean :
	rm -f $(TARGET) $(OBJS) $(OBJS:.o=.d)

-include $(OBJS:.o=.d)

.PHONY : all clean
