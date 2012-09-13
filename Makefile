TARGET = compiler
OBJS = codeGen.o lex.o parse.o sourceDump.o
CPP = g++
COMPILE_FLAGS = -Wall -g -c `llvm-config --cppflags`
LINK_FLAGS = -Wall `llvm-config --ldflags --libs core`

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CPP) -o $@ $^ $(LINK_FLAGS)
codeGen.o : codeGen.cpp parse.hpp
	$(CPP) $< $(COMPILE_FLAGS)
lex.o : lex.cpp lex.hpp
	$(CPP) $< $(COMPILE_FLAGS)
parse.o : parse.cpp parse.hpp lex.hpp
	$(CPP) $< $(COMPILE_FLAGS)
sourceDump.o : sourceDump.cpp parse.hpp
	$(CPP) $< $(COMPILE_FLAGS)

clean :
	rm -rf $(OBJS) $(TARGET)

.PHONY : all clean
