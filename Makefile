FLAGS1 = -std=c++2a -o
.PHONY = all clean cleanall

all: gebalang

gebalang: gebalang_y.ypp gebalang_l.l utils.cpp procedure.cpp 
	bison -o yacc.cpp -d gebalang_y.ypp
	flex -o lexer.cpp gebalang_l.l
	g++ -g -std=c++2a -o compiler lexer.cpp yacc.cpp utils.cpp procedure.cpp
	
clean:
	rm -f build/*.cpp build/*.hpp build/*.o

cleanall: clean
	rm -f calc