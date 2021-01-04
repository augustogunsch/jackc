#include <stdlib.h>
#include <string.h>
#include "vm-templates.h"
#include "vm-translator.h"
#include "util.h"
#define eq(translator, index, str) !strcmp(translator->currln->tokens[index], str)

typedef struct {
	STRINGLIST* head;
	STRINGLIST* tail;
} ASMBLK;

STRINGLIST* asmln(char* content) {
	STRINGLIST* ln = (STRINGLIST*)malloc(sizeof(STRINGLIST));
	ln->content = content;
	return ln;
}

void togarbage(VMTRANSLATOR* t, char* str) {
	STRINGLIST* garb = asmln(str);
	garb->next = t->garbage;
	t->garbage = garb;
}

char* atraw(VMTRANSLATOR* t, char* n, int len) {
	int sz = sizeof(char) * (len + 2);
	char* atstr = (char*)malloc(sz);
	sprintf(atstr, "@%s", n);
	togarbage(t, atstr);
	return atstr;
}

char* at(VMTRANSLATOR* t, char* n) {
	return atraw(t, n, strlen(n));
}

char* atn(VMTRANSLATOR* t, int n) {
	char* str = itoa(n);
	togarbage(t, str);
	return at(t, str);
}

char* mkstr(VMTRANSLATOR* t, char* str) {
	char* heapstr = ezheapstr(str);
	togarbage(t, heapstr);
	return heapstr;
}

char* mkpointerind(VMTRANSLATOR* t) {
	if(t->currln->tokens[2][0] == 0)
		return mkstr(t, "@THIS");
	else
		return mkstr(t, "@THAT");
}

char* mktempind(VMTRANSLATOR* t) {
	int index = atoi(t->currln->tokens[2]);
	char* actualind = itoa(index+5);	
	togarbage(t, actualind);
	return at(t, actualind);
}

char* dotat(VMTRANSLATOR* t, char* name, char* n) {
	int sz = sizeof(char) * (strlen(name) + strlen(n) + 3);
	char* atstr = (char*)malloc(sz);
	sprintf(atstr, "@%s.%s", name, n);
	togarbage(t, atstr);
	return atstr;
}

char* comment(VMTRANSLATOR* t) {
	int sz = (4 + strlen(t->currln->tokens[0])) * sizeof(char);
	for(int i = 1; i < t->currln->count; i++)
		sz += (1 + strlen(t->currln->tokens[i])) * sizeof(char);

	char* com = (char*)malloc(sz);
	if(t->currln->count == 1)
		sprintf(com, "// %s", t->currln->tokens[0]);
	else if(t->currln->count == 2)
		sprintf(com, "// %s %s", t->currln->tokens[0],
				t->currln->tokens[1]);
	else if(t->currln->count == 3)
		sprintf(com, "// %s %s %s", t->currln->tokens[0],
				t->currln->tokens[1],
				t->currln->tokens[2]);
	
	togarbage(t, com);
	return com;
}

char* switchsegment(VMTRANSLATOR* t) {
	if(eq(t, 1, "local"))
		return mkstr(t, "@LCL");
	if(eq(t, 1, "argument"))
		return mkstr(t, "@ARG");
	if(eq(t, 1, "this"))
		return mkstr(t, "@THIS");
	return mkstr(t, "@THAT");
}

char* mkspeciallab(VMTRANSLATOR* t, char* suffix, int* ind, int* len) {
	(*ind)++;
	*len = t->classnamelen + countplaces(*ind) + strlen(suffix) + 2;
	int sz = ((*len)+1) * sizeof(char);
	char* lab = (char*)malloc(sz);
	sprintf(lab, "%s$%s.%i", t->classname, suffix, (*ind));
	togarbage(t, lab);
	return lab;
}

char* mkcmplab(VMTRANSLATOR* t, int* len) {
	return mkspeciallab(t, "cmp", &(t->cmpind), len);
}

