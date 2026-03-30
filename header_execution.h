#ifndef HEADER_EXECUTION_H
	#define HEADER_EXECUTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#define taille_memoire 65536
#define nb_registre 32

typedef struct{
    int op;
    int dest;
    int src1;
    int imm;
    short src2;
}t_instruction2;


void execute_programme(FILE* f,char* memoire, short * registre, int *N, int *Z, int *C, unsigned short *PC);
void charge_programme_en_memoire(FILE * f, char * memoire);
void maj_bit_Z_N(short resultat,int *N, int *Z);
int execute_instruction(char* memoire,short * registre,int *N, int *Z, int *C, unsigned short *PC,int * fin);
t_instruction2 * intialise_instruction(char* memoire,unsigned short *PC);
void execution(void);




#endif