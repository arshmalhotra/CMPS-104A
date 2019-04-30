%{

#include "lyutils.h"
#include "astree.h"

#define YY_USER_ACTION  { scanner::advance(); }

extern FILE* outFile;

int yylval_token (int symbol) {
  yylval = new astree (symbol, lexer::lloc, yytext);
  yylval->pretty_print(outFile, yylval);
  return symbol;
}

%}

%option 8bit
%option debug
%option nobackup
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option warn
/*%option verbose*/

LETTER          [A-Za-z_]
DIGIT           [0-9]
MANTISSA        ({DIGIT}+\.?{DIGIT}*|\.{DIGIT}+)
EXPONENT        ([Ee][+-]?{DIGIT}+)
NUMBER          ({MANTISSA}{EXPONENT}?)
NOTNUMBER       ({MANTISSA}[Ee][+-]?)
IDENT           ({LETTER}({LETTER}|{DIGIT})*)

STRING          (\"([^\\"\n]|\\[\\'"0nt])*\")
CHAR            ('([^\\'\n]|\\[\\'"0nt])')

%%

"#".*           { scanner::include(); }
[ \t]+          { }
\n              { scanner::newline(); }

(if)            { return yylval_token (TOK_IF); }
(else)          { return yylval_token (TOK_ELSE); }
(while)         { return yylval_token (TOK_WHILE); }
(return)        { return yylval_token (TOK_RET); }
(int)           { return yylval_token (TOK_INT); }
(string)        { return yylval_token (TOK_STRING); }
(struct)        { return yylval_token (TOK_STRUCT); }
(void)          { return yylval_token (TOK_VOID); }
(new)           { return yylval_token (TOK_NEW); }
(null)          { return yylval_token (TOK_NULL); }

(<=)            { return yylval_token (TOK_LEQ); }
(>=)            { return yylval_token (TOK_GEQ); }
(==)            { return yylval_token (TOK_EQ); }
(!=)            { return yylval_token (TOK_NEQ); }

(\[\])          { return yylval_token (TOK_ARRAY); }
{NUMBER}        { return yylval_token (NUMBER); }
{IDENT}         { return yylval_token (IDENT); }
"="             { return yylval_token ('='); }
"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"%"             { return yylval_token ('%'); }
"<"             { return yylval_token ('<'); }
">"             { return yylval_token ('>'); }
","             { return yylval_token (','); }
"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
"["             { return yylval_token ('['); }
"]"             { return yylval_token (']'); }
"{"             { return yylval_token ('{'); }
"}"             { return yylval_token ('}'); }
";"             { return yylval_token (';'); }

{NOTNUMBER}     { scanner::badtoken (NUMBER);
                  return yylval_token (NUMBER); }
.               { return yylval_token (-1); }

%%
