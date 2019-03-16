CCC = g++
CCFLAGS= -O2 -Wno-write-strings 
LEX = flex
LFLAGS= -8     
YACC= bison 
YFLAGS= -d -t -y

RM = /bin/rm -f

pm_model_checker: src/y.tab.o src/lex.yy.o src/pm_model_checker.o src/solver.o src/dependency_graph.o src/move_composer.o src/mu_calculus_move_generator.o src/aldebaran_parser.o src/input_checker.o src/expr_move.o src/operator.o src/position.o
	${CCC} ${CCFLAGS} src/lex.yy.o src/y.tab.o src/pm_model_checker.o src/solver.o src/dependency_graph.o src/move_composer.o src/mu_calculus_move_generator.o src/aldebaran_parser.o src/input_checker.o src/expr_move.o src/operator.o src/position.o -o pm_model_checker -lfl
	
src/solver.o: src/solver.cpp include/solver.h
	${CCC} -c -o src/solver.o src/solver.cpp
	
src/dependency_graph.o: src/dependency_graph.cpp include/dependency_graph.h
	${CCC} -c -o src/dependency_graph.o src/dependency_graph.cpp
		
src/mu_calculus_move_generator.o: src/mu_calculus_move_generator.cpp include/mu_calculus_move_generator.h
	${CCC} -c -o src/mu_calculus_move_generator.o src/mu_calculus_move_generator.cpp

src/aldebaran_parser.o: src/aldebaran_parser.cpp include/aldebaran_parser.h
	${CCC} -c -o src/aldebaran_parser.o src/aldebaran_parser.cpp

src/move_composer.o: src/move_composer.cpp include/move_composer.h
	${CCC} -c -o src/move_composer.o  src/move_composer.cpp

src/input_checker.o: src/input_checker.cpp include/input_checker.h
	${CCC} -c -o src/input_checker.o src/input_checker.cpp

src/operator.o: src/operator.cpp include/operator.h
	${CCC} -c -o src/operator.o src/operator.cpp

src/position.o: src/position.cpp include/position.h
	${CCC} -c -o src/position.o src/position.cpp

src/expr_move.o: src/expr_move.cpp include/expr_move.h
	${CCC} -c -o src/expr_move.o src/expr_move.cpp

src/y.tab.o: src/myparser.yacc
	${CCC} ${CCFLAGS} -o src/y.tab.o src/y.tab.c -c 

src/lex.yy.o: src/mylex.l
	${CCC} ${CCFLAGS} -o src/lex.yy.o src/lex.yy.c -c 

clean:
	/bin/rm -f src/lex.yy.* src/y.tab.* src/*.o

parser:
	${LEX} $(LFLAGS) src/mylex.l
	${YACC} ${YFLAGS} src/myparser.yacc
	/bin/mv lex.yy.c y.tab.c y.tab.h src/
