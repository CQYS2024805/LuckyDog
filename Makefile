#debug makefile
.PHONY: run
Guesswho:
	g++ src/main.cpp -o guesswho -g -Wall
run:
	guesswho