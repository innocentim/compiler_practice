TARGET = compiler_practice
OBJS = lexer.o main.o parser.o code_gen.o emit_source.o
YACC_OBJ = parser.cpp
CC = g++
YACC = yacc
YACC_FLAGS = -d -t
COMPILE_FLAGS = -Wall -g `llvm-config --cppflags`
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
	rm -f $(TARGET) $(OBJS) $(OBJS:.o=.d) $(YACC_OBJ:.cpp=.o) $(YACC_OBJ) $(YACC_OBJ:.cpp=.h)

-include $(OBJS:.o=.d)

.PHONY : all clean
