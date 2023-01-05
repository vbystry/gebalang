FLAGS = -W -O3
.PHONY = all clean cleanall

all: calc

calc: 1.y 1.l
	bison -o calc_y.c -d 1.y
	flex -o calc_l.c 1.l
	$(CC) $(FLAGS) -o calc calc_y.c calc_l.c
	
clean:
	rm -f build/*.cpp build/*.hpp build/*.o

cleanall: clean
	rm -f calc