char* mkretlab(VMTRANSLATOR* t, int* len) {
	return mkspeciallab(t, "ret", &(t->retind), len);
}

char* enclosingparenthesis(VMTRANSLATOR* t, char* content, int len) {
	int sz = sizeof(char) * (len + 3);
	char* str = (char*)malloc(sz);
	sprintf(str, "(%s)", content);
	togarbage(t, str);
	return str;
}

char* mklab(VMTRANSLATOR* t) {
	int sz = (t->classnamelen + strlen(t->currln->tokens[1]) + 4) * sizeof(char);
	char* lab = (char*)malloc(sz);
	sprintf(lab, "(%s$%s)", t->classname, t->currln->tokens[1]);
	togarbage(t, lab);
	return lab;
}

char* mkgotolab(VMTRANSLATOR* t) {
	int sz = sizeof(char) * (t->classnamelen + strlen(t->currln->tokens[1]) + 3);
	char* lab = (char*)malloc(sz);
	sprintf(lab, "@%s$%s", t->classname, t->currln->tokens[1]);
	togarbage(t, lab);
	return lab;
}

ASMBLK* copytemplate(TEMPLATE* t) {
	ASMBLK* blk = (ASMBLK*)malloc(sizeof(ASMBLK));
	blk->head = asmln(t->items[0]);
	STRINGLIST* curr = blk->head;
	for(int i = 1; i < t->count; i++) {
		STRINGLIST* newln = asmln(t->items[i]);
		curr->next = newln;
		curr = newln;
	}
	curr->next = NULL;
	blk->tail = curr;
	return blk;
}

ASMBLK* mkasmlns(VMTRANSLATOR* t, TEMPLATE* tp) {
	// instruction comment
	tp->items[0] = comment(t);
	
	return copytemplate(tp);
}

void mergeasmblks(ASMBLK* a, ASMBLK* b) {
	a->tail->next = b->head;
	a->tail = b->tail;
	free(b);
}



/* START STACK MANIPULATION */

ASMBLK* translatepushconst(VMTRANSLATOR* t) {
	// @i
	tpushcons.items[1] = at(t, t->currln->tokens[2]);

	return mkasmlns(t, &tpushcons);
}

ASMBLK* translatepushstatic(VMTRANSLATOR* t) {
	// @classname.i
	tpushstat.items[1] = dotat(t, t->classname, t->currln->tokens[2]);

	return mkasmlns(t, &tpushstat);
}

ASMBLK* translatepushpointer(VMTRANSLATOR* t) {
	// @THIS/@THAT
	tpushpointer.items[1] = mkpointerind(t);

	return mkasmlns(t, &tpushpointer);
}

ASMBLK* translatepushtemp(VMTRANSLATOR* t) {
	// @5+i
	tpushtemp.items[1] = mktempind(t);

	return mkasmlns(t, &tpushtemp);
}

void pushpopcommon(VMTRANSLATOR* t, TEMPLATE* tp) {
	// @segment
	tp->items[1] = switchsegment(t);
	
	// D=M
	tp->items[2] = mkstr(t, "D=M");

	// @i
	tp->items[3] = at(t, t->currln->tokens[2]);
}

ASMBLK* translatepushgeneric(VMTRANSLATOR* t) {
	pushpopcommon(t, &tpush);

	return mkasmlns(t, &tpush);
}

ASMBLK* translatepush(VMTRANSLATOR* t) {
	if(eq(t, 1, "constant"))
		return translatepushconst(t);
	if(eq(t, 1, "static"))
		return translatepushstatic(t);
	if(eq(t, 1, "pointer"))
		return translatepushpointer(t);
	if(eq(t, 1, "temp"))
		return translatepushtemp(t);
	return translatepushgeneric(t);
}

