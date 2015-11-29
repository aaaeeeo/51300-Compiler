LEX=flex
YACC=bison
CC=g++
PP=cpp
OBJECT=main
INTERN=code
SOURCE=test.c
ASM=out.s
BIN=out
LIB=COMPILER_EXAMPLE/lib/lib.o

final: $(BIN)
	@./$(BIN)

$(BIN): $(ASM)
	gcc -m32 -o $(BIN) $(ASM) $(LIB)

$(ASM): $(INTERN) $(OBJECT)
	@./$(OBJECT) < $(INTERN) > $(ASM)

$(INTERN): preprocessing $(SOURCE)
	$(PP) $(SOURCE) > $(INTERN)

$(OBJECT): lex.yy.o  yacc.tab.o
	$(CC) lex.yy.o yacc.tab.o main.o -o $(OBJECT)

lex.yy.o: lex.yy.c  yacc.tab.h  main.h
	$(CC) -c lex.yy.c -std=c++11

yacc.tab.o: yacc.tab.c  main.h main.cpp
	$(CC) -c yacc.tab.c main.cpp -std=c++11

yacc.tab.c  yacc.tab.h: yacc.y 
	$(YACC) -d -o yacc.tab.c yacc.y

lex.yy.c: parsing.lex
	$(LEX) -o lex.yy.c parsing.lex

preprocessing: preprocessing.lex.yy.c
	$(CC) preprocessing.lex.yy.c -o preprocessing -ll

preprocessing.lex.yy.c: preprocessing.lex
	$(LEX) -o preprocessing.lex.yy.c preprocessing.lex

clean:
	@rm -f $(OBJECT) $(INTERN) $(ASM) $(BIN) preprocessing *.o *.tab.* *.yy.* 
