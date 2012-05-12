all :  compiler

compiler : code_gen.o lex.o parse.o source_dump.o
	clang++ -Wall -o compiler *.o `llvm-config --ldflags --libs core`
code_gen.o : code_gen.cpp parse.hpp
	clang++ -Wall -c $< `llvm-config --cppflags`
lex.o : lex.cpp lex.hpp
	clang++ -Wall -c $< `llvm-config --cppflags`
parse.o : parse.cpp parse.hpp lex.hpp
	clang++ -Wall -c $< `llvm-config --cppflags`
source_dump.o : source_dump.cpp parse.hpp
	clang++ -Wall -c $< `llvm-config --cppflags`

clean :
	rm -rf code_gen.o lex.o parse.o source_dump.o compiler

.PHONY : all clean
