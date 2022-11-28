#include "parseInt.h"

map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults; 
queue <Value> * ValQue; 

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount(){
    return error_count;
}

void ParseError(int line, string msg){
	++error_count;
	cout << line << ": " << msg << endl;
}

bool Prog(istream& in, int& line){
	bool pp = false;
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			pp = StmtList(in, line);
			if( !pp  )
			{
				ParseError(line, "Incorrect Syntax in the Program");
				return false;
			}
				
			tok = Parser::GetNextToken(in, line);
			
			if (tok.GetToken() == END){
				tok = Parser::GetNextToken(in, line);
				
				if (tok.GetToken() != PROGRAM){
					
					ParseError(line, "Missing PROGRAM at the End");
					return false;
				}	
				return true;
			}
			else{
				ParseError(line, "Missing END of Program");
				return false;
			}	
		}
		else{
			ParseError(line, "Missing Program Name.");
				return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		return false;
	}
	else if(tok.GetToken() == DONE && tok.GetLinenum() <= 1){
		ParseError(line, "Empty File");
		return false;
	}
	ParseError(line, "Missing PROGRAM.");
	return false;
}

bool StmtList(istream& in, int& line){
	bool stat;
	LexItem tok;
	stat = Stmt(in, line);
	if(stat)
	{
		tok = Parser::GetNextToken(in, line);
		while(tok == SEMICOL && stat){
			stat = Stmt(in, line);
			tok = Parser::GetNextToken(in, line);
		}
		if(tok == END && stat){
			Parser::PushBackToken(tok);
			return true;
		}
		else if( tok == SEMICOL && !stat){
			ParseError(line, "Syntactic error in Statement.");
			return false;
		}
		else if(tok != SEMICOL && tok != END && stat){
			ParseError(line, "Missing a semicolon.");
			return false;
		}
	}
	else{
		Parser::PushBackToken(tok);
	}	
	return stat;	
}

bool ControlStmt(istream& in, int& line) {
	bool stat;
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {
	case WRITE:
		stat = WriteStmt(in, line);
		break;
	case IF:
		stat = IfStmt(in, line);
		break;
	case IDENT:
		Parser::PushBackToken(t);
        stat = AssignStmt(in, line);
		break;
	default:
		Parser::PushBackToken(t);
		return true;
	}
	return stat;
}


bool WriteStmt(istream& in, int& line) {
	LexItem t;
	ValQue = new queue<Value>;	
	bool exr = ExprList(in, line);
	if( !exr ) {
		ParseError(line, "Missing expression after Write");
		return false;
	}
    while (!(*ValQue).empty()){
        Value valNext = (*ValQue).front();
        cout << valNext;
        ValQue->pop();
    }
    cout << "\n";
	return exr;
}
bool DeclStmt(istream& in, int& line) {
	bool stat = false;
	LexItem tok;
	LexItem t = Parser::GetNextToken(in, line);
	if(t == INT || t == FLOAT ) {
		stat = IdentList(in, line, t);
		if (!stat){
			ParseError(line, "Incorrect variable in Declaration Statement.");
			return stat;
		}
	}
	else{
		Parser::PushBackToken(t);
		ParseError(line, "Incorrect Type.");
		return false;
	}
	return true;
}

