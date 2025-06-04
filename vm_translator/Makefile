CLFAGS = -Isrc -Isrc/parser_util -Isrc/codegen

OBJECTS = 	build/parser.o \
			build/scanner.o \
			build/parser_util.o \
			build/memory_codegen.o

all: build/vmtranslator

build/vmtranslator: ${OBJECTS}
	gcc -o build/vmtranslator build/*.o

build/parser.o: build/parser.tab.c
	gcc ${CLFAGS} -c $< -o $@

build/scanner.o: build/scanner.c
	gcc ${CLFAGS} -c $< -o $@

build/memory_codegen.o: src/codegen/memory_codegen.c
	gcc ${CLFAGS} -c $< -o $@

build/parser_util.o: src/parser_util/parser_util.c src/codegen/memory_codegen.h build/memory_codegen.o
	gcc ${CLFAGS} -c $< -o $@

build/parser.tab.c build/parser.tab.h: src/parser.y
	mkdir -p build
	bison -d -o build/parser.tab.c $<

build/scanner.c: src/scanner.l
	flex --outfile=build/scanner.c $<

clean:
	rm -rf build *.asm