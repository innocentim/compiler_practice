compiler : checker.o main.o misc.o parser.o source.o target.o token.o
	clang++ -o compiler checker.o main.o misc.o parser.o source.o target.o token.o

main.o : main.cpp common.hpp token.hpp parser.hpp checker.hpp
	clang++ -c main.cpp

misc.o : misc.cpp misc.hpp
	clang++ -c misc.cpp

parser.o : parser.cpp misc.hpp common.hpp token.hpp parser.hpp
	clang++ -c parser.cpp

checker.o : checker.cpp misc.hpp common.hpp checker.hpp
	clang++ -c checker.cpp

source.o : source.cpp common.hpp misc.hpp parser.hpp
	clang++ -c source.cpp

target.o : target.cpp common.hpp misc.hpp parser.hpp
	clang++ `llvm-config --cxxflags` -c target.cpp

token.o : token.cpp misc.hpp token.hpp
	clang++ -c token.cpp

clean :
	rm compiler *.o
