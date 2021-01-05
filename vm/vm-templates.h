#ifndef VM_TEMPLATES
#define VM_TEMPLATES
#else
#error vm-templates may only be included once
#endif

#include "util.h"

#define mktemplate(name, array) TEMPLATE name = { .items = array, .count = strcount(array) };

typedef struct {
	char** items;
	int count;
} TEMPLATE;

char* tpushlns[] = {
	"",
	"",
	"",
	"A=D+A",
	"D=M",
	"@SP",
	"A=M",
	"M=D",
	"@SP",
	"M=M+1",
};
mktemplate(tpush, tpushlns);

char* tpushconslns[] = {
	"",
	"D=A",
	"@SP",
	"A=M",
	"M=D",
	"@SP",
	"M=M+1",
};
mktemplate(tpushcons, tpushconslns);

char* tpushstatlns[] = {
	"",
	"D=M",
	"@SP",
	"A=M",
	"M=D",
	"@SP",
	"M=M+1",
};
mktemplate(tpushstat, tpushstatlns);


mktemplate(tpushtemp, tpushstatlns);
mktemplate(tpushpointer, tpushstatlns);

char* tpoplns[] = {
	"",
	"",
	"",
	"D=D+A",
	"@R13",
	"M=D",
	"@SP",
	"AM=M-1",
	"D=M",
	"@R13",
	"A=M",
	"M=D"
};
mktemplate(tpop, tpoplns);

char* tpopstatlns[] = {
	"@SP",
	"AM=M-1",
	"D=M",
	"",
	""
};
mktemplate(tpopstat, tpopstatlns);

mktemplate(tpoptemp, tpopstatlns);

mktemplate(tpoppointer, tpopstatlns);

char* tarithlns[] = {
	"@SP",
	"AM=M-1",
	"D=M",
	"A=A-1",
	""
};
mktemplate(tarith, tarithlns);

char* tneglns[] = {
	"@SP",
	"A=M-1",
	"M=-M",
};
mktemplate(tneg, tneglns);

char* tnotlns[] = {
	"@SP",
	"A=M-1",
	"M=!M",
};
mktemplate(tnot, tnotlns);

char* tcomplns[] = {
	"@SP",
	"AM=M-1",
	"D=M",
	"A=A-1",
	"D=D-M",
	"M=-1",
	"",
	"",
	"@SP",
	"A=M-1",
	"M=0",
	""
};
mktemplate(tcomp, tcomplns);

char* tlabellns[] = {
	""
};
mktemplate(tlabel, tlabellns);

char* tgotolns[] = {
	"",
	"0;JMP"
};
mktemplate(tgoto, tgotolns);

char* tifgotolns[] = {
	"@SP",
	"AM=M-1",
	"D=M",
	"",
	"D;JNE"
};
mktemplate(tifgoto, tifgotolns);

char* tcallstartlns[] = {
	"D=A",
	"@SP",
	"A=M",
	"M=D",
	"@SP",
	"M=M+1",
};
mktemplate(tcallstart, tcallstartlns);

char* tcallpushlns[] = {
	"",
	"D=M",
	"@SP",
	"A=M",
	"M=D",
	"@SP",
	"M=M+1",
};
mktemplate(tcallpush, tcallpushlns);

char* tcallsetarglns[] = {
	"@SP",
	"D=M",
	"@LCL",
	"M=D",
	"",
	"D=D-A",
	"@ARG",
	"M=D"
};
mktemplate(tcallsetarg, tcallsetarglns);

char* tcalljmplns[] = {
	"",
	"0;JMP",
	""
};
mktemplate(tcalljmp, tcalljmplns);
	
char* tframevarslns[] = {
	"@LCL",
	"@ARG",
	"@THIS",
	"@THAT"
};
mktemplate(tframevars, tframevarslns);

char* tfunctionlns[] = {
	""
};
mktemplate(tfunction, tfunctionlns);

char* tfunctionpushlns[] = {
	"@SP",
	"A=M",
	"M=0",
	"@SP",
	"M=M+1"
};
mktemplate(tfunctionpush, tfunctionpushlns);

char* tstartreturnlns[] = {
	"@LCL",
	"D=M",
	"@5",
	"A=D-A",
	"D=M",
	"@R13",
	"M=D",
	"@SP",
	"A=M-1",
	"D=M",
	"@ARG",
	"A=M",
	"M=D",
	"@ARG",
	"D=M+1",
	"@SP",
	"M=D"
};
mktemplate(tstartreturn, tstartreturnlns);

char* tretpoplns[] = {
	"@LCL",
	"AM=M-1",
	"D=M",
	"",
	"M=D",
};
mktemplate(tretpop, tretpoplns);

char* tendreturnlns[] = {
	"@R13",
	"A=M",
	"0;JMP"
};
mktemplate(tendreturn, tendreturnlns);

char* tbootstraplns[] = {
	"@256",
	"D=A",
	"@SP",
	"M=D",
	"@BOOTSTRAP$ret",
	"D=A",
	"@SP",
	"A=M",
	"M=D",
	"@SP",
	"M=M+1",
	"@LCL",
	"D=M",
	"@SP",
	"A=M",
	"M=D",
	"@SP",
	"M=M+1",
	"@ARG",
	"D=M",
	"@SP",
	"A=M",
	"M=D",
	"@SP",
	"M=M+1",
	"@THIS",
	"D=M",
	"@SP",
	"A=M",
	"M=D",
	"@SP",
	"M=M+1",
	"@THAT",
	"D=M",
	"@SP",
	"A=M",
	"M=D",
	"@SP",
	"M=M+1",
	"@5",
	"D=A",
	"@SP",
	"D=M-D",
	"@ARG",
	"M=D",
	"@SP",
	"D=M",
	"@LCL",
	"M=D",
	"@Sys.init",
	"0;JMP",
	"(BOOTSTRAP$ret)"
};
mktemplate(tbootstrap, tbootstraplns);
