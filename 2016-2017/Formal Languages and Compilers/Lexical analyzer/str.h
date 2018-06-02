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

#ifndef STR_H
#define STR_H


//hlavickovy soubor pro praci s nekonecne dlouhymi retezci

typedef struct str
{
  char* str;		// misto pro dany retezec ukonceny znakem '\0'
  int length;		// skutecna delka retezce
  int allocSize;	// velikost alokovane pameti
} string;


int strInit(string *s);
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
int strCopyString(string *s1, string *s2);
int strCmpString(string *s1, string *s2);
int strCmpConstStr(string *s1, char *s2);

char *strGetStr(string *s);
int strGetLength(string *s);

int oct2dec(char *oct);


// zde budou pridany vest. funkce pro praci s retezci


#endif
