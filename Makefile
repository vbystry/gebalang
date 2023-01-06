FLAGS = -W -O3
.PHONY = all clean cleanall

all: gebalang

gebalang: gebalang_y.y gebalang_l.l
	bison -o yacc.cpp -d gebalang_y.y
	flex -o lexer.cpp gebalang_l.l
	$(CC++) $(FLAGS) -o gebalang yacc.cpp lexer.cpp
	
clean:
	rm -f build/*.cpp build/*.hpp build/*.o

cleanall: clean
	rm -f calc