FILES = *.c */*.c
LIBRARIES = -lpthread
INCLUDES = -I. -I./parser/ -I./compiler/ -I./vm/ -I./tokenizer/ -I./misc/ -I./assembler/
CFLAGS = -std=c99 -Wall -O3
OUTFILE = jackc

main: ${FILES}
	${CC} ${CFLAGS} ${LIBRARIES} ${INCLUDES} -o ${OUTFILE} ${FILES}
