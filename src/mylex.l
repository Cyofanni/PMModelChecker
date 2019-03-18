%option interactive
%{
%}
%{
#include <string.h>
#include "../include/expr_move.h"
#include "y.tab.h"
%}
AND    and
OR    or
EQMAX    =max
PHI    phi_[0-9]+_[a-z]+
FALSE    false
ATOM    [b_[0-9]+,[0-9]+]
PHI_PROP    phi_[0-9]+_[p][0-9]+
TRUE    true
PROP    [p][0-9]+
EQ    =
ID    [x][0-9]+
EQMIN    =min
%{
%}
%x ERROR
%%
%{
%}
{AND}    {yylval.id = strdup(yytext); return AND_TOK;}
{PHI}    {yylval.id = strdup(yytext); return PHI_TOK;}
{FALSE}    {yylval.id = strdup(yytext); return FALSE_TOK;}
{PHI_PROP}    {yylval.id = strdup(yytext); return PHI_PROP_TOK;}
{EQ}    {yylval.id = strdup(yytext); return EQ_TOK;}
{PROP}    {yylval.id = strdup(yytext); return PROP_TOK;}
{EQMAX}    {yylval.id = strdup(yytext); return EQMAX_TOK;}
{ATOM}    {yylval.id = strdup(yytext); return ATOM_TOK;}
{EQMIN}    {yylval.id = strdup(yytext); return EQMIN_TOK;}
{TRUE}    {yylval.id = strdup(yytext); return TRUE_TOK;}
{ID}    {yylval.id = strdup(yytext); return ID_TOK;}
{OR}    {yylval.id = strdup(yytext); return OR_TOK;}
[,()\n] {return *yytext;}
[ \t]+  ;
. {BEGIN(ERROR);yymore();}
<ERROR>[^{DIGIT}{LETTER}+\-/*()= \t\n\f\r] { yymore();}
<ERROR>(.|\n) { yyless(yyleng-1);printf("error token: %s\n",yytext);return 1;
 BEGIN(INITIAL);}
%%
