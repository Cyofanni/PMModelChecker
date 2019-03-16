#Flex and Bison input generator;
#reads a configuration file like this:
#   and	    2
#   or      2
#   diamond 1
#   myop    3
import sys
import os

standard_macros = {"EQ":"=", "EQMIN":"=min", "EQMAX":"=max", "ID":"[x][0-9]+","PROP":"[p][0-9]+","PHI":"phi_[0-9]+_[a-z]+", 
		   "PHI_PROP":"phi_[0-9]+_[p][0-9]+", "ATOM":"[b_[0-9]+,[0-9]+]", "TRUE": "true", "FALSE": "false"}

#read the operators and their arities
class OperatorReader:
	def __init__(self, input_file):
		self._file_handler = open(input_file, "r")
	def read(self):
		operators = {}
		for line in self._file_handler.readlines():
			line = line.strip()
			if not (line.startswith('#')):
				columns = line.split("\t")
				operator = columns[0]
				arity = int(columns[1])
				if (arity > 6):
					print "Input error: some arities are not within bounds (max = 6)"
					sys.exit()
				operators[operator] = arity
		self._file_handler.close()
		return operators



class FlexWriter:
	#operators: operators with arity read from input file
	def __init__(self, operators, output_file):
		self._file_handler = open(output_file, "wa")
		self._operators = operators
		#build macro-token map: {"EQ":"EQ_TOK", "ID":"ID_TOK" etc...}
		self._macro_token_map = {}
		for macro in standard_macros.keys():
			self._macro_token_map[macro] = macro + "_TOK"
		for op in operators.keys():
			op_macro = op.upper()
			self._macro_token_map[op_macro] = op_macro + "_TOK"

	#associate a token ("*_TOK") to each macro
	def get_macro_token_map(self):
		return self._macro_token_map

	def flex_header1_printer(self):
		self._file_handler.write("%option interactive\n%{\n%}\n%{\n#include <string.h>\n#include \"../include/expr_move.h\"\n#include \"y.tab.h\"\n%}\n")

	def flex_header2_printer(self):
		self._file_handler.write("%{\n%}\n%x ERROR\n%%\n%{\n%}\n")

	#write EQ,EQMIN,PHI etc... (general tokens)
	def flex_std_macro_printer(self):
		for macro in standard_macros.keys():
			self._file_handler.write(macro + "    " + standard_macros[macro] + "\n")
	#write macros derived from custom operators
	def flex_custom_macro_printer(self):
		for op in self._operators.keys():   #loop over the keys (operators)
			macro = op.upper()
			self._file_handler.write(macro + "    " + op + "\n");
	#write lexer rules
	def flex_rules_printer(self):
		for k in self._macro_token_map.keys():
			self._file_handler.write("{" + k + "}" + "    "  + "{yylval.id = strdup(yytext); return " + self._macro_token_map[k] + ";}\n")

	def flex_error_handling_printer(self):
		self._file_handler.write("[,()\\n] {return *yytext;}\n[ \\t]+  ;\n. {BEGIN(ERROR);yymore();}\n<ERROR>[^{DIGIT}{LETTER}+\-/*()= \\t\\n\\f\\r] { yymore();}\n<ERROR>(.|\\n) { yyless(yyleng-1);printf(\"error token: %s\\n\",yytext);return 1;\n BEGIN(INITIAL);}\n%%\n")

	def file_closer(self):
		self._file_handler.close()



