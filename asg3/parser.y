%{

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "astree.h"
#include "lyutils.h"
#include <iostream>

using namespace std;

extern FILE* outFile;
extern int exit_status;

%}

%debug
%defines
%error-verbose
%token-table
%verbose

%destructor { destroy ($$); } <>
%printer { astree::dump (yyoutput, $$); } <>

%initial-action {
   parser::root = new astree (ROOT, {0, 0, 0}, "<<ROOT>>");
}

%token  ROOT IDENT NUMBER UNOP BINOP
%token  TOK_IF TOK_ELSE TOK_WHILE TOK_RET TOK_IFELSE
%token  TOK_INT TOK_STRING TOK_STRUCT TOK_VOID TOK_PTR
%token  TOK_VAR TOK_FUNC TOK_PARAM TOK_INDEX
%token  TOK_NULL TOK_ARRAY TOK_TYPEID TOK_ATTR
%token  TOK_ASTRING TOK_ANARRAY TOK_CALL TOK_EXC
%token  TOK_EQ TOK_NEQ TOK_LEQ TOK_GEQ
%token  TOK_INTCON TOK_CHARCON TOK_STRINGCON
%token  TOK_ALLOC TOK_BLOCK TOK_POS TOK_NEG

%token  '('  ')'  '['  ']'  '{'  '}'  ';'  ','  '.'
%token  '<'  '>'  '='  '+'  '-'  '*'  '/'  '%'  '!'
%token  '\''  '"'  '\\'

%right  TOK_IF TOK_ELSE
%right  '='
%left   TOK_EQ TOK_NEQ '<' TOK_LEQ '>' TOK_GEQ
%left   '+' '-'
%left   '*' '/' '%'
%right  POS NEG TOK_EXC TOK_NEW
%right  '^'
%left   '[' '.'

%nonassoc '('

%start  start


%%

start     : program                   { $$ = $1 = nullptr; }

program   : program structdef         { $$ = $1->adopt($2); }
          | program function          { $$ = $1->adopt($2); }
          | program vardecl           { $$ = $1->adopt($2); }
          | program error '}'         { exit_status = EXIT_FAILURE;
                                        destroy($3); $$ = $1; }
          | program error ';'         { exit_status = EXIT_FAILURE;
                                        destroy($3); $$ = $1; }
          |                           { $$ = parser::root; }
          ;

structdef : TOK_STRUCT IDENT '{' '}'  { destroy($3, $4);
                                        $2->swap(TOK_TYPEID);
                                        $$ = $1->adopt($2); }
          | TOK_STRUCT IDENT '{' attr '}'
                                      { destroy($3, $5);
                                        $2->swap(TOK_TYPEID);
                                        $$ = $1->adopt($2, $4); }
          ;

attr      : type IDENT ';' attr       { destroy($3);
                                        $2->swap(TOK_ATTR);
                                        $$ = $1->adopt($2, $4); }
          | type IDENT ';'            { destroy($3);
                                        $2->swap(TOK_ATTR);
                                        $$ = $1->adopt($2); }
          ;

type      : plaintype                 { $$ = $1; }
          | TOK_ARRAY '<' plaintype '>'
                                      { destroy($2, $4);
                                        $$ = $3->adopt($1); }
          ;

plaintype : TOK_VOID                  { $$ = $1; }
          | TOK_INT                   { $$ = $1; }
          | TOK_STRING                { $$ = $1; }
          | TOK_PTR '<' TOK_STRUCT IDENT '>'
                                      { destroy($2, $5);
                                        $4->swap (TOK_TYPEID);
                                        $$ = $1->adopt($3, $4); }
          ;

function  : decl '('  ')' block       { $3->swap(TOK_FUNC);
                                        $2->swap(TOK_PARAM);
                                        $$ = $3->adopt($1, $2, $4); }
          | decl '(' decls ')' block  { $4->swap(TOK_FUNC);
                                        $2->swap(TOK_PARAM);
                                        $2->adopt($3);
                                        $$ = $4->adopt($1, $2, $5); }
          ;

decls     : decl ',' decls            { destroy($2);
                                        $$ = $1->adopt($3); }
          | decl                      { $$ = $1; }
          ;

decl      : type IDENT                { $2->swap(TOK_TYPEID);
                                        $$ = $1->adopt($2); }
          ;

block     : '{' '}'                   { destroy($2);
                                        $1->swap(TOK_BLOCK);
                                        $$ = $1; }
          | '{' statements '}'         { destroy($3);
                                        $1->swap(TOK_BLOCK);
                                        $$ = $1->adopt($2); }
          | ';'                       { $1->swap(TOK_BLOCK);
                                        $$ = $1; }
          ;

statements: statement statements     { $$ = $1->adopt($2); }
          | statement                 { $$ = $1; }
          ;

