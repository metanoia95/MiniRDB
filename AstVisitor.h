#pragma once
#include "Ast.h"
#include <iostream>

// 전방 선언. 클래스가 있다는 것만 알려줌.(순환참조 방지)
// expression
class ColumnExpression;
class LiteralExpression;
class BinaryExpression;
class UnaryExpression;

// statement
class SelectStatement;
class CreateStatement;
class InsertStatement;

class AstVisitor {
	
public:
	virtual ~AstVisitor() = default;

	// expression
	virtual void visit(ColumnExpression& node) = 0; // 컬럼참조
	virtual void visit(LiteralExpression& node) = 0; // 리터럴 
	virtual void visit(BinaryExpression& node) = 0;  // 이항연산
	virtual void visit(UnaryExpression& node) = 0;  // 단항연산

	// statement
	virtual void visit(SelectStatement& node) = 0; // select 문
	virtual void visit(CreateStatement& node) = 0; // create 문
	virtual void visit(InsertStatement& node) = 0;  // insert 문

};

class AstPrinter : public AstVisitor {
	
	//1. Expression ==============================================================
	// 컬럼참조
	void visit(ColumnExpression& node) {
		std::cout << "[Column: " << node.column_name << "]";
	}

	// 리터럴
	void visit(LiteralExpression& node) {
		std::cout << "[Literal: " << node.value << "]";
	}

	// 이항연산
	void visit(BinaryExpression& node) {
	
		std::cout << "(";
		node.left->accept(*this); // 좌항 방문 -> 좌항이 리터럴이거나 컬럼이면 위의 printer 방문
		std::cout << " " << node.op << " ";
		node.right->accept(*this); // 우항 방문 
		std::cout << ")";

	};  

	// 단항연산
	void visit(UnaryExpression& node) {
		std::cout << "(";
		std::cout << node.op;
		node.operand->accept(*this);
		std::cout << ")";
	}

	//2. Statement
	// select 문
	void visit(SelectStatement& node) {
		std::cout << "SELECT" << std::endl;
		for (std::string column : node.select_list) {
			std::cout << "	"<< column << std::endl;
		}
		std::cout << "FROM" << std::endl;
		std::cout << "	" << node.from << std::endl;
		if(node.where!=nullptr) node.where->accept(*this);
		

	}; 
	// create 문
	void visit(CreateStatement& node) {
		std::cout << "CREATE TABLE " << node.table << std::endl;
		std::cout << "{" << std::endl;
		for (ColumnDef column : node.columns) {
			std::cout << column.name <<"	" << dataTypeToString(column.type) << std::endl;
		}
		std::cout << "}";
		
	
	}; // insert 문
	void visit(InsertStatement& node) {
		std::cout << "INSERT INTO " << node.table << std::endl;
		std::cout << "{" << std::endl;
		for (std::string value : node.values) {
			std::cout<<"	"<< value << std::endl;
		}
		std::cout << "}";
		

	};  
};