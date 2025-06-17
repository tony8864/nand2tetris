<<<<<<< HEAD
CLFAGS = -Isrc -Isrc/file_util -Isrc/safe_util -Isrc/common -Isrc/lexer -Isrc/parser -Isrc/symbol_table/class_symbol_table -Isrc/str_util
=======
CLFAGS = -Isrc -Isrc/file_util -Isrc/safe_util -Isrc/common -Isrc/lexer -Isrc/parser -Isrc/symbol_table/class_symbol_table -Isrc/symbol_table/routine_symbol_table -Isrc/str_util
>>>>>>> c0c37b5 (implement subroutine level symbol table)

OBJECTS = 	build/parser.o		\
			build/scanner.o 	\
			build/file_util.o 	\
			build/safe_util.o 	\
			build/common.o 		\
			build/strutil.o		\
			build/string_handler.o  \
			build/parser_util.o 	\
<<<<<<< HEAD
			build/class_symbol_table.o
=======
			build/class_symbol_table.o \
			build/routine_symbol_table.o
>>>>>>> c0c37b5 (implement subroutine level symbol table)

CLASS_SYMTAB_PATH = src/symbol_table/class_symbol_table
CLASS_SYMTAB_H = ${CLASS_SYMTAB_PATH}/class_symbol_table.h
CLASS_SYMTAB_C = ${CLASS_SYMTAB_PATH}/class_symbol_table.c
CLASS_SYMTAB_DEP = ${CLASS_SYMTAB_C} ${SAFEUTIL_H}

<<<<<<< HEAD
=======
ROUTINE_SYMTAB_PATH = src/symbol_table/routine_symbol_table
ROUTINE_SYMTAB_H = ${ROUTINE_SYMTAB_PATH}/routine_symbol_table.h
ROUTINE_SYMTAB_C = ${ROUTINE_SYMTAB_PATH}/routine_symbol_table.c
ROUTINE_SYMTAB_DEP = ${ROUTINE_SYMTAB_C} ${SAFEUTIL_H}

>>>>>>> c0c37b5 (implement subroutine level symbol table)
FILEUTIL_PATH 	= src/file_util
FILEUTIL_H 		= ${FILEUTIL_PATH}/file_util.h
FILEUTIL_C 		= ${FILEUTIL_PATH}/file_util.c
FILEUTIL_DEP 	= ${FILEUTIL_C} ${FILEUTIL_H} ${SAFEUTIL_H}

STRUTIL_PATH = src/str_util
STRUTIL_H	 = ${STRUTIL_PATH}/str_util.h
STRUTIL_C	 = ${STRUTIL_PATH}/str_util.c
STRUTIL_DEP	 = ${STRUTIL_C} ${SAFEUTIL_H}

PARSERUTIL_PATH = src/parser
PARSERUTIL_H 	= ${PARSERUTIL_PATH}/parser_util.h
PARSERUTIL_C	= ${PARSERUTIL_PATH}/parser_util.c
PARSERUTIL_DEP	= ${PARSERUTIL_C} ${COMMON_H} ${SAFEUTIL_H}

SAFEUTIL_PATH = src/safe_util
SAFEUTIL_H    = ${SAFEUTIL_PATH}/safe_util.h
SAFEUTIL_C    = ${SAFEUTIL_PATH}/safe_util.c
SAFEUTIL_DEP  = ${SAFEUTIL_C} ${SAFEUTIL_H}

COMMON_PATH = src/common
COMMON_H 	= ${COMMON_PATH}/common.h
COMMON_C 	= ${COMMON_PATH}/common.c
COMMON_DEP  = ${COMMON_C} ${SAFEUTIL_H}

SCANNER  	= src/lexer/scanner.l
SCANNER_C 	= build/scanner.c
SCANNER_DEP = ${SCANNER_C} ${PARSER_H} ${COMMON_H} ${STRING_HANDLER_H}

STRING_HANDLER_PATH = src/lexer
STRING_HANDLER_H = ${STRING_HANDLER_PATH}/string_handler.h
STRING_HANDLER_C = ${STRING_HANDLER_PATH}/string_handler.c

PARSER   	= src/parser/parser.y
PARSER_C 	= build/parser.tab.c
PARSER_H 	= build/parser.tab.h
<<<<<<< HEAD
PARSER_DEP 	= ${PARSER_C} ${COMMON_H} ${SAFEUTIL_H} ${FILEUTIL_H} ${PARSERUTIL_H}
=======
PARSER_DEP 	= ${PARSER_C} ${COMMON_H} ${SAFEUTIL_H} ${FILEUTIL_H} ${PARSERUTIL_H} ${CLASS_SYMTAB_H} ${ROUTINE_SYMTAB_H}
>>>>>>> c0c37b5 (implement subroutine level symbol table)

TARGET = compiler

all: build/${TARGET}

build/compiler: ${OBJECTS}
	gcc -o build/compiler build/*.o

build/parser.o: ${PARSER_DEP}
	gcc ${CLFAGS} -c $< -o $@

${PARSER_C} ${PARSER_H}: ${PARSER}
	mkdir -p build
	bison -d -o ${PARSER_C} $<

build/scanner.o: ${SCANNER_DEP}
	gcc ${CLFAGS} -c $< -o $@

build/scanner.c: ${SCANNER} 
	flex --outfile=${SCANNER_C} $<

build/string_handler.o: ${STRING_HANDLER_C}
	gcc ${CLFAGS} -c $< -o $@

build/parser_util.o: ${PARSERUTIL_DEP}
	gcc ${CLFAGS} -c $< -o $@

build/class_symbol_table.o: ${CLASS_SYMTAB_DEP}
	gcc ${CLFAGS} -c $< -o $@

<<<<<<< HEAD
=======
build/routine_symbol_table.o: ${ROUTINE_SYMTAB_DEP}
	gcc ${CLFAGS} -c $< -o $@

>>>>>>> c0c37b5 (implement subroutine level symbol table)
build/common.o: ${COMMON_DEP}
	gcc ${CLFAGS} -c $< -o $@

build/file_util.o: ${FILEUTIL_DEP}
	gcc ${CLFAGS} -c $< -o $@

build/strutil.o: ${STRUTIL_DEP}
	gcc ${CLFAGS} -c $< -o $@

build/safe_util.o: ${SAFEUTIL_DEP}
	gcc ${CLFAGS} -c $< -o $@

clean:
	rm -rf build