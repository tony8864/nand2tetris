CLFAGS = -Isrc -Isrc/parser_util -Isrc/codegen/include -Isrc/file_util

OBJECTS = 	build/parser.o \
			build/scanner.o \
			build/parser_util.o \
			build/memory_codegen.o \
			build/compute_codegen.o \
			build/file_util.o \
			build/common.o

all: build/vmtranslator

build/vmtranslator: ${OBJECTS}
	gcc -o build/vmtranslator build/*.o

build/parser.o: build/parser.tab.c
	gcc ${CLFAGS} -c $< -o $@

build/scanner.o: build/scanner.c
	gcc ${CLFAGS} -c $< -o $@

build/common.o: src/common.c
	gcc ${CLFAGS} -c $< -o $@

build/file_util.o: src/file_util/file_util.c
	gcc ${CLFAGS} -c $< -o $@

build/memory_codegen.o: src/codegen/src/memory_codegen.c
	gcc ${CLFAGS} -c $< -o $@

build/compute_codegen.o: src/codegen/src/compute_codegen.c
	gcc ${CLFAGS} -c $< -o $@

build/parser_util.o: src/parser_util/parser_util.c src/codegen/include/memory_codegen.h build/compute_codegen.o
	gcc ${CLFAGS} -c $< -o $@

build/parser.tab.c build/parser.tab.h: src/parser.y
	mkdir -p build
	bison -d -o build/parser.tab.c $<

build/scanner.c: src/scanner.l
	flex --outfile=build/scanner.c $<

clean:
	rm -rf build *.asm