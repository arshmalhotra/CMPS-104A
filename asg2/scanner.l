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
EQUALCMP        "=="
NOTEQUAL        "!="
LESSTHANEQ      "<="
GRTERTHANEQ     ">="
POINTER         "->"

%%

"#".*           { scanner::include(); }
[ \t]+          { }
\n              { scanner::newline(); }

{NUMBER}        { return scanner::token (NUMBER); }
{IDENT}         { return scanner::token (IDENT); }
"="             { return scanner::token ('='); }
"+"             { return scanner::token ('+'); }
"-"             { return scanner::token ('-'); }
"*"             { return scanner::token ('*'); }
"/"             { return scanner::token ('/'); }
"%"             { return scanner::token ('%'); }
{EQUALCMP}      { return scanner::token (EQUALCMP); }
{NOTEQUAL}      { return scanner::token (NOTEQUAL); }
"<"             { return scanner::token ('<'); }
{LESSTHANEQ}    { return scanner::token (LESSTHANEQ); }
">"             { return scanner::token ('>'); }
{GRTERTHANEQ}   { return scanner::token (GRTERTHANEQ); }
{POINTER}       { return scanner::token (POINTER); }
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
