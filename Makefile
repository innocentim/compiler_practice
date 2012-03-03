TARGET = compiler_practice
OBJS = lexer.o main.o parser.o code_gen.o emit_source.o
YACC_OBJ = parser.cpp
CC = clang++
YACC = yacc
YACC_FLAGS = -d -t --locations -k
COMPILE_FLAGS = -Wall -ggdb `llvm-config --cppflags`
LINK_FLAGS = `llvm-config --ldflags --libs core`

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ $(LINK_FLAGS)

$(OBJS) : %.o : %.cpp
	$(CC) -c -MMD -o $@ $< $(COMPILE_FLAGS)

$(YACC_OBJ) : %.cpp : %.y
	$(YACC) $(YACC_FLAGS) -o $@ $^

code_gen.o lexer.o : parser.cpp

clean :
	rm -f $(TARGET) $(OBJS) $(OBJS:.o=.d) $(YACC_OBJ:.cpp=.o) $(YACC_OBJ) $(YACC_OBJ:.cpp=.hpp)

-include $(OBJS:.o=.d)

.PHONY : all clean