bool Var(istream& in, int& line){
	string ident;
	LexItem tok = Parser::GetNextToken(in, line);
	if (tok == IDENT){
		ident = tok.GetLexeme();
		if (!(defVar.find(ident)->second)){
			ParseError(line, "Undeclared Variable");
			return false;
		}
		return true;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}

bool Stmt(istream& in, int& line){
	bool stat = true;
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {
	case INT: case FLOAT: 
		Parser::PushBackToken(t);
		stat = DeclStmt(in, line);
		if(!stat)
		{
			ParseError(line, "Incorrect Declaration Statement.");
			return stat;
		}
		break;
	case IF: case WRITE: case IDENT: 
		Parser::PushBackToken(t);
		stat = ControlStmt(in, line);
		if(!stat)
		{
			ParseError(line, "Incorrect control Statement.");
			return stat;
		}
		break;
	default:
		Parser::PushBackToken(t);
	}
	return stat;
}

bool IfStmt(istream& in, int& line) {
	bool exr = false ; 
	LexItem t;
	Value val;
	if( (t=Parser::GetNextToken(in, line)) != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	exr = LogicExpr(in, line, val);
	if( !exr ) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}

	if((t=Parser::GetNextToken(in, line)) != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	
	if(val.IsBool()){
		if(val.GetBool() ){
			bool stnt = ControlStmt(in, line);
			if( !stnt ) {
				ParseError(line, "Missing statement for if");
				return false;
			}
		}
		else
		{
			t = Parser::GetNextToken(in, line);
			while (t != SEMICOL){
				t = Parser::GetNextToken(in, line);
			}
			Parser::PushBackToken(t);
		}
		
	}
	else{
		ParseError(line, "Run-Time Error-Illegal Type for If statement condition");
		return false;
	}
	
	return true;
}

bool AssignStmt(istream& in, int& line) {
	bool status = false, stat = false;
	LexItem t, tok;
    Value value1;
    tok = Parser::GetNextToken(in, line);
    Parser::PushBackToken(tok);

	status = Var(in, line);	
	if (status){
		t = Parser::GetNextToken(in, line);
        
		if (t == ASSOP){
            t = Parser::GetNextToken(in, line);      
            
            Parser::PushBackToken(t);
            
			stat = Expr(in, line, value1);
			if(!stat) {
				ParseError(line, "Missing Expression in Assignment Statment");
				return stat;
			}
   
            
            if((SymTable.find(tok.GetLexeme())->second)==INT)   
            {
                if(value1.GetType()==VINT)
                {
                    TempsResults[tok.GetLexeme()] = value1;
                }
                else if(value1.GetType()==VREAL)
                {
                    int tempVal;
                    tempVal = value1.GetReal();
                    value1.SetType(VINT);
                    value1.SetInt(tempVal);
                    TempsResults[tok.GetLexeme()] = value1;
                }               
                else
                {
                    cout << "VINT" << endl;
                    ParseError(line, "Illegal Assignment Operation");
                    return false;
                }
            }
            else if((SymTable.find(tok.GetLexeme())->second) == FLOAT)
            {
                if(value1.GetType()==VINT)
                {
                    float tempVal;
                    tempVal = value1.GetInt();
                    value1.SetType(VREAL);
                    value1.SetReal(tempVal);
                    TempsResults[tok.GetLexeme()] = value1;
                }
                else if(value1.GetType()==VREAL)
                {
                    TempsResults[tok.GetLexeme()] = value1;
                }
                else
                {
                    ParseError(line, "Illegal Assignment Operation");
                    return false;
                }
            }
            else
            {
                return false;
            }   
            
		}
		else if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator =");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return stat;	
}

bool IdentList(istream& in, int& line, LexItem type) {
	bool stat = false;
	string ident;
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok == IDENT){
		ident = tok.GetLexeme();
		if (!(defVar.find(ident)->second)){
			defVar[ident] = true;
			SymTable[ident] = type.GetToken();
		}	
		else{
			ParseError(line, "Variable Redefinition");
			return false;
		}
		
	}
	else{
		ParseError(line, "Missing Variable");
		return false;
	}
	tok = Parser::GetNextToken(in, line);
	if (tok == COMMA) {
		stat = IdentList(in, line, type);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return stat;
}
bool Expr(istream& in, int& line, Value &retVal) {
    Value value1, value2;
	bool e1 = Term(in, line, value1);
    retVal = value1;
	LexItem tok;
	
	if( !e1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
    
	while ( tok == PLUS || tok == MINUS ) 
	{
        
		e1 = Term(in, line, value2);
		if( !e1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
        
        if (retVal.GetType() == VCHAR || value1.GetType() == VCHAR || value2.GetType() == VCHAR) 
        {
            ParseError(line, "Run-Time Error-Illegal Mixed Type Operands");
            return false;
        }
        else
        {
            if(tok == PLUS)
            {
                retVal = value1 + value2;
            }
            else if(tok == MINUS)
            {
                retVal = value1 - value2;
            }
        }
        
        
		
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
	}
	Parser::PushBackToken(tok);
	return true;
}

bool ExprList(istream& in, int& line) {
    Value value1;
	bool stat = false;
	stat = Expr(in, line, value1);
    
	if(!stat){
		ParseError(line, "Missing Expression");
		return false;
	}
    
	ValQue->push(value1);
	LexItem tok = Parser::GetNextToken(in, line);
    
	if (tok == COMMA) {
		stat = ExprList(in, line);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return stat;
}

bool SFactor(istream& in, int& line, Value &retVal)
{
	LexItem t = Parser::GetNextToken(in, line);
	bool stat;
	int sign = 0;
	if(t == MINUS ){
		sign = -1;
    }
	else if(t == PLUS){
		sign = 1;
	}
	else
		Parser::PushBackToken(t);
		
	stat = Factor(in, line, sign, retVal);
        
    if(sign == -1){
        if(retVal.IsInt()){
            retVal.SetInt(-1 * retVal.GetInt());

        }
        else if(retVal.IsReal()){
            retVal.SetReal(-1 * retVal.GetReal());
        }
    }
	return stat;
}

bool Term(istream& in, int& line, Value &retVal) {
    Value value1, value2;
	bool e1 = SFactor(in, line, value1);
    retVal = value1;
	LexItem tok;
	if( !e1 ) {
		return false;
	}
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
    
	while ( tok == MULT || tok == DIV  || tok == REM)
	{
		e1 = SFactor(in, line, value2);
		if( !e1 ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}
        if(retVal.GetType() == VCHAR || value1.GetType() == VCHAR || value2.GetType() == VCHAR)
        {   
            ParseError(line, "Run-Time Error-Illegal Mixed Type operation");
            return false;
        }
        else
        {
            if(tok == MULT)
            {
                retVal = retVal * value2;
            }
            else if(tok == DIV)
            {
                if(!(value2.GetInt() == 0 || value2.GetReal() == 0))
                {
                    retVal = retVal / value2;
                }
                else
                {
                    ParseError(line, "Division by zero");
                    return false;
                }
            }   
            else if(tok == REM)
            {
                if(retVal.GetType() == VINT && value2.GetType() == VINT)
                {    
                    retVal = retVal % value2;
                }
                else
                {
                    ParseError(line, "Run-Time Error-Illegal Operand Type for Remainder Operator");
                    return false;
                }
            }
        }
		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
}

bool LogicExpr(istream& in, int& line, Value & retVal)
{
	Value value1, value2;    
	bool e1 = Expr(in, line, value1);
    retVal = value1;
	LexItem tok;
	
	if( !e1 ) {
		return false;
	}
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == GTHAN  || tok == EQUAL) {
		e1 = Expr(in, line, value2);
		if(!e1) {
			ParseError(line, "Missing expression after relational operator");
			return false;
		}
		if(tok == GTHAN){
			retVal = (value1 > value2);
		}
		else if(tok == EQUAL){
			retVal = (value1 == value2);
		}
		if(retVal.GetType() == VERR){
			ParseError(line, "Run-Time Error-Illegal Mixed Type operation");
			return false;
		}
		return true;
	}
	Parser::PushBackToken(tok);
	return true;
}

bool Factor(istream& in, int& line, int sign, Value &retVal) {
	LexItem tok = Parser::GetNextToken(in, line);
	if( tok == IDENT ) {
		string lexeme = tok.GetLexeme();
        
		if (!(defVar.find(lexeme)->second)){
			ParseError(line, "Undefined Variable");
			return false;	
		}
        
        if(SymTable[lexeme] == INT || SymTable[lexeme] == FLOAT){
            retVal = TempsResults[lexeme];
        }
		return true;
	}
	else if( tok == ICONST) {
        retVal.SetType(VINT);
		retVal.SetInt(stoi(tok.GetLexeme()));
		return true;
	}
	else if( tok == SCONST) {
		retVal.SetType(VCHAR);
        retVal.SetChar(tok.GetLexeme());
		return true;
	}
	else if( tok == RCONST) {
		retVal.SetType(VREAL);
        retVal.SetReal(stof(tok.GetLexeme()));
		return true;
	}
	else if( tok == LPAREN) {
		bool exr = Expr(in, line, retVal);
		if( !exr ) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return exr;

		ParseError(line, "Missing ) after expression");
		return false;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		return false;
	}
	ParseError(line, "Unrecognized input");
	return 0;
}