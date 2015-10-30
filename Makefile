LEX=flex
YACC=bison
CC=g++
OBJECT=main
INTERN=code

final: $(INTERN) $(OBJECT)
	@./$(OBJECT) < $(INTERN)

$(INTERN): preprocessing
	@./preprocessing < test.c > $(INTERN)

$(OBJECT): lex.yy.o  yacc.tab.o
	$(CC) lex.yy.o yacc.tab.o -o $(OBJECT)

lex.yy.o: lex.yy.c  yacc.tab.h  main.h
	$(CC) -c lex.yy.c -std=c++0x

yacc.tab.o: yacc.tab.c  main.h
	$(CC) -c yacc.tab.c

yacc.tab.c  yacc.tab.h: yacc.y
	$(YACC) -d -o yacc.tab.c yacc.y

lex.yy.c: parsing.lex
	$(LEX) -o lex.yy.c parsing.lex

preprocessing: preprocessing.lex.yy.c
	$(CC) preprocessing.lex.yy.c -o preprocessing -ll

preprocessing.lex.yy.c: preprocessing.lex
	$(LEX) -o preprocessing.lex.yy.c preprocessing.lex

clean:
	@rm -f $(OBJECT) $(INTERN) preprocessing *.o *.tab.* *.yy.*