#pragma once
#include <vector>
#include <string>
#include <memory>
#include "types.h"
#include "Table.h"

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
enum class ExpressionClass {
	INVALID,
	COLUMN_REF,
	LITERAL,
	BINARY,
	UNARY
};

class Expression : public ASTNode {

public:
	ExpressionClass expression_class;

	// 생성자
	explicit Expression(ExpressionClass cls) : expression_class(cls) {};

	// 가상소멸자
	virtual ~Expression() = default;

	// 인터프리터
	virtual Value interpret (const Row& row) = 0;

};

// Alias
using ExprPtr = std::unique_ptr<Expression>;

// 컬럼 참조  age, name ===============================
class ColumnExpression : public Expression {
	
public:
	std::string column_name;
	int cached_idx = -1;

	// 1. 생성자
	explicit ColumnExpression(const std::string& name) 
		:Expression(ExpressionClass::COLUMN_REF), column_name(name) { }

	// 2. 방문자 
	void accept(AstVisitor& v) override;

	// 3. 인터프리터
	Value interpret(const Row& row) override { return row.values[cached_idx]; }

	// [중요] 실행 루프에 들어가기 전에 이 함수를 딱 한 번 호출해줍니다.
	void bind(const Table& t) {
		cached_idx = t.getColumnIdx(column_name);
	}
};


// 리터럴  123, 'hello' ===============================
class LiteralExpression : public Expression {
	
public: 
	DataType type;
	Value value; 
	//일단 스트링으로 저장

	// 1. 생성자
	LiteralExpression(DataType t, const Value& v) 
		:Expression(ExpressionClass::LITERAL), type(t), value(v) {}

	// 2. 방문자 
	void accept(AstVisitor& v) override;

	// 3. 인터프리터
	Value interpret(const Row& row) {
		return value;
	}
};

// 이항 연산 a + b, age > 20, x AND y =============================
class BinaryExpression : public Expression {

public :
	// 연산자
	BinaryOp op;

	// 좌항
	ExprPtr left; 

	// 우항
	ExprPtr right;

	// 생성자
	BinaryExpression(BinaryOp op_, ExprPtr lhs, ExprPtr rhs) :
		Expression(ExpressionClass::BINARY),
		op(std::move(op_)), left(std::move(lhs)), right(std::move(rhs)) {}
	// ExprPtr 객체가 유니크포인터를 사용하므로 move를 사용해야만 소유권이 이전됨.

	// 2. 방문자 
	void accept(AstVisitor& v) override;

	// 3. 인터프리터
	Value interpret(const Row& row) override{

		Value leftVal = left->interpret(row);
		Value rightVal = right->interpret(row);

		return evaluateBinary(leftVal, op, rightVal);
	}
};

// 단항 연산 not, -a ==============================================
class UnaryExpression : public Expression {
	
public:
	//연산자
	std::string op;

	//항
	ExprPtr operand;

	//생성자
	UnaryExpression(std::string op_, ExprPtr expr) :
		Expression(ExpressionClass::UNARY),
		op(std::move(op_)), operand(std::move(expr)) {}

	// 2. 방문자 
	void accept(AstVisitor& v) override;

	// TODO : 나중에 단항연산 추가하면 추가
	Value interpret(const Row& row) {};
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
class CreateStatement : public Statement {

public :
	std::string table;
	std::vector <Column> columns;

	// 2. 방문자 
	void accept(AstVisitor& v) override;

};


// INSERT 
class InsertStatement : public Statement {
	
public :
	std::string table;
	std::vector <Value> values; 
	//TODO 나중에 숫자 넣을 수 있게 variant 객체?

	// 2. 방문자 
	void accept(AstVisitor& v) override;

};


