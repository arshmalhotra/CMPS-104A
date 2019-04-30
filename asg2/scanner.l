%{

#include "lyutils.h"

#define YY_USER_ACTION  { scanner::advance(); }

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

(if)            { return scanner::token (TOK_IF); }
(else)          { return scanner::token (TOK_ELSE); }
(while)         { return scanner::token (TOK_WHILE); }
(return)        { return scanner::token (TOK_RET); }
(int)           { return scanner::token (TOK_INT); }
(string)        { return scanner::token (TOK_STRING); }
(struct)        { return scanner::token (TOK_STRUCT); }
(void)          { return scanner::token (TOK_VOID); }
(new)           { return scanner::token (TOK_NEW); }
(null)          { return scanner::token (TOK_NULL); }

(<=)            { return scanner::token (TOK_LEQ); }
(>=)            { return scanner::token (TOK_GEQ); }
(==)            { return scanner::token (TOK_EQ); }
(!=)            { return scanner::token (TOK_NEQ); }

(\[\])          { return scanner::token (TOK_ARRAY); }
{NUMBER}        { return scanner::token (NUMBER); }
{IDENT}         { return scanner::token (IDENT); }
"="             { return scanner::token ('='); }
"+"             { return scanner::token ('+'); }
"-"             { return scanner::token ('-'); }
"*"             { return scanner::token ('*'); }
"/"             { return scanner::token ('/'); }
"%"             { return scanner::token ('%'); }
"<"             { return scanner::token ('<'); }
">"             { return scanner::token ('>'); }
","             { return scanner::token (','); }
"("             { return scanner::token ('('); }
")"             { return scanner::token (')'); }
"["             { return scanner::token ('['); }
"]"             { return scanner::token (']'); }
"{"             { return scanner::token ('{'); }
"}"             { return scanner::token ('}'); }
";"             { return scanner::token (';'); }

{NOTNUMBER}     { return scanner::badtoken (NUMBER); }
.               { scanner::badchar (*yytext); }

%%
