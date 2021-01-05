#ifndef VM_TRANSLATOR
#define VM_TRANSLATOR
#include "vm-lines.h"

typedef struct {
	char* classname;
	int classnamelen;
	LINE* start;
	LINE* currln;
	STRINGLIST* garbage;
	int cmpind;
	int retind;
} VMTRANSLATOR;

typedef struct {
	STRINGLIST* head;
	STRINGLIST* tail;
} ASMBLK;

ASMBLK* translatevm(VMTRANSLATOR* t);
VMTRANSLATOR* mkvmtranslator(char* classname, LINEBLOCK* vmlines);
void freevmtranslator(VMTRANSLATOR* t);
void mergeasmblks(ASMBLK* a, ASMBLK* b);

#endif
