OBJECTS = scanner.o parser.o

exe: ${OBJECTS}
	gcc -o exe ${OBJECTS}

scanner.o: scanner.c
	gcc -c $< -o $@

scanner.c: scanner.l parser.tab.h
	flex --outfile=scanner.c $<

parser.o: parser.tab.c
	gcc -c $< -o $@

parser.tab.c parser.tab.h: parser.y
	bison -d $<

clean:
	rm -f exe scanner.c ${OBJECTS} parser.tab.c parser.tab.h