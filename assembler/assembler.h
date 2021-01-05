#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include <stdio.h>
#include "parser.h"

#define RAM_LIMIT 24577
#define TOP_VAR 16383
#define BOTTOM_VAR 16
#define INST_SIZE 17
#define C_TOKEN_SIZE 4

typedef struct {
	char* name;
	int value;
} SYMBOL;

typedef struct {
	SYMBOL** items;
	int count;
	int size;
} SYMBOLARRAY;

typedef struct {
	STRINGLIST* lns;
	STRINGLIST* garbage;

	SYMBOLARRAY* labels;

	SYMBOLARRAY* vars;
	int varsramind;
} ASSEMBLER;

ASSEMBLER* mkassembler(STRINGLIST* input);
void preprocess(ASSEMBLER* a);
void assemble(ASSEMBLER* a);
void freeassembler(ASSEMBLER* a);
#endif
