#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "str.h"
#include "error.h"

#define MAX_KEY 17
#define OFFSET 23
#define ESC_N '\n'
#define ESC_T '\t'




typedef enum TokenTypes {
    TYPE_UNSET = -1,        // default state
    TYPE_EOF = 0,           // end of file
    TYPE_INTEGER = 1,       
    TYPE_DOUBLE = 2,
    TYPE_SMPL_ID = 3,       //_id, $id, Id, id, _id45, $id8A, Id, id...
    TYPE_FULL_ID = 4,       // a.b_ , a.ba , JM_a.ba
    TYPE_STRING = 5,
    TYPE_DIVIDE_OP = 6,
    TYPE_PLUS_OP = 7,
    TYPE_MINUS_OP = 8,
    TYPE_MULT_OP = 9,
    TYPE_GREATER = 10,
    TYPE_LESS = 11,
    TYPE_GRT_EQ = 12,       // >= 
    TYPE_LESS_EQ = 13,      // <=
    TYPE_EQUAL = 14,        // ==
    TYPE_NOT_EQ = 15,    // !=
    TYPE_ASSIGN = 16,       // =
    TYPE_SEMICOLON = 17,    // ;
    TYPE_LPARENT = 18,      // (
    TYPE_RPARENT = 19,      // )
    TYPE_LPAR2 = 20,
    TYPE_RPAR2 = 21,
    TYPE_COMMA = 22,
    TYPE_KEY_BOOL = 23,
    TYPE_KEY_BREAK = 24,
    TYPE_KEY_CLASS = 25,
    TYPE_KEY_CONTINUE = 26,
    TYPE_KEY_DO = 27,
    TYPE_KEY_DOUBLE = 28,
    TYPE_KEY_ELSE = 29,
    TYPE_KEY_FALSE = 30,
    TYPE_KEY_FOR = 31,
    TYPE_KEY_IF = 32,
    TYPE_KEY_INT = 33,
    TYPE_KEY_RETURN = 34,
    TYPE_KEY_STRING = 35,
    TYPE_KEY_STATIC = 36,
    TYPE_KEY_TRUE = 37,
    TYPE_KEY_VOID = 38,
    TYPE_KEY_WHILE = 39
}TokenType;

const char *TOKEN_TABLE[41] = {
    "EOF",
    "CISLO INT",
    "CISLO DOUBLE",
    "SIMPLE_ID",
    "FULL_ID",
    "STRING",
    "DIVIDE",
    "PLUS",
    "MINUS",
    "MULT",
    "GREATER",
    "LESS",
    "GRT_EQ",
    "LESS_EQ",
    "EQUAL",
    "NOT_EQ",
    "ASSIGNM",
    "SEMICOLON",
    "LEFT PARENTHESIS",
    "RIGHT PARENTHESIS",
    "LEVA ZAVORKA {",
    "PRAVA ZAVORKA }",
    "COMMA",                //23
    "KEY_boolean",
    "KEY_break",
    "KEY_class",
    "KEY_continue",
    "KEY_do",
    "KEY_double",
    "KEY_else",
    "KEY_false",
    "KEY_for",
    "KEY_if",
    "KEY_int",
    "KEY_return",
    "KEY_String",
    "KEY_static",
    "KEY_true",
    "KEY_void",
    "KEY_while"
};

const char *KEY_TABLE[MAX_KEY] = {
    "boolean",
    "break",
    "class",
    "continue",
    "do",
    "double",
    "else",
    "false",
    "for",
    "if",
    "int",
    "return",
    "String",
    "static",
    "true",
    "void",
    "while"
};

typedef struct token {
    string *data;
    TokenType type;
} tToken;



tToken* getNextToken(FILE *in);


//---------------------------------------------------------------STAVY---------------------------------------------------------------------
enum{
    START = 1,
    CISLO_INT = 2,
    P_DOUBLE1 = 3, 
    P_DOUBLE = 4,
    CISLO_DOUBLE = 5,
    P_EXP = 6,              //potential exponent    -->EXP_OP, -->CISLO_DOUBLE
    EXP_OP = 7,             //exp s operatorem +, -
    EXP = 8,
    SIMPLE_ID = 9,          // a, $, _, A, a pak slozitejsi  
    P_FULL_ID = 10,
    FULL_ID = 11,
    P_STRING = 12,
    P_ESC_SEQ = 13,
    P_OCT1 = 14,
    P_OCT2 = 15,
    P_DIVIDE = 16,
    STR_COMMENT = 17,
    P_BLOCK_COMMENT = 18,
    FINISH_BL_COM,
    P_EQUAL,
    P_GRT_EQ,
    P_LESS_EQ,
    P_NOT_EQUAL
};

//-------------------ERRORS-------------------------------
/*
typedef enum ERRORS{
    ERR_OK = 0,
    ERR_LEX = 1,
    ERR_SYNT = 2,
    ERR_SEM_UNDEF = 3,
    ERR_SEM_TYP = 4,
    ERR_SEM_OTHERS = 6,
    ERR_NUM_INPT = 7,
    ERR_VAR_UNINIT = 8,
    ERR_DIVZERO = 9,
    ERR_OTHERS = 10,
    ERR_INTERN = 99
}ErrorCode;
*/
typedef enum Error {
    ERROR_UNDEFINED = 0,    
    ERROR_FILE = 1,       
    ERROR_MALLOC = 2,
    ERROR_UNFINISHED_COM = 3,
    ERROR_WRONGTYPE = 4,
    ERROR_STR = 5,
    ERROR_ESC = 6
} Error;

const char *ERR_TABLE[20] = {
    "Neznama chyba.",
    "Chyba otevreni souboru.",
    "Chyba alokace.",
    "Nedokonceny komentar.",
    "Unknown type.",
    "Neuzavreny retezec.",
    "Nemuze byt zadane samotne lomitko -> chybna escape sekvence."
};


//--------------------------------------------------------
void printErr(Error TypeError, ErrorCode ReturnCode);


#endif