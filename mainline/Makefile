TARGET = compiler
OBJS = code_gen.o lex.o parse.o source_dump.o
CPP = clang++
COMPILE_FLAGS = -Wall -c `llvm-config --cppflags`
LINK_FLAGS = -Wall `llvm-config --ldflags --libs core`

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CPP) -o $@ $^ $(LINK_FLAGS)
code_gen.o : code_gen.cpp parse.hpp
	$(CPP) $< $(COMPILE_FLAGS)
lex.o : lex.cpp lex.hpp
	$(CPP) $< $(COMPILE_FLAGS)
parse.o : parse.cpp parse.hpp lex.hpp
	$(CPP) $< $(COMPILE_FLAGS)
source_dump.o : source_dump.cpp parse.hpp
	$(CPP) $< $(COMPILE_FLAGS)

clean :
	rm -rf $(OBJS) $(TARGET)

.PHONY : all clean
