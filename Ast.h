#pragma once
#include <vector>
#include <string>
#include "types.h"

class ASTNode {

public:
	virtual ~ASTNode() = default;
	// 가상소멸자 : 부모 포인터로 delete 할 때 자식 클래스 소멸자도 호출.
};


// 식(expression) --------------------------------------------------------------
class Expression : public ASTNode {

public:
	// 가상소멸자
	virtual ~Expression() = default;

};

// Alias
using ExprPtr = std::unique_ptr<Expression>;

// 컬럼 참조  age, name
class ColumnExpression : public Expression {
	
public:
	std::string column_name;

	// 1. 생성자
	explicit ColumnExpression(std::string& name) : column_name(name) {};
};

// 리터럴  123, 'hello'
class LiteralExpression : public Expression {
	
public: 
	DataType type;
	std::string value; 
	//일단 스트링으로 저장

	// 1. 생성자
	LiteralExpression(DataType t, const std::string& v) :type(t), value(v) {}

};

// 이항 연산 a + b, age > 20, x AND y
class BinaryExpression : public Expression {

public :
	// 연산자
	std::string op;

	// 좌항
	ExprPtr left; 

	// 우항
	ExprPtr right;

	// 생성자
	BinaryExpression(std::string op_, ExprPtr lhs, ExprPtr rhs) :
		op(std::move(op_)), left(std::move(lhs)), right(std::move(rhs)) {}
	// ExprPtr 객체가 유니크포인터를 사용하므로 move를 사용해야만 소유권이 이전됨.

};

// 단항 연산 not, -a
class UnaryExpression : public Expression {
	
	//연산자
	std::string op;

	//항
	ExprPtr operand;

	//생성자
	UnaryExpression(std::string op_, ExprPtr expr) 
		: op(std::move(op_)), operand(std::move(expr)) {}

};




// 문(Statement) -------------------------------------------------
class Statement : public ASTNode { // : public ASTNode : Statement는 ASTNode의 자식 클래스

};


// SELECT 
class SelectStatement : public Statement {

public:
	std::vector<std::string> select_list;
	std::string from;
	ExprPtr where;
};


// CRETAE
struct ColumnDef {

	std::string name;
	DataType type;

};

class CreateStatement : public Statement {

public :
	std::string table;
	std::vector <ColumnDef> columns;
};


// INSERT 
class InsertStatement : public Statement {
	
public :
	std::string table;
	std::vector <std::string> values; 
	//TODO 나중에 숫자 넣을 수 있게 variant 객체?
};


