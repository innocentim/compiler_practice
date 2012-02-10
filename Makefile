TARGET = compiler_practice
OBJS = lexer.o main.o parser.o code_gen.o emit_source.o
YACC_OBJ = parser.cpp
CC = clang++
YACC = yacc
YACC_FLAGS = -d
COMPILE_FLAGS = -Wall -ggdb -Qunused-arguments -DNDEBUG -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS
LINK_FLAGS = -L/usr/lib/llvm -lpthread -lffi -ldl -lm -lLLVMCore -lLLVMSupport

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ $(LINK_FLAGS)

$(OBJS) : %.o : %.cpp
	$(CC) -c -MMD -o $@ $< $(COMPILE_FLAGS)

$(YACC_OBJ) : %.cpp : %.y
	$(YACC) $(YACC_FLAGS) -o $@ $^

lexer.o : parser.cpp

clean :
	rm -f $(TARGET) $(OBJS) $(OBJS:.o=.d) $(YACC_OBJ:.cpp=.o) $(YACC_OBJ) $(YACC_OBJ:.cpp=.hpp)

-include $(OBJS:.o=.d)

.PHONY : all clean
