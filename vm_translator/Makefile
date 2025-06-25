CLFAGS = -Isrc -Isrc/parser_util -Isrc/codegen/include -Isrc/file_util -Isrc/common

OBJECTS = 	build/parser.o \
			build/scanner.o \
			build/parser_util.o \
			build/codegen_util.o \
			build/memory_codegen.o \
			build/compute_codegen.o \
			build/label_codegen.o \
			build/function_codegen.o \
			build/file_util.o \
			build/common.o

PARSER   	= src/parser.y
PARSER_C 	= build/parser.tab.c
PARSER_H 	= build/parser.tab.h
PARSER_DEP  = 	${PARSER_C} ${PARSER_UTIL_H} ${FILEUTIL_H} ${COMMON_H} ${CODEGEN_UTIL_H} \
				${COMPUTE_CODEGEN_H} ${FUNCTION_CODEGEN_H} \
				${LABEL_CODEGEN_H} ${MEMORY_CODEGEN_H} ${FILEUTIL_H}

SCANNER  	= src/scanner.l
SCANNER_C 	= build/scanner.c
SCANNER_DEP = ${SCANNER_C} ${COMMON_H}

COMMON_PATH = src/common
COMMON_H 	= ${COMMON_PATH}/common.h
COMMON_C 	= ${COMMON_PATH}/common.c

FILEUTIL_PATH 	= src/file_util
FILEUTIL_H 		= ${FILEUTIL_PATH}/file_util.h
FILEUTIL_C 		= ${FILEUTIL_PATH}/file_util.c

PARSER_UTIL_PATH = src/parser_util
PARSER_UTIL_C = ${PARSER_UTIL_PATH}/parser_util.c
PARSER_UTIL_H = ${PARSER_UTIL_PATH}/parser_util.h
PARSER_UTIL_DEP = ${PARSER_UTIL_C} ${FILEUTIL_H} ${FUNCTION_CODEGEN_H}

CODEGEN_PATH 	= src/codegen

CODEGEN_UTIL_C = ${CODEGEN_PATH}/src/codegen_util.c
CODEGEN_UTIL_H = ${CODEGEN_PATH}/include/codegen_util.h
CODEGEN_UTIL_DEP = ${CODEGEN_UTIL_C} ${COMMON_H}

COMPUTE_CODEGEN_C = ${CODEGEN_PATH}/src/compute_codegen.c
COMPUTE_CODEGEN_H = ${CODEGEN_PATH}/include/compute_codegen.h
COMPUTE_CODEGEN_DEP = ${COMPUTE_CODEGEN_C} ${CODEGEN_UTIL_H}

FUNCTION_CODEGEN_C = ${CODEGEN_PATH}/src/function_codegen.c
FUNCTION_CODEGEN_H = ${CODEGEN_PATH}/include/function_codegen.h
FUNCTION_CODEGEN_DEP = ${FUNCTION_CODEGEN_C} ${CODEGEN_UTIL_H}

LABEL_CODEGEN_C = ${CODEGEN_PATH}/src/label_codegen.c
LABEL_CODEGEN_H = ${CODEGEN_PATH}/include/label_codegen.h
LABEL_CODEGEN_DEP = ${LABEL_CODEGEN_C} ${CODEGEN_UTIL_H}

MEMORY_CODEGEN_C = ${CODEGEN_PATH}/src/memory_codegen.c
MEMORY_CODEGEN_H = ${CODEGEN_PATH}/include/memory_codegen.h
MEMORY_CODEGEN_DEP = ${MEMORY_CODEGEN_C} ${CODEGEN_UTIL_H}


all: build/vmtranslator

build/vmtranslator: ${OBJECTS}
	gcc -o build/vmtranslator build/*.o


${PARSER_C} ${PARSER_H}: ${PARSER}
	mkdir -p build
	bison -d -o ${PARSER_C} $<

build/parser.o: ${PARSER_DEP}
	gcc ${CLFAGS} -c $< -o $@

build/scanner.c: ${SCANNER}
	flex --outfile=${SCANNER_C} $<

build/scanner.o: ${SCANNER_DEP}
	gcc ${CLFAGS} -c $< -o $@

build/common.o: ${COMMON_C}
	gcc ${CLFAGS} -c $< -o $@

build/file_util.o: ${FILEUTIL_C}
	gcc ${CLFAGS} -c $< -o $@

build/parser_util.o: ${PARSER_UTIL_DEP}
	gcc ${CLFAGS} -c $< -o $@

build/codegen_util.o: ${CODEGEN_UTIL_DEP}
	gcc ${CLFAGS} -c $< -o $@

build/compute_codegen.o: ${COMPUTE_CODEGEN_DEP}
	gcc ${CLFAGS} -c $< -o $@

build/function_codegen.o: ${FUNCTION_CODEGEN_DEP}
	gcc ${CLFAGS} -c $< -o $@

build/label_codegen.o: ${LABEL_CODEGEN_DEP}
	gcc ${CLFAGS} -c $< -o $@

build/memory_codegen.o: ${MEMORY_CODEGEN_DEP}
	gcc ${CLFAGS} -c $< -o $@

clean:
	rm -rf build out

clean-out:
	rm -rf out