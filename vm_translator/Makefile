CLFAGS = -Isrc

OBJECTS = build/parser.o build/scanner.o

all: build/vmtranslator

build/vmtranslator: ${OBJECTS}
	gcc -o build/vmtranslator build/*.o

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