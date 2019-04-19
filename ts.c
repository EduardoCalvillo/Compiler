#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ts.h"

int current_adresse = 400;
int ts_index = 0;
int current_profondeur = 0;

int current_instruction = 0;
int lastId = 0;


FILE *asmFile = NULL;
void init(void)
{
    asmFile = fopen("asmFile.asm", "w+");
    if (asmFile == NULL)
    {
        perror("Error in init with asmFile");
        fclose(asmFile);
        exit(EXIT_FAILURE);
    }
}

void ts_add(char *name, int type)
{
    // AJOUTER LES TESTS.
    if (ts_index > 0 && ts_get_index(name) != -1 && current_profondeur == ts_get_profondeur(name))
    {
        fprintf(stderr, "😱😱😱 Vous ne pouvez pas déclarer deux fois la même vairable dans le contexte");
        exit(EXIT_FAILURE);
    }
    strcpy(symbols[ts_index].name, name);
    symbols[ts_index].adresse = current_adresse;
    symbols[ts_index].profondeur = current_profondeur;
    symbols[ts_index].type = type;
    ts_index++;
    current_adresse++;
}

int ts_add_tmp(int type)
{
    // AJOUTER LES TESTS.
    strcpy(symbols[ts_index].name, "tmp");
    symbols[ts_index].adresse = current_adresse;
    symbols[ts_index].type = type;
    symbols[ts_index].profondeur = current_profondeur;
    ts_index++;
    current_adresse++;
    return ts_index - 1;
}

int ts_get_adr(int index)
{
    return symbols[index].adresse;
}

int ts_get_index(char *name)
{
    int i = ts_index - 1;
    while (i >= 0)
    {
        // printf("symbole[%d].name = [%s],   name = [%s]", i, symbols[i].name, name);
        if (strcmp(symbols[i].name, name) == 0)
            return i;
        i--;
    }
    return -1;
}

int ts_get_profondeur(char *name)
{
    int i = 0;
    while (i < ts_index)
    {
        // printf("symbole[%d].name = [%s],   name = [%s]", i, symbols[i].name, name);
        if (strcmp(symbols[i].name, name) == 0)
            return symbols[i].profondeur;
        i++;
    }
    return -1;
}

int ts_get_type(int index)
{
    if (index >= ts_index || index < 0)
        return -1;
    return symbols[index].type;
}

int ts_get_last_index()
{
    return ts_index - 1;
}

void ts_pop_tmp()
{
    ts_index--;
    current_adresse--;
}

void ts_inc_profondeur()
{
    current_profondeur++;
}
void ts_dec_profondeur()
{
    int i = ts_index - 1;
    ts_print();
    while (i >= 0 && symbols[i].profondeur == current_profondeur)
    {
        ts_index--;
        current_adresse--;
        i--;
    }
    ts_print();
    current_profondeur--;
}

void check_type(int first, int second)
{
    if (first != second)
    {
        printError("ಠ_ಠ\ttypes différents %s .. %s\tt(°¤°t)", toString(first), toString(second));
    }
}

void tins_add1(char *instruction, int rA)
{
    if (current_instruction > INST_TBL_SIZE)
    {
        fprintf(stderr, "😱😱😱 Instruction table overflow");
        exit(EXIT_FAILURE);
    }
    //AJOUTER DES TESTS
    instTable[current_instruction].inst = instruction;
    instTable[current_instruction].rA = rA;
    instTable[current_instruction].rB = -1;
    instTable[current_instruction].rC = -1;
    instTable[current_instruction].nbrArg = 1;
    current_instruction++;
}
void tins_add2(char *instruction, int rA, int rB)
{
    if (current_instruction >= INST_TBL_SIZE)
    {
        fprintf(stderr, "😱😱😱 Instruction table overflow");
        exit(EXIT_FAILURE);
    }
    //AJOUTER DES TESTS
    instTable[current_instruction].inst = instruction;
    instTable[current_instruction].rA = rA;
    instTable[current_instruction].rB = rB;
    instTable[current_instruction].rC = -1;
    instTable[current_instruction].nbrArg = 2;
    current_instruction++;
}
void tins_add3(char *instruction, int rA, int rB, int rC)
{
    if (current_instruction > INST_TBL_SIZE)
    {
        fprintf(stderr, "😱😱😱 Instruction table overflow");
        exit(EXIT_FAILURE);
    }
    //AJOUTER DES TESTS
    instTable[current_instruction].inst = instruction;
    instTable[current_instruction].rA = rA;
    instTable[current_instruction].rB = rB;
    instTable[current_instruction].rC = rC;
    instTable[current_instruction].nbrArg = 3;
    current_instruction++;
}

void update_jmp_addr(int addr){
    instTable[addr].rA = tins_get_next();
}

int tins_get_current(void){
    return current_instruction - 1;
}

int tins_get_next(void){
    return current_instruction;
}

void ts_print(void)
{
    printf("\n\n===========================================\n");
    printf("Index\tAddress\tName\tType\tProfondeur\n\n");
    for (int i = 0; i < ts_index; i++)
    {
        printf("@%02d:\t %d\t%s\t%s\t%d\n",
               i,
               symbols[i].adresse,
               symbols[i].name,
               toString(symbols[i].type),
               symbols[i].profondeur);
    }
    printf("\n===========================================\n\n");
}

void printError(const char *name, ...)
{
    va_list args;
    va_start(args, name);
    vfprintf(stderr, name, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

void printASM(void)
{
    for(int i=0; i<current_instruction; i++){
        char str[1000] = "";
        if(instTable[i].nbrArg == 3)
            sprintf(str, "%s %d %d %d", instTable[i].inst, instTable[i].rA, instTable[i].rB, instTable[i].rC);
        else if(instTable[i].nbrArg == 2)
            sprintf(str, "%s %d %d", instTable[i].inst, instTable[i].rA, instTable[i].rB);
        else
            sprintf(str, "%s %d", instTable[i].inst, instTable[i].rA);
        if(i < current_instruction-1)
            fprintf(asmFile, "%s\n", str);
        else
            fprintf(asmFile, "%s", str);
    }    
}

char *toString(int type)
{
    switch (type)
    {
    case INT:
        return "int";
        break;

    case CHAR:
        return "char";
        break;

    case STRING:
        return "string";
        break;

    case BOOL:
        return "bool";
        break;

    case FLOAT:
        return "float";
        break;

    default:
        return "error";
        break;
    }
}