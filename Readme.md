# Introduction
Gebalang is programming language invented for the purpose of `Formal Languages and Translation Theory` course conducted at Wroclaw University of Science and Technology.
Lecture is taught by `dr. Maciej Gębala`. Compiler provides basic optimisation.
For information about language see bottom of this file.

## Instalation
Simply clone this repo and
```
make all
```

## Usage
Compile using
```
./compiler <input_path> <compiled_path>
```
## Requirements
- GNU Bison
- Flex
- gcc 

## Files description

- `Makefile` makefile
- `gebalang_y.ypp` bison file, contains main()
- `gebalang_l.l` flex file
- `utils.hpp` file containing type declarations for the translation of procedures and the program, as well as function declarations for the translation of the program.
- `utils.cpp` file containg definitions for utils.hpp
- `procedure.hpp` file containing function declarations for the translation of procedures.
- `procedure.cpp` file containing function definitions for procedure.hpp

## Virtual Machine
Virtual machine was created by `dr. Maciej Gębala` and can parse following assembler directives:

| Directive | Interpretation                                          | Time |
|-----------|---------------------------------------------------------|------|
| GET i     | gets number from input and saves it in p_i and k <- k+1 | 100  |
| PUT i     | Prints p_i and k <- k+1                                 | 100  |
| LOAD i    | p_0 <- p_i and k <- k+1                                 | 10   |
| STORE i   | p_i <- p_0 and k <- k+1                                 | 10   |
| LOADI i   | p_0 <- p_{p_i} and k <- k+1                             | 10   |
| STOREI i  | p_{p_i} <- p_0 and k <- k+1                             | 10   |
| ADD i     | p_0 <- p_0 + p_i and k <- k+1                           | 10   |
| SUB i     | p_0 <- p_0 - p_i and k <- k+1                           | 10   |
| ADDI i    | p_0 <- p_0 + p_{p_i} and k <- k+1                       | 10   |
| SUBI i    | p_0 <- p_0 - p_{p_i} and k <- k+1                       | 10   |
| SET x     | p_0 <- x  and k <- k+1                                  | 10   |
| HALF      | p_0 <- floor{p_0 / 2} and k <- k+1                      | 5    |
| JUMP j    | k <- j                                                  | 1    |
| JPOS j    | If p_0 > 0 then k <- j else k <- k+1                    | 1    |
| JZERO j   | If p_0 = 0 then k <- j else k <- k+1                    | 1    |
| JUMPI i   | k <- p_i                                                | 1    |
| HALT      | Exit program execution                                  | 0    |

## Gebalang
Gebalang supports following grammar:

|               |                                                                       |
|---------------|-----------------------------------------------------------------------|
| program_all   | procedures main                                                       |
| procedures    | procedures PROCEDURE proc_head IS VAR declarations BEGIN commands END |
|               | procedures PROCEDURE proc_head IS BEGNI commands END                  |
|               | %empty                                                                |
| main          | PROGRAM IS VAR declarations BEGIN commands END                        |
|               | PROGRAM IS BEGIN commands END                                         |
| commands      | commands command                                                      |
|               | command                                                               |
| command       | identifier := expression;                                             |
|               | IF condition THEN commands ELSE commands ENDIF                        |
|               | IF condition THEN commands ENDIF                                      |
|               | WHILE condition DO commands ENDWHILE                                  |
|               | REPEAT commands UNTIL condition;                                      |
|               | proc_head;                                                            |
|               | READ identifier;                                                      |
|               | WRITE value;                                                          |
| proc_head     | identifier ( declarations )                                           |
| declarations  | declarations, identifier                                              |
|               | identifier                                                            |
| expression    | value                                                                 |
|               | value + value                                                         |
|               | value - value                                                         |
|               | value * value                                                         |
|               | value / value                                                         |
|               | value % value                                                         |
| condition     | value = value                                                         |
|               | value != value                                                        |
|               | value > value                                                         |
|               | value < value                                                         |
|               | value >= value                                                        |
|               | value <= value                                                        |
| value         | num                                                                   |
|               | identifier                                                            |

