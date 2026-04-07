#pragma once
#include <vector>
#include <string>
#include <memory>
#include "types.h"

// 전방선언
class AstVisitor;

class ASTNode {

public:
	// 가상소멸자 : 부모 포인터로 delete 할 때 자식 클래스 소멸자도 호출.
	virtual ~ASTNode() = default;
	// * virtual 
	// 이 함수는 자식이 재정의할 수 있고, 부모 포인터로 불러도 실제 객체 기준으로 동작해라
	// override -> 부모의 virtual 함수를 재정의
	// 부모객체에서 virtual로 선언하지 않으면 override가 불가함.

	// 방문자
	virtual void accept(class AstVisitor& v) = 0;
	/* 방문자패턴
	* 
	단일 디스패치
	* - 객체지향 언어의 메소드 호출은 단일 디스패치
	메소드를 받는 객체의 실제 타입만 보고 메소드 실행 결정. => 상속
	
	이중 디스패치
	방문 대상 객체의 타입 + 방문자 객체의 타입 2개로 실행될 메소드를 결정
	
	*/ 
	
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
	explicit ColumnExpression(const std::string& name) : column_name(name) {}

	// 2. 방문자 
	void accept(AstVisitor& v) override;
};

// 리터럴  123, 'hello'
class LiteralExpression : public Expression {
	
public: 
	DataType type;
	std::string value; 
	//일단 스트링으로 저장

	// 1. 생성자
	LiteralExpression(DataType t, const std::string& v) :type(t), value(v) {}

	// 2. 방문자 
	void accept(AstVisitor& v) override;
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

	// 2. 방문자 
	void accept(AstVisitor& v) override;

};

// 단항 연산 not, -a
class UnaryExpression : public Expression {
	
public:
	//연산자
	std::string op;

	//항
	ExprPtr operand;

	//생성자
	UnaryExpression(std::string op_, ExprPtr expr) 
		: op(std::move(op_)), operand(std::move(expr)) {}

	// 2. 방문자 
	void accept(AstVisitor& v) override;

};




// 문(Statement) -------------------------------------------------
class Statement : public ASTNode { // : public ASTNode : Statement는 ASTNode의 자식 클래스

};


// SELECT 
class SelectStatement : public Statement {

public:
	std::vector<std::string> select_list;
	std::string from;
	ExprPtr where; // TODO : 나중에 vector 객체로 전환?


	// 2. 방문자 
	void accept(AstVisitor& v) override;

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

	// 2. 방문자 
	void accept(AstVisitor& v) override;

};


// INSERT 
class InsertStatement : public Statement {
	
public :
	std::string table;
	std::vector <std::string> values; 
	//TODO 나중에 숫자 넣을 수 있게 variant 객체?

	// 2. 방문자 
	void accept(AstVisitor& v) override;

};


