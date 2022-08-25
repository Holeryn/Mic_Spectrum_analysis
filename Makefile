SRC_FILES = mic_monitor.c transform.c libportaudio.a
CC_FLAGS = -lrt -lm -lasound -pthread -O3
CC = gcc

all:
	${CC} ${SRC_FILES} ${CC_FLAGS} -o mic_monitor
