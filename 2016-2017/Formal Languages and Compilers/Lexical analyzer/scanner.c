/* 
** Author: Anastasiia Stoika, xstoik00
** Project: Lexical Analyzer
*/

#include "scanner.h"

//--------------------------------------------------------------  MAIN  ---------------------------------------------------------------------
int main(int argc, char* argv[])
{
    
    if(argc != 2){
        printf("Error\n");
        return 1;
    }

    char *filename = malloc(50 * sizeof(char));
    filename = argv[1];

  //  printf("Input file: %s\n", filename);

    FILE *in;
    if((in = fopen(filename, "r")) == NULL)
      setError(ERR_FILE);


  //  FILE* in = fopen(argv[1], "r");
    tToken *tok;

    while ((tok = getNextToken(in))->type != TYPE_EOF){
        printf("TYPE >> %s\t\t\tDATA >> %s \n", TOKEN_TABLE[tok->type], tok->data->str);
    }


    return ERR_OK;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void printErr(Error TypeError, ErrorCode ReturnCode){

    printf("ERROR: %s -> exit(%d)\n", ERR_TABLE[TypeError], ReturnCode);
    //exit(ReturnCode);
    setError(ReturnCode);
}


tToken* getNextToken(FILE *in) {

    tToken *token = (struct token*)malloc(sizeof(struct token));
    string *tokenData = (struct str*)malloc(sizeof(struct str));
    token->data = tokenData;
    strInit(token->data);

    // Vymazat stare udaje
//    s->type = TYPE_UNSET;

    // Lokalni promenne
    int state = START;      // stav automatu
    int c;                  // Cteny znak
    char oct[4];

    while (true) {
        c = fgetc(in);   // Nacteni znaku

        //debug vystup
 //       printf("Znak: %c,  stav: %d\n", c, state);

        // Konecny automat
        switch (state) {
        case START:
            if (isspace(c)) {           // Mezera - ponechani stavu
                state = START;
            }
            else if (c >= '1' && c <= '9'){
                state = CISLO_INT;
                strAddChar(token->data, (char)c);
            }
            else if(c == '0'){          // 0.56
                state = P_DOUBLE1;
            }
            else if (c == EOF) {
                printf("------ Ukoncujem soubor --------\n");
                token->type = TYPE_EOF;
                return token;
            }
            else if(c == '_' || c == '$' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
                state = SIMPLE_ID;
                strAddChar(token->data, (char)c);
            }
            else if(c == '"'){
                state = P_STRING; 
            }
            else if(c == '/'){                          //bud' operator deleni, nebo zacatek komentare
                state = P_DIVIDE;
            }
            else if(c == '+'){
                token->type = TYPE_PLUS_OP;
                strAddChar(token->data, (char)c);
                return token;
            }
            else if(c == '-'){
                token->type = TYPE_MINUS_OP;
                strAddChar(token->data, (char)c);
                return token;
            }
            else if(c == '*'){
                token->type = TYPE_MULT_OP;
                strAddChar(token->data, (char)c);
                return token;
            }
            else if(c == ';'){
                token->type = TYPE_SEMICOLON;
                strAddChar(token->data, (char)c);
                return token;
            }
            else if(c == '('){
                token->type = TYPE_LPARENT;
                strAddChar(token->data, (char)c);
                return token;
            }
            else if(c == ')'){
                token->type = TYPE_RPARENT;
                strAddChar(token->data, (char)c);
                return token;
            }
            else if(c == '='){
                state = P_EQUAL;
                strAddChar(token->data, (char)c);
            }
            else if(c == '>'){
                state = P_GRT_EQ;
                strAddChar(token->data, (char)c);
            }
            else if(c == '<'){
                state = P_LESS_EQ;
                strAddChar(token->data, (char)c);
            }
            else if(c == '!'){
                state = P_NOT_EQUAL;
            }
            else if(c == '{'){
                token->type = TYPE_LPAR2;
                strAddChar(token->data, (char)c);
                return token;
            }
            else if(c == '}'){
                token->type = TYPE_RPAR2;
                strAddChar(token->data, (char)c);
                return token;
            }
            else if(c == ','){
                token->type = TYPE_COMMA;
                strAddChar(token->data, (char)c);
                return token;
            }
            else{
                printErr(ERROR_WRONGTYPE, ERR_LEX);
            }
            break;
        case CISLO_INT:
            if(c >= '0' && c <= '9')                   // --> tu se budou ukladat znaky
                strAddChar(token->data, (char)c);
            else if(c == '.'){
                state = P_DOUBLE;
                strAddChar(token->data, (char)c);
            }
            else if(c == 'e' || c == 'E'){
                state = P_EXP;
                strAddChar(token->data, (char)c);
            }
            else{    
                ungetc(c, in);
                token->type = TYPE_INTEGER;
                return token;
            }
            break;
        case P_DOUBLE1:                                   //je-li prvni nactena nula
            if(c == '0'){
//                printf("Ignore nuly\n");
            }                                //prebytecne nuly ignorovat az nenalezena tecka nebo cele cislo
            else if(c == '.'){
                state = P_DOUBLE;
                strAddChar(token->data, '0');
                strAddChar(token->data, (char)c);
            }
            else if(c == 'e' || c == 'E'){
                state = P_EXP; 
                strAddChar(token->data, '0');
                strAddChar(token->data, (char)c);
            }
            else if(c >= '1' && c <= '9'){               //ulozit toto cislo a pokracovat v integeru
 //               printf("Jdeme do state CISLO_INT\n");
                state = CISLO_INT;
                strAddChar(token->data, (char)c);
            }
            else{                                        //0aaa - nemuze byt ani identifikator 
                ungetc(c, in);
                token->type = TYPE_INTEGER;
                strAddChar(token->data, '0');
                return token;
            }
            break;
        case P_DOUBLE:
            if(c >= '0' && c <= '9'){
                state = CISLO_DOUBLE;
                strAddChar(token->data, (char)c);
            }
            else{                                        //12. -> nedokonceny double, 12.kje - nemuze byt identifikatorem
                printErr(ERROR_WRONGTYPE, ERR_LEX);   // => NEROZPOZNANY TYP
            }
            //else -> nahlasit chybu
            break;
        case CISLO_DOUBLE:
            if(c >= '0' && c <= '9')                    // --->tu se ukladaji cisla
                strAddChar(token->data, (char)c);
            else if(c == 'e' || c == 'E'){
                state = P_EXP;
                strAddChar(token->data, (char)c);
            } 
            else{                                        // 12.3ghs --> vratit znak odlisny od cislice, ten se pak rozpozna napr.jako identifikator
                ungetc(c, in);                       // ---> 12.3ghs se rozpozna jako TYPE_DOUBLE a TYPE_SMPL_ID
                token->type = TYPE_DOUBLE;
                return token;
            }
            break;
        case P_EXP:
            if(c >= '0' && c <= '9'){                       //12e9, 12E9, 12.12e4, 12.12E4 -->cislo double
                state = EXP;
                strAddChar(token->data, (char)c);
            }
            else if(c == '+' || c == '-'){                 //12e+ ----> je cislo po operatoru?
                state = EXP_OP;
                strAddChar(token->data, (char)c);
            }
            else{
  //              ungetc(c, in);     //12eHVacs napr. - po e,E jsou nejake hovadiny
                printErr(ERROR_WRONGTYPE, ERR_LEX);
            }
            break;
        case EXP_OP:
            if(c >= '0' && c <= '9'){
                state = EXP;                               //12e+6, 12E+6, 11.5e+1, 11.5E+1, -->taky s minusem
                strAddChar(token->data, (char)c);
            }
            else{
                printErr(ERROR_WRONGTYPE, ERR_LEX);
//                ungetc(c, in);     //12e+HOVADINY misto cisel 0-9
//                state = START;
            }
            break;
        case EXP:
            if(c >= '0' && c <= '9')
                strAddChar(token->data, (char)c);
            else{
                ungetc(c, in);
                token->type = TYPE_DOUBLE;
                return token; 
            }
            break;
        case SIMPLE_ID:
            if(c == '_' || c == '$' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
                strAddChar(token->data, (char)c);
            else if(c == '.'){                             // id. --> potential full id, kdyz ne nasleduje po . simple id - CHYBA
                state = P_FULL_ID;
                strAddChar(token->data, (char)c);
            }
            else{
                /*!!!!!!!!!!---------------------OVERIT NA KLICOVA SLOVA-----------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                **
                **---------------------------------------------------------------------------------------------*/
                int i;
                for(i = 0; i < MAX_KEY; i++){
                    //printf(".%s. : .%s.", KEY_TABLE[i], tokenData->str);
                    if(strcmp(KEY_TABLE[i], tokenData->str) == 0){
                        
                        ungetc(c, in);

                        token->type = i + OFFSET;
                        return token;
                    }
                }
                ungetc(c, in);
                token->type = TYPE_SMPL_ID;
                return token;

            }
            break;
        case P_FULL_ID:
            if(c == '_' || c == '$' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
                state = FULL_ID;
                strAddChar(token->data, (char)c);
            }
            else{                                // NAPR. $id. --> chyba
                printErr(ERROR_WRONGTYPE, ERR_LEX);
            }
            break; 
        case FULL_ID:
            if(c == '_' || c == '$' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
                strAddChar(token->data, (char)c);
            else{
                ungetc(c, in);
                token->type = TYPE_FULL_ID;
                return token;
            }
            break; 
        case P_STRING:                                  // chyba kdyz neuzavreny retezec
            if(c == '"'){                               // prazdny retezec --> ""
//                ungetc(c, in);
                token->type = TYPE_STRING;
                return token;
            }
            else if(c == EOF || !isprint(c))            //err string
                printErr(ERROR_STR, ERR_LEX);   
            else if(c == '\\'){                         // '\' - potential sequence
                state = P_ESC_SEQ;
            }                                                       //krome znaka 34 -> "
            else if((c > 31 && c < 34) || (c > 34 && c <= 255)){     //255 - posledni znak v tabulce ASCII (oct 377),
                strAddChar(token->data, (char)c);
            }
            else{
                printErr(ERROR_STR, ERR_LEX);
//                break;
            }
            //-------ELSE ERROR - "WFEWQEF ---> NEUZAVRENY RETEZEC NAPR.-------------------
            break;
        case P_ESC_SEQ:                          
            if(c == 'n'){
                state = P_STRING;
                strAddChar(token->data, ESC_N);
            }
            else if(c == 't'){
                state = P_STRING;
                strAddChar(token->data, ESC_T);
            }
            else if(c == '\\'){
                state = P_STRING;
                strAddChar(token->data, (char)c);
            }
            else if(c == '"'){
                state = P_STRING;
                strAddChar(token->data, (char)c);
            }
            else if(c >= '0' && c <= '3'){                         //MOZNA zacatek oktaloveho
                state = P_OCT1;                                    // --> \0, \1, \2, \3
                oct[0] = c;

//                strAddChar(token->data, '\\');
//                strAddChar(token->data, (char)c);
            }
            else{
                printErr(ERROR_ESC, ERR_LEX);
/*                state = P_STRING;                                  //jinak se proste ulozi to nactene lomitko do stringu
                strAddChar(token->data, '\\');
                strAddChar(token->data, (char)c);
*/
            }
            break;
        case P_OCT1:
            if(c >= '0' && c <= '7'){                              // --> \(0-3)(0-7)
                state = P_OCT2;
                oct[1] = c;
//                strAddChar(token->data, (char)c);
            }
            else{
                printErr(ERROR_ESC, ERR_LEX);
/*                state = P_STRING;                                  // --> \2c --> uz je proste posloupnost znaku
                strAddChar(token->data, '\\');
                strAddChar(token->data, oct[0]);
                strAddChar(token->data, (char)c);
*/
            }                
            // ELSE ----> UZ NENI OCTALOVE, ALE PROSTE STRING
            break;
        case P_OCT2:
            if(c >= '1' && c <= '7'){                               // --> \<001, 377>  --> rozpoznano octalove cislo
                state = P_STRING;
                oct[2] = c;
                oct[3] = '\0';
                strAddChar(token->data, oct2dec(oct));
            }
            else{
                printErr(ERROR_ESC, ERR_LEX);
/*                state = P_STRING;                                   // --> \01a - uz je proste posloupnost znaku
                strAddChar(token->data, '\\');
                strAddChar(token->data, oct[0]);
                strAddChar(token->data, oct[1]);
                strAddChar(token->data, (char)c);
*/
            } 
            break;
        case P_DIVIDE:
            if(c == '/'){
                state = STR_COMMENT;
            }
            else if(c == '*'){
                state = P_BLOCK_COMMENT;                            // /* --> zacatek potencialniho blokoveho komentare
            }
            else{
                ungetc(c, in);
                token->type = TYPE_DIVIDE_OP;
                strAddChar(token->data, '/');
                return token;
            }
            break;
        case STR_COMMENT:
            if(c == '\n' || c == EOF){
                state = START;
            }
            break;
        case P_BLOCK_COMMENT:                                        // /* ..... *
            if(c == '*'){                                            // jinak zas do P_BLOCK_COMMENT
                state = FINISH_BL_COM;                               // ale hvezdicka muze nevyskytnout, a proto bude chyba neukonc. komentare
            }
            else if(c == EOF){
                printErr(ERROR_UNFINISHED_COM, ERR_LEX);
            }
            break;
        case FINISH_BL_COM:
            if(c == '/')
                state = START;
            else if(c == EOF)
                printErr(ERROR_UNFINISHED_COM, ERR_LEX);
            else if(c == '*');
            else
                state = P_BLOCK_COMMENT;
            break;
        case P_EQUAL:
            if(c == '='){
                token->type = TYPE_EQUAL;
                strAddChar(token->data, (char)c);
                return token;
            }
            else{
                ungetc(c, in);                                      //vratit znak nasledujici za prvnim =, a vratit typ toho prvniho
                token->type = TYPE_ASSIGN;
                return token;                                          //OVERIT =+ napr. -> ma vratit type assign. a type_plus_op
            }
            break;
        case P_GRT_EQ:
            if (c == '='){
                token->type = TYPE_GRT_EQ;
                strAddChar(token->data, (char)c);
                return token;
            }
            else{
                ungetc(c, in);
                token->type = TYPE_GREATER;
                return token;
            }
            break;
        case P_LESS_EQ:
            if(c == '='){
                token->type = TYPE_LESS_EQ;
                strAddChar(token->data, (char)c);
                return token;
            }
            else{
                ungetc(c, in);
                token->type = TYPE_LESS;
                return token;
            }
            break;
        case P_NOT_EQUAL:
            if(c == '='){
                token->type = TYPE_NOT_EQ;
                strAddChar(token->data, '!');
                strAddChar(token->data, (char)c);
                return token;
            }
            else{
                printErr(ERROR_WRONGTYPE, ERR_LEX);
            }
            break;

        }
    }
}
