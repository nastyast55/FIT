/*********************************************************
  Predmet: IFJ / IAL
  Nazev: Prekladac a interpret jazyka IFJ16
  Varianta: a/2/I
  Datum: 14.10. 2016
  Autori: 
      Volak Marek
      Homa Patrik
      Kocnar David
      Zivcak Adam
      Stoika Anastasiia 
*********************************************************/

#include <stdlib.h>
#include "error.h"

/**
 * Globalne premenne
 */
ErrorCode error = ERR_OK;
int riadok = 1;
int stlpec = 1;

/**
 * Nazvy jednotlivych chyb
 */
const char *err_text[] =
{
    "",
    "Chyba v ramci lexikalni analyzy.\0",
    "Chyba v ramci syntakticke analyzy.\0",
    "Chyba v ramci semanticke analyzy. Nedefinovany identifikator. \0",
    "Chyba v ramci semanticke analyzy. Spatny datovy typ. \0",
    "Chyba v ramci semanticke analyzy. \0",
    "Chyba NUM INPUT. \0",
    "Chyba neinicializovane. \0",
    "Chyba deleni nulou. \0",
    "Chyba pri interpretaci.\0",
    "Interni chyba prekladace.\0",
    "ERROR: Input file."
};

/**
 * Vypise chybu
 */
void setError(ErrorCode error)
{
    //fprintf(stderr, "%s radek:%d sloupec %d\n", tError_mena[error], token.radek,token.sloupec);
    fprintf(stderr, "%s \n", err_text[error]);
    exit(error);
}

void printError(char* string)
{
    printf(" ! Chyba ! - %s\n", string);
    return;
}


