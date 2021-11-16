.PHONY: all compile run clean

EXE = runfile

all:
	if [ -f ${EXE} ]; \
	then \
		rm ${EXE}; \
	fi;
	cc -o ${EXE} main.c threads.c ncurses.c waveform.c miscfunc.c -Wall -lm -lpthread -lncurses
	./${EXE}

compile: 
	cc -o ${EXE} main.c threads.c ncurses.c waveform.c miscfunc.c -Wall -lm -lpthread -lncurses

run:
	./${EXE}

load:
	./${EXE} -l

clean:
	rm ${EXE}
	# if [ -f ${TEST} ]; \
	# then \
	# 	rm ${TEST}; \
	# fi;