Sample Gebalang file looks like this:
```
[ program to calculate gcd of 4 numbers ]
PROCEDURE swap(a,b) IS
VAR c
BEGIN
  c:=a;
  a:=b;
  b:=c;
END

PROCEDURE gcd(a,b,c) IS
VAR x,y
BEGIN
  x:=a;
  y:=b;
  WHILE y!=0 DO
    IF x>=y THEN 
      x:=x-y;
    ELSE 
      swap(x,y);
    ENDIF
  ENDWHILE
  c:=x;
END

PROGRAM IS
VAR a,b,c,d,x,y,z
BEGIN
  READ a;
  READ b;
  READ c;
  READ d;
  gcd(a,b,x);
  gcd(c,d,y);
  gcd(x,y,z);
  WRITE z;
END
```
Compiled:
```
SET 1
STORE 1
SET 0
STORE 2
SET 2
STORE 3
JUMP 148
SET 0
STOREI 5
LOADI 7
STORE 16
LOADI 6
STORE 17
JZERO 14
SET 0
STORE 15
LOAD 17
SUB 16
JPOS 46
SET 1
STORE 14
LOAD 17
STORE 13
LOAD 16
SUB 13
SUB 13
JZERO 37
LOAD 14
ADD 14
STORE 14
LOAD 13
ADD 13
STORE 13
LOAD 16
SUB 13
SUB 13
JPOS 27
LOAD 14
ADD 15
STORE 15
LOAD 16
SUB 13
STORE 16
LOAD 17
SUB 16
JZERO 19
LOAD 16
STORE 8
SET 0
STORE 13
LOAD 8
SUB 13
STORE 13
SET 0
SUB 8
ADD 13
JPOS 138
LOADI 7
STORE 16
LOADI 6
STORE 17
JZERO 62
SET 0
STORE 15
LOAD 17
SUB 16
JPOS 94
SET 1
STORE 14
LOAD 17
STORE 13
LOAD 16
SUB 13
SUB 13
JZERO 85
LOAD 14
ADD 14
STORE 14
LOAD 13
ADD 13
STORE 13
LOAD 16
SUB 13
SUB 13
JPOS 75
LOAD 14
ADD 15
STORE 15
LOAD 16
SUB 13
STORE 16
LOAD 17
SUB 16
JZERO 67
LOAD 15
STOREI 7
SET 1
ADDI 5
STOREI 5
LOADI 7
STORE 16
LOADI 6
STORE 17
JZERO 104
SET 0
STORE 15
LOAD 17
SUB 16
JPOS 136
SET 1
STORE 14
LOAD 17
STORE 13
LOAD 16
SUB 13
SUB 13
JZERO 127
LOAD 14
ADD 14
STORE 14
LOAD 13
ADD 13
STORE 13
LOAD 16
SUB 13
SUB 13
JPOS 117
LOAD 14
ADD 15
STORE 15
LOAD 16
SUB 13
STORE 16
LOAD 17
SUB 16
JZERO 109
LOAD 16
STORE 8
SET 0
STORE 13
LOAD 8
SUB 13
STORE 13
SET 0
SUB 8
ADD 13
JZERO 48
JUMPI 4
GET 12
LOAD 3
STORE 9
LOAD 9
STORE 15
LOAD 9
STORE 16
SET 0
STORE 13
LOAD 16
JZERO 179
LOAD 16
HALF
STORE 14
ADD 14
STORE 14
LOAD 16
SUB 14
JZERO 170
LOAD 13
ADD 15
STORE 13
LOAD 15
ADD 15
STORE 15
LOAD 16
HALF
STORE 16
LOAD 16
JPOS 157
LOAD 13
STORE 11
LOAD 11
SUB 12
JPOS 229
SET 10
STORE 5
SET 9
STORE 6
SET 12
STORE 7
SET 192
STORE 4
JUMP 7
LOAD 10
SUB 2
JZERO 197
PUT 9
PUT 10
LOAD 9
ADD 1
STORE 9
LOAD 9
STORE 15
LOAD 9
STORE 16
SET 0
STORE 13
LOAD 16
JZERO 228
LOAD 16
HALF
STORE 14
ADD 14
STORE 14
LOAD 16
SUB 14
JZERO 219
LOAD 13
ADD 15
STORE 13
LOAD 15
ADD 15
STORE 15
LOAD 16
HALF
STORE 16
LOAD 16
JPOS 206
LOAD 13
STORE 11
LOAD 11
SUB 12
JZERO 180
LOAD 12
SUB 1
STORE 13
LOAD 1
SUB 12
ADD 13
JZERO 241
PUT 12
PUT 1
HALT
```
