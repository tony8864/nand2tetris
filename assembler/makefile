OBJECTS = scanner.o parser.o parser_util.o symbol_table.o

PARSER_UTIL_C = parser_util/parser_util.c
SYM_TAB_C = symbol_table/symbol_table.c

exe: ${OBJECTS}
	gcc -o exe ${OBJECTS}

scanner.o: scanner.c
	gcc -c $< -o $@

scanner.c: scanner.l parser.tab.h
	flex --outfile=scanner.c $<

parser.o: parser.tab.c parser_util.o
	gcc -c $< -o $@

parser.tab.c parser.tab.h: parser.y
	bison -d $<

parser_util.o: ${PARSER_UTIL_C} symbol_table.o
	gcc -c $< -o $@

symbol_table.o: ${SYM_TAB_C}
	gcc -c $< -o $@

clean:
	rm -f exe scanner.c ${OBJECTS} parser.tab.c parser.tab.h