class BisonWriter:
	#needs map for tokens, operator arities
	def __init__(self, macro_tokens, operators, output_file):
		self._macro_tokens = macro_tokens
		self._operators = operators
		self._file_handler = open(output_file, "wa")

	def bison_header_printer(self):
		self._file_handler.write("%{\n#include \"../include/expr_move.h\"\nusing namespace std;\nEqSystem *root;\nint yylex();\nvoid yyerror(char *s);\n%}\n")

	def bison_definitions_printer(self):
		self._file_handler.write("%start system\n")
		self._file_handler.write("%union {\n char *id;\n char *op;\n char *equal;\n char *atom;\n ExpNode *expnode;\n vector<Equation*> *equations;\n Equation *eq;\n EqSystem *sys;\n}\n")

		for tok in self._macro_tokens.values():
			angle_brackets = ""
			if tok[0:2] == "EQ":
				angle_brackets = "<equal>"
			elif tok[0:3] == "PHI" or tok[0:2] == "ID" or tok[0:4] == "PROP" or tok[0:8] == "PHI_PROP":
				angle_brackets = "<id>"
			elif tok[0:4] == "ATOM" or tok[0:4] == "TRUE" or tok[0:4] == "FALSE":
				angle_brackets = "<atom>"
			else:
				angle_brackets = "<op>"
				self._file_handler.write("%left " + tok + "\n")		#added to handle shift/reduce conflicts

			self._file_handler.write("%token" + " " + angle_brackets + " " + tok + "\n")

		self._file_handler.write("%type <expnode> expr_eq\n%type <expnode> expr_move\n%type <equations> equationlist\n%type <eq> equation\n%type <sys> system\n%%\n")

	def bison_grammar_printer(self):
		self._file_handler.write("system: equationlist {$$ = new EqSystem($1); root = $$;}\n;\n")
		self._file_handler.write("equationlist: equationlist \'\\n\'  {$$ = $1;}\n  | equationlist equation \'\\n\'  {$$ = $1;$1->push_back($2);}\n  |  {$$ = new vector<Equation*>();}\n;\n")
		self._file_handler.write("equation:  ID_TOK EQMIN_TOK expr_eq  {$$ = new Equation($1,$2,$3);}\n  | ID_TOK EQMAX_TOK expr_eq  {$$ = new Equation($1,$2,$3);}\n  | PHI_TOK EQ_TOK expr_move {$$ = new Equation($1,$2,$3);}\n  | PHI_PROP_TOK EQ_TOK expr_move {$$ = new Equation($1,$2,$3);}\n ;\n")
		self._file_handler.write("expr_move: expr_move AND_TOK expr_move  {$$ = new ExpNode($2,$1,$3);}\n  | expr_move OR_TOK expr_move  {$$ = new ExpNode($2, $1,$3);}\n  | '(' expr_move ')'  { $$ = $2; }\n  | ATOM_TOK  {$$ = new ExpNode($1,0,0);}\n | TRUE_TOK  {$$ = new ExpNode($1,0,0);}\n | FALSE_TOK {$$ = new ExpNode($1,0,0);}\n;\n")

		self._file_handler.write("expr_eq:  ID_TOK  {$$ = new ExpNode($1,0,0);}\n | PROP_TOK  {$$ = new ExpNode($1,0,0);}\n")
		self._file_handler.write("  | '(' expr_eq ')'  {$$ = $2;}\n")
		for op in self._operators.keys():
			macro = op.upper()
			token = self._macro_tokens[macro]
			#branch on the arity
			arity = self._operators[op]
			#infix notation
			if (arity == 2):
				self._file_handler.write("  | " + "expr_eq " + token + " expr_eq  {$$ = new ExpNode($2,$1,$3);}\n")
			elif (arity != 2):
				#prefix notation, the number of expr_eq symbols depends on the arity
				self._file_handler.write("  | " + token)
				for i in range(arity):
					self._file_handler.write(" expr_eq ")
				self._file_handler.write(" {$$ = new ExpNode($1,")
				#call to constructor depends on arity, so write it piece by piece
				for i in range(arity):
					self._file_handler.write("$" + str(i+2))
					if (i < arity-1):
						self._file_handler.write(",")

				self._file_handler.write(");}\n")

		self._file_handler.write(";\n")
		self._file_handler.write("%%\n")
		


	def bison_error_handling_printer(self):
		self._file_handler.write("void yyerror(char *s){fprintf(stderr, \"%s\\n\", s); return;}\n")

	def file_closer(self):
		self._file_handler.close()



def main():
	if (len(sys.argv) < 4):
		print "usage: python syntax_generator.py [configuration_file] [flex_output_file] [bison_output_file]"
		sys.exit()

	opr = OperatorReader(sys.argv[1])
	operator_map = opr.read()

	#generate Flex input
	fw = FlexWriter(operator_map, sys.argv[2])

	fw.flex_header1_printer()
	fw.flex_custom_macro_printer();
	fw.flex_std_macro_printer();
	fw.flex_header2_printer()
	fw.flex_rules_printer();
	fw.flex_error_handling_printer()
	fw.file_closer()
	#bison will need the map for *_TOK tokens tokens
	macro_tokens = fw.get_macro_token_map()

	#generate Bison input
	bw = BisonWriter(macro_tokens, operator_map, sys.argv[3])
	bw.bison_header_printer()
	bw.bison_definitions_printer()
	bw.bison_grammar_printer()
	bw.bison_error_handling_printer()
	bw.file_closer()




if __name__ == "__main__":
	main()
