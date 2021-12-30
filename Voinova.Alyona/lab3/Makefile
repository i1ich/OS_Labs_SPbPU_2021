CFLAGS = -std=c++14 -Wall -Werror
PFLAGS = -lpthread -lrt

INCLUDE = ./include/*
SOURCE = ./src/*
TESTS = ./test/*

all:
	g++ $(CFLAGS) $(INCLUDE) $(SOURCE) ${TESTS} -o tests $(PFLAGS)

clean:
	@rm -f tests