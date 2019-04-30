%{

#include "lyutils.h"
#include "astree.h"

#define YY_USER_ACTION  { scanner::advance(); }

extern FILE* outFile;

int scanner_token (int symbol) {
  yylval = new astree (symbol, scanner::lloc, yytext);
  yylval->print_tok(outFile, yylval);
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

(if)            { return scanner_token (TOK_IF); }
(else)          { return scanner_token (TOK_ELSE); }
(while)         { return scanner_token (TOK_WHILE); }
(return)        { return scanner_token (TOK_RET); }
(int)           { return scanner_token (TOK_INT); }
(string)        { return scanner_token (TOK_STRING); }
(struct)        { return scanner_token (TOK_STRUCT); }
(void)          { return scanner_token (TOK_VOID); }
(new)           { return scanner_token (TOK_NEW); }
(null)          { return scanner_token (TOK_NULL); }

(<=)            { return scanner_token (TOK_LEQ); }
(>=)            { return scanner_token (TOK_GEQ); }
(==)            { return scanner_token (TOK_EQ); }
(!=)            { return scanner_token (TOK_NEQ); }

(\[\])          { return scanner_token (TOK_ARRAY); }
{NUMBER}        { return scanner_token (NUMBER); }
{IDENT}         { return scanner_token (IDENT); }
"="             { return scanner_token ('='); }
"+"             { return scanner_token ('+'); }
"-"             { return scanner_token ('-'); }
"*"             { return scanner_token ('*'); }
"/"             { return scanner_token ('/'); }
"%"             { return scanner_token ('%'); }
"<"             { return scanner_token ('<'); }
">"             { return scanner_token ('>'); }
","             { return scanner_token (','); }
"("             { return scanner_token ('('); }
")"             { return scanner_token (')'); }
"["             { return scanner_token ('['); }
"]"             { return scanner_token (']'); }
"{"             { return scanner_token ('{'); }
"}"             { return scanner_token ('}'); }
";"             { return scanner_token (';'); }

{NOTNUMBER}     { scanner::badtoken (NUMBER);
                  return scanner_token (NUMBER); }
.               { scanner::badchar (*yytext); }

%%
