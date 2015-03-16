# Makefile--
# AUTHOR:Ruikang Dai
# DATE:Sun Mar  1 14:42:37 EST 2015
qsort: qsort.o restart.o
	cc qsort.o restart.o -o $@
qsort.o: qsort.c restart.h 
	cc -c qsort.c -o $@
restart.o: restart.c restart.h
	cc -c restart.c -o $@
