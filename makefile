CFLAGS = -g3 -o0 -std=c++17
LIBS = 	-lc++ -lgtest -lgtest_main `llvm-config --cxxflags --ldflags --system-libs --libs core` -frtti
INC = -I/usr/local/include/gtest

test: parser lexer
	cc -lc++ -g3 -o0 -std=c++17 driver.cpp parser.o lexer.o -o test

lib: ast parser lexer
	ar -r libkaleidoscope.a parser.o lexer.o abstract-syntax-tree.o

parser: lexer
	cc -c $(CFLAGS) parser.cpp -o parser.o

lexer:
	cc -c $(CFLAGS) lexer.cpp -o lexer.o

driver:
	cc -c $(CFLAGS) driver.cpp -o driver.o

ast:
	cc -c $(CFLAGS) abstract-syntax-tree.cpp -o abstract-syntax-tree.o

test_lexer: lexer.o
	cc $(CFLAGS) $(INC) $(LIBS) -DBUILD_TESTS lexer.cpp -o test_lexer

test_parser: lexer ast
	cc $(CFLAGS) $(INC) $(LIBS) -DBUILD_TESTS parser.cpp lexer.o abstract-syntax-tree.o -o test_parser

clean:
	-rm *.o
	-rm *.a
	-rm *.out
	-rm test_parser
	-rm test_lexer
	-rm test
	-rm -r *.dSYM/
