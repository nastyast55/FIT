#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/**
 * Chybove kody.
 */ 
typedef enum ERRORS{
    ERR_OK = 0,
    ERR_LEX = 1,        //lexikalni
    ERR_SYN = 2,        //syntakticka
    ERR_SEM_UNDEF = 3,  //nedefinovany identifikator funkce/tridy/prom, nebo pokus o redefinici
    ERR_SEM_TYP = 4,    //spatny datovy typ
    ERR_SEM_OTHERS = 6, //semanticke ostatni
    ERR_NUM_INPT = 7,   //behova chyba pri nacitani cisla ze vstupu
    ERR_VAR_UNINIT = 8, //behova chyba pri praci s neinic promennou
    ERR_DIVZERO = 9,    //behova chyba deleni nulou
    ERR_OTHERS = 10,
    ERR_INTERN = 99,     //interni chyby interpretu, soubor, parametry
    ERR_FILE
} ErrorCode;
 
/**
 * Globalni promenne
 */
ErrorCode error;
 
/**
 * Prototypy funkci
 */
void setError(ErrorCode err);
void printError(char* string);

void debug(char* string);