ASMBLK* translatepopstatic(VMTRANSLATOR* t) {
	// @classname.i
	tpopstat.items[tpopstat.count-2] = dotat(t, t->classname, t->currln->tokens[2]);

	// M=D
	tpopstat.items[tpopstat.count-1] = mkstr(t, "M=D");

	return mkasmlns(t, &tpopstat);
}

ASMBLK* translatepoppointer(VMTRANSLATOR* t) {
	// @THIS/@THAT
	tpoppointer.items[tpoppointer.count-2] = mkpointerind(t);

	// M=D
	tpoppointer.items[tpoppointer.count-1] = mkstr(t, "M=D");

	return mkasmlns(t, &tpoppointer);
}

ASMBLK* translatepoptemp(VMTRANSLATOR* t) {
	// @5+i
	tpoptemp.items[tpoptemp.count-2] = mktempind(t);

	tpoptemp.items[tpoptemp.count-1] = mkstr(t, "M=D");

	return mkasmlns(t, &tpoptemp);
}

ASMBLK* translatepopgeneric(VMTRANSLATOR* t) {
	pushpopcommon(t, &tpop);
	
	return mkasmlns(t, &tpop);
}

ASMBLK* translatepop(VMTRANSLATOR* t) {
	if(eq(t, 1, "static"))
		return translatepopstatic(t);
	if(eq(t, 1, "pointer"))
		return translatepoppointer(t);
	if(eq(t, 1, "temp"))
		return translatepoptemp(t);
	return translatepopgeneric(t);
}

/* END STACK MANIPULATION */


/* BEGIN OPERATIONS */

ASMBLK* translatearith(VMTRANSLATOR* t, char* op) {
	tarith.items[tarith.count-1] = mkstr(t, op);

	return mkasmlns(t, &tarith);
}

ASMBLK* translatecomp(VMTRANSLATOR* t, char* op) {
	int labellen;
	char* label = mkcmplab(t, &labellen);

	// @label
	tcomp.items[tcomp.count-6] = atraw(t, label, labellen);
	
	// D;J(op)
	int sz = sizeof(char) * 6;
	char* trueop = (char*)malloc(sz);
	sprintf(trueop, "D;J%s", op);
	tcomp.items[tcomp.count-5] = trueop;
	togarbage(t, trueop);

	// (label)
	tcomp.items[tcomp.count-1] = enclosingparenthesis(t, label, labellen);

	return mkasmlns(t, &tcomp);
}

/* END OPERATIONS */

ASMBLK* translatelabel(VMTRANSLATOR* t) {
	// (classname$label)
	tlabel.items[tlabel.count-1] = mklab(t);

	return mkasmlns(t, &tlabel);
}

ASMBLK* translategoto(VMTRANSLATOR* t) {
	// @label
	tgoto.items[tgoto.count-2] = mkgotolab(t);

	return mkasmlns(t, &tgoto);
}

ASMBLK* translateifgoto(VMTRANSLATOR* t) {
	// @label
	tifgoto.items[tifgoto.count-2] = mkgotolab(t);

	return mkasmlns(t, &tifgoto);
}

ASMBLK* translatereturn(VMTRANSLATOR* t) {
	ASMBLK* blk = mkasmlns(t, &tstartreturn);
	
	for(int i = tframevars.count-1; i >= 0; i--) {
		tretpop.items[tretpop.count-2] = tframevars.items[i];
		mergeasmblks(blk, copytemplate(&tretpop));
	}

	mergeasmblks(blk, copytemplate(&tendreturn));
	return blk;
}

ASMBLK* translatefunction(VMTRANSLATOR* t) {
	t->retind = 0;
	t->cmpind = 0;

	// (funcname)
	tfunction.items[1] = mklab(t);
	ASMBLK* blk = mkasmlns(t, &tfunction);

	// repeat nVars times:
	int nlocals = atoi(t->currln->tokens[2]);

	for(int i = 0; i < nlocals; i++)
		mergeasmblks(blk, copytemplate(&tfunctionpush));

	return blk;
}