statement : block                     { $$ = $1; }
          | vardecl                   { $$ = $1; }
          | while                     { $$ = $1; }
          | ifelse                    { $$ = $1; }
          | return                    { $$ = $1; }
          | expr ';'                  { destroy ($2);
                                        $$ = $1; }
          ;

vardecl   : type IDENT ';'            { $3->swap(TOK_VAR);
                                        $2->swap(TOK_TYPEID);
                                        $$ = $3->adopt($1, $2); }
          | type IDENT '=' expr ';'   { destroy($5);
                                        $3->swap(TOK_VAR);
                                        $2->swap(TOK_TYPEID);
                                        $$ = $3->adopt($1, $2, $4); }
          ;

while     : TOK_WHILE '(' expr ')' statement
                                      { destroy($2, $4);
                                        $$ = $1->adopt($3, $5); }
          ;

ifelse    : TOK_IF '(' expr ')' statement %prec TOK_IF
                                      { destroy($2, $4);
                                        $$ = $1->adopt($3, $5); }
          | TOK_IF '(' expr ')' statement TOK_ELSE statement
                                      { destroy($2, $4);
                                        destroy($5, $6);
                                        $1->swap(TOK_IFELSE);
                                        $$ = $1->adopt($3, $5, $7); }
          ;

return    : TOK_RET ';'            { destroy($2);
                                        $$ = $1; }
          | TOK_RET expr ';'       { destroy($3);
                                        $$ = $1->adopt($2); }
          ;

expr      : binop                     { $$ = $1; }
          | unop                      { $$ = $1; }
          | allocator                 { $$ = $1; }
          | call                      { $$ = $1; }
          | '(' expr ')'              { destroy($1, $3); $$ = $2; }
          | variable                  { $$ = $1; }
          | constant                  { $$ = $1; }
          ;

binop     : expr '=' expr             { $$ = $2->adopt($1, $3); }
          | expr '+' expr             { $$ = $2->adopt($1, $3); }
          | expr '-' expr             { $$ = $2->adopt($1, $3); }
          | expr '*' expr             { $$ = $2->adopt($1, $3); }
          | expr '/' expr             { $$ = $2->adopt($1, $3); }
          | expr '>' expr             { $$ = $2->adopt($1, $3); }
          | expr '<' expr             { $$ = $2->adopt($1, $3); }
          | expr '^' expr             { $$ = $2->adopt($1, $3); }
          | expr TOK_LEQ expr         { $$ = $2->adopt($1, $3); }
          | expr TOK_GEQ expr         { $$ = $2->adopt($1, $3); }
          | expr TOK_EQ expr          { $$ = $2->adopt($1, $3); }
          | expr TOK_NEQ expr         { $$ = $2->adopt($1, $3); }
          ;

unop      : '+' expr %prec POS        { $$ = $1->adopt_sym($2, TOK_POS); }
          | '-' expr %prec NEG        { $$ = $1->adopt_sym($2, TOK_NEG); }
          | '!' expr %prec TOK_EXC    { $$ = $1->adopt_sym($2, TOK_EXC); }
          ;

allocator : TOK_ALLOC '<' TOK_STRING '>' '(' expr ')'
                                      { destroy($2, $4, $5, $7);
                                        $1->swap(TOK_ASTRING);
                                        $$ = $1->adopt($3, $6); }
          | TOK_ALLOC '<' TOK_STRUCT IDENT '>' '(' ')'
                                      { destroy($2, $5, $6, $7);
                                        $4->swap(TOK_TYPEID);
                                        $$ = $1->adopt($3, $4); }
          | TOK_ALLOC '<' TOK_ARRAY '<' plaintype '>' '>' '(' expr ')'
                                      { destroy($2, $4, $6, $7);
                                        destroy($8, $10);
                                        $1->swap(TOK_ANARRAY);
                                        $$ = $1->adopt($3, $5, $9); }
          ;

call      : IDENT '(' params ')'      { destroy($4);
                                        $2->swap(TOK_CALL);
                                        $$ = $2->adopt($1, $3); }
          | IDENT '(' ')'             { destroy($3);
                                        $2->swap(TOK_CALL);
                                        $$ = $2->adopt($1); }
          ;

params    : expr ',' params           { destroy($2);
                                        $$ = $1->adopt($3); }
          | expr                      { $$ = $1; }
          ;

variable  : IDENT                     { $$ = $1; }
          | expr '[' expr ']'         { destroy($4);
                                        $2->swap(TOK_INDEX);
                                        $$ = $2->adopt($1, $3); }
          | expr TOK_ATTR IDENT       { $3->swap(TOK_ATTR);
                                        $$ = $2->adopt($1, $3); }
          ;

constant  : TOK_INTCON                { $$ = $1; }
          | TOK_CHARCON               { $$ = $1; }
          | TOK_STRINGCON             { $$ = $1; }
          | TOK_NULL                  { $$ = $1; }
          ;

%%

const char* parser::get_tname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}

bool is_defined_token (int symbol) {
   return YYTRANSLATE (symbol) > YYUNDEFTOK;
}
