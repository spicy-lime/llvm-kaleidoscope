CFLAGS = -g3 -o0 -std=c++17 -lc++
LIBS = 	-lgtest -lgtest_main
INC = -I/usr/local/include/gtest

test: parser.o lexer.o
	cc -lc++ -g3 -o0 -std=c++17 driver.cpp parser.o lexer.o -o test

lib: parser.o lexer.o
	ar -r libkaleidoscope.a parser.o lexer.o

parser.o: parser.hpp lexer.hpp ast.hpp
	cc -c -g3 -o0 -std=c++17 parser.cpp -o parser.o

lexer.o: lexer.cpp
	cc -c -g3 -o0 -std=c++17 lexer.cpp -o lexer.o

driver.o: driver.cpp parser.hpp lexer.hpp
	cc -c -g3 -o0 -std=c++17 driver.cpp -o driver.o

test_lexer: lexer.o
	cc $(CFLAGS) $(INC) $(LIBS) -DBUILD_TESTS lexer.cpp -o test_lexer

clean:
	-rm *.o
	-rm *.a
	-rm *.out