ASMBLK* pushframe(VMTRANSLATOR* t, char* retlab, int retlablen, int* framesize) {
	tcallstart.items[1] = atraw(t, retlab, retlablen);

	ASMBLK* blk = mkasmlns(t, &tcallstart);

	for(int i = 0; i < tframevars.count; i++) {
		tcallpush.items[0] = tframevars.items[i];
		mergeasmblks(blk, copytemplate((&tcallpush)));
	}

	*framesize = tframevars.count + 1;
	return blk;
}

ASMBLK* translatecall(VMTRANSLATOR* t) {
	// return label
	int retlablen;
	char* retlab = mkretlab(t, &retlablen);

	// push frame
	int framesize;
	ASMBLK* blk = pushframe(t, retlab, retlablen, &framesize);

	// setting ARG
	int nargs = atoi(t->currln->tokens[2]);
	tcallsetarg.items[tcallsetarg.count-4] = atn(t, nargs + framesize);
	mergeasmblks(blk, copytemplate(&tcallsetarg));
	
	// jmp
	tcalljmp.items[tcalljmp.count-3] = at(t, t->currln->tokens[1]);
	tcalljmp.items[tcalljmp.count-1] = enclosingparenthesis(t, retlab, retlablen);
	mergeasmblks(blk, copytemplate(&tcalljmp));

	return blk;
}

ASMBLK* translateln(VMTRANSLATOR* t) {
	if(eq(t, 0, "push"))
		return translatepush(t);
	if(eq(t, 0, "pop"))
		return translatepop(t);

	if(eq(t, 0, "add"))
		return translatearith(t, "M=D+M");
	if(eq(t, 0, "sub"))
		return translatearith(t, "M=M-D");
	if(eq(t, 0, "and"))
		return translatearith(t, "M=D&M");
	if(eq(t, 0, "or"))
		return translatearith(t, "M=D|M");

	if(eq(t, 0, "neg"))
		return mkasmlns(t, &tneg);
	if(eq(t, 0, "not"))
		return mkasmlns(t, &tnot);

	if(eq(t, 0, "eq"))
		return translatecomp(t, "EQ");
	if(eq(t, 0, "gt"))
		return translatecomp(t, "LT");
	if(eq(t, 0, "lt"))
		return translatecomp(t, "GT");

	if(eq(t, 0, "label"))
		return translatelabel(t);
	if(eq(t, 0, "goto"))
		return translategoto(t);
	if(eq(t, 0, "if-goto"))
		return translateifgoto(t);

	if(eq(t, 0, "return"))
		return translatereturn(t);
	if(eq(t, 0, "function"))
		return translatefunction(t);
	return translatecall(t);
}

STRINGLIST* translatevm(VMTRANSLATOR* t) {
	ASMBLK* blk = copytemplate(&tbootstrap);
	while(t->currln != NULL) {
		mergeasmblks(blk, translateln(t));
		t->currln = t->currln->next;
	}
	STRINGLIST* output = blk->head;
	free(blk);
	return output;
}

VMTRANSLATOR* mkvmtranslator(char* classname, LINEBLOCK* vmlines) {
	VMTRANSLATOR* transl = (VMTRANSLATOR*)malloc(sizeof(VMTRANSLATOR));
	transl->currln = vmlines->head;
	transl->start = vmlines->head;
	transl->garbage = NULL;
	transl->retind = 0;
	transl->cmpind = 0;
	transl->classname = classname;
	transl->classnamelen = strlen(classname);
	return transl;
}

void freegarbage(STRINGLIST* garbage) {
	if(garbage != NULL) {
		free(garbage->content);
		STRINGLIST* next = garbage->next;
		free(garbage);
		freegarbage(next);
	}
}

void freevmtranslator(VMTRANSLATOR* t) {
	freegarbage(t->garbage);
	free(t);
}
