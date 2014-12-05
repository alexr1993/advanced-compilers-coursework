## TODO Clean up all the repitition in this file
EVALOBJS = evaluation/param.o evaluation/function.o evaluation/environment.o evaluation/evaluate.o
EVALSRCS = evaluation/param.c evaluation/function.c evaluation/environment.c evaluation/evaluate.c

ANALOBJS = analysis/lex.yy.o analysis/C.tab.o analysis/symbol_table.o analysis/nodes.o
ANALSRCS = analysis/lex.yy.c analysis/C.tab.c analysis/symbol_table.c analysis/nodes.c

COMMOBJS = common/operations.o
COMMSRCS = common/operations.o

TESTOBJS = util.o tests/tests.o
TESTSRCS = util.c tests/tests.c

OBJS = util.o main.o
SRCS = util.c main.c

CC = gcc

all:	mycc tests

clean:
	rm ${ANALOBJS} ${EVALOBJS} ${OBJS} ${COMMOBJS}

mycc:	 ${ANALOBJS} ${EVALOBJS} ${OBJS} ${COMMOBJS}
	${CC} -g -o mycc -I . ${ANALOBJS} ${EVALOBJS} ${OBJS} ${COMMOBJS}

# Generate lexical analyser with flex
lex.yy.c: analysis/C.flex
	flex analysis/C.flex

# Generate parser with bison
C.tab.c:	analysis/C.y
	bison -d -t -v analysis/C.y

.c.o:
	${CC} -Wall -g -c -I . -o $@ $*.c

# FIXME
depend:
	${CC} -M $(ANALSRCS) $(EVALSRCS) $(SRCS)$(COMMOBJS) > .deps
	cat Makefile .deps > makefile

# FIXME
dist:	symbol_table.c nodes.c util.c main.c Makefile C.flex C.y nodes.h token.h
	tar cvfz mycc.tgz symbol_table.c nodes.c util.c main.c Makefile C.flex C.y \
		nodes.h token.h

tests: ${ANALOBJS} ${EVALOBJS} ${TESTOBJS} ${COMMOBJS}
	${CC} -g -o run_tests ${ANALOBJS} ${EVALOBJS} ${TESTOBJS} ${COMMOBJS} `pkg-config --cflags --libs check`
