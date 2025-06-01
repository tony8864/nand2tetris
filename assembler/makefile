CLFAGS = -Isrc -Isrc/parser_util -Isrc/symbol_table

OBJECTS = build/parser.o build/scanner.o build/parser_util.o build/symbol_table.o

all: build/assembler

build/assembler: ${OBJECTS}
	gcc -o build/assembler build/*.o

build/parser_util.o: src/parser_util/parser_util.c
	gcc ${CLFAGS} -c $< -o $@

build/symbol_table.o: src/symbol_table/symbol_table.c
	gcc ${CLFAGS} -c $< -o $@

build/parser.o: build/parser.tab.c
	gcc ${CLFAGS} -c $< -o $@

build/scanner.o: build/scanner.c
	gcc ${CLFAGS} -c $< -o $@

build/parser.tab.c build/parser.tab.h: src/parser.y
	mkdir -p build
	bison -d -o build/parser.tab.c $<

build/scanner.c: src/scanner.l
	flex --outfile=build/scanner.c $<

clean:
	rm -rf build