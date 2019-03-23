%{
#include "../include/expr_move.h"
using namespace std;
EqSystem *root;
int yylex();
void yyerror(char *s);
%}
%start system
%union {
 char *id;
 char *op;
 char *equal;
 char *atom;
 ExpNode *expnode;
 vector<Equation*> *equations;
 Equation *eq;
 EqSystem *sys;
}
%left AND_TOK
%token <op> AND_TOK
%left BOX_TOK
%token <op> BOX_TOK
%token <id> PHI_TOK
%left DIAMOND_TOK
%token <op> DIAMOND_TOK
%left FALSE_TOK
%token <op> FALSE_TOK
%token <id> PHI_PROP_TOK
%token <equal> EQ_TOK
%token <id> PROP_TOK
%token <equal> EQMAX_TOK
%token <atom> ATOM_TOK
%token <equal> EQMIN_TOK
%token <atom> TRUE_TOK
%token <id> ID_TOK
%left OR_TOK
%token <op> OR_TOK
%type <expnode> expr_eq
%type <expnode> expr_move
%type <equations> equationlist
%type <eq> equation
%type <sys> system
%%
system: equationlist {$$ = new EqSystem($1); root = $$;}
;
equationlist: equationlist '\n'  {$$ = $1;}
  | equationlist equation '\n'  {$$ = $1;$1->push_back($2);}
  |  {$$ = new vector<Equation*>();}
;
equation:  ID_TOK EQMIN_TOK expr_eq  {$$ = new Equation($1,$2,$3);}
  | ID_TOK EQMAX_TOK expr_eq  {$$ = new Equation($1,$2,$3);}
  | PHI_TOK EQ_TOK expr_move {$$ = new Equation($1,$2,$3);}
  | PHI_PROP_TOK EQ_TOK expr_move {$$ = new Equation($1,$2,$3);}
 ;
expr_move: expr_move AND_TOK expr_move  {$$ = new ExpNode($2,$1,$3);}
  | expr_move OR_TOK expr_move  {$$ = new ExpNode($2, $1,$3);}
  | '(' expr_move ')'  { $$ = $2; }
  | ATOM_TOK  {$$ = new ExpNode($1,0,0);}
 | TRUE_TOK  {$$ = new ExpNode($1,0,0);}
 | FALSE_TOK {$$ = new ExpNode($1,0,0);}
;
expr_eq:  ID_TOK  {$$ = new ExpNode($1,0,0);}
 | PROP_TOK  {$$ = new ExpNode($1,0,0);}
  | '(' expr_eq ')'  {$$ = $2;}
  | expr_eq AND_TOK expr_eq  {$$ = new ExpNode($2,$1,$3);}
  | BOX_TOK expr_eq  {$$ = new ExpNode($1,$2);}
  | DIAMOND_TOK expr_eq  {$$ = new ExpNode($1,$2);}
  | expr_eq OR_TOK expr_eq  {$$ = new ExpNode($2,$1,$3);}
;
%%
void yyerror(char *s){fprintf(stderr, "%s\n", s); return;}
