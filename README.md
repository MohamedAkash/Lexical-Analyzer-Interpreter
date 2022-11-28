# Lexical-Analyzer-Interpreter
The syntax definitions of the small programming language are given below using EBNF notations.

Prog ::= PROGRAM IDENT StmtList END PROGRAM
StmtList ::= Stmt; {Stmt;}
Stmt ::= DeclStmt | ControlStmt
DeclStmt ::= (INT | FLOAT | STRING) VarList
ControlStmt ::= AssigStmt | IfStmt | WriteStmt | RepeatStmt | CompStmt
WriteStmt ::= WRITE ExprList
IfStmt ::= IF (LogicExpr) ControlStmt
RepeatStmt ::= REPEAT (Var = Expr, Expr [,ICONST]) ControlStmt
AssignStmt ::= Var = Expr
CompStmt ::= BEGIN StmtList END
VarList ::= Var {,Var}
ExprList ::= Expr {, Expr}
Expr ::= Term {(+|-) Term}
Term ::= SFactor {( *| / | % ) SFactor}
SFactor ::= (+ | -) Factor | Factor
LogicExpr ::= Expr (== | >) Expr
Var ::= IDENT
Factor = IDENT | ICONST | RCONST | SCONST | (Expr)
