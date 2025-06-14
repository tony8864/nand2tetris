CLFAGS = -Isrc -Isrc/file_util -Isrc/safe_util -Isrc/common -Isrc/lexer

OBJECTS = 	build/parser.o		\
			build/scanner.o 	\
			build/file_util.o 	\
			build/safe_util.o 	\
			build/common.o 		\
			build/string_handler.o

FILEUTIL_PATH 	= src/file_util
FILEUTIL_H 		= ${FILEUTIL_PATH}/file_util.h
FILEUTIL_C 		= ${FILEUTIL_PATH}/file_util.c
FILEUTIL_DEP 	= ${FILEUTIL_C} ${FILEUTIL_H} ${SAFEUTIL_H}

SAFEUTIL_PATH = src/safe_util
SAFEUTIL_H    = ${SAFEUTIL_PATH}/safe_util.h
SAFEUTIL_C    = ${SAFEUTIL_PATH}/safe_util.c
SAFEUTIL_DEP  = ${SAFEUTIL_C} ${SAFEUTIL_H}

COMMON_PATH = src/common
COMMON_H 	= ${COMMON_PATH}/common.h
COMMON_C 	= ${COMMON_PATH}/common.c

SCANNER  	= src/lexer/scanner.l
SCANNER_C 	= build/scanner.c
SCANNER_DEP = ${SCANNER_C} ${PARSER_H} ${COMMON_H} ${STRING_HANDLER_H}

STRING_HANDLER_PATH = src/lexer
STRING_HANDLER_H = ${STRING_HANDLER_PATH}/string_handler.h
STRING_HANDLER_C = ${STRING_HANDLER_PATH}/string_handler.c

PARSER   	= src/parser/parser.y
PARSER_C 	= build/parser.tab.c
PARSER_H 	= build/parser.tab.h
PARSER_DEP 	= ${PARSER_C} ${COMMON_H} ${SAFEUTIL_H} ${FILEUTIL_H}

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

build/common.o: ${COMMON_C}
	gcc ${CLFAGS} -c $< -o $@

build/file_util.o: ${FILEUTIL_DEP}
	gcc ${CLFAGS} -c $< -o $@

build/safe_util.o: ${SAFEUTIL_DEP}
	gcc ${CLFAGS} -c $< -o $@

clean:
	rm -rf build