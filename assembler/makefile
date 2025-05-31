OBJECTS = scanner.o

exe: ${OBJECTS}
	gcc -o exe ${OBJECTS}

scanner.o: scanner.c
	gcc -c $< -o $@

scanner.c: scanner.l
	flex --outfile=scanner.c $<

clean:
	rm -f exe scanner.c ${OBJECTS}