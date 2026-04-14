#pragma once
#include "Ast.h"
#include "StorageManager.h"
#include <iostream>
#include <memory>
#include <cassert>

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


	// 상속 객체에서 오버라이딩할 함수
	//void visit(SelectStatement& node) {}
	//void visit(CreateStatement& node) {}
	//void visit(InsertStatement& node) {}


	//void visit(ColumnExpression& node) {}; // 컬럼참조
	//void visit(LiteralExpression& node) {}; // 리터럴 
	//void visit(BinaryExpression& node) {};  // 이항연산
	//void visit(UnaryExpression& node) {};  // 단항연산

};

// 프린터
class AstPrinter : public AstVisitor {
	
	//1. Expression ==============================================================
	// 컬럼참조
	void visit(ColumnExpression& node) {
		std::cout << "[Column: " << node.column_name << "]";
	}

	// 리터럴
	void visit(LiteralExpression& node) {
		std::cout << "[Literal: ";
		std::visit([](auto&& arg) {
			std::cout << arg;
			}, node.value);
		std::cout<< "]";
	}

	// 이항연산
	void visit(BinaryExpression& node) {
	
		std::cout << "(";
		node.left->accept(*this); // 좌항 방문 -> 좌항이 리터럴이거나 컬럼이면 위의 printer 방문
		std::cout << " " << binaryOpToString(node.op) << " ";
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
		for (const Column& column : node.columns) {
			std::cout << column.name <<"	" << dataTypeToString(column.type) << std::endl;
		}
		std::cout << "}";
		
	
	}; // insert 문
	void visit(InsertStatement& node) {
		std::cout << "INSERT INTO " << node.table << std::endl;
		std::cout << "{" << std::endl;
		for (const auto& value : node.values) {
			std::cout << "	";
			
			std::visit([](auto&& arg) {
				std::cout << arg;
				}, value);
			
			std::cout<< std::endl;
		}
		std::cout << "}";
		

	};  
};

// 실행기 
class AstExecutor : public AstVisitor {

public:
	// 스토리지 매니저.
	StorageManager manager;

	// 리턴 객체
	ReturnSet lastReturnSet;

	//Create
	void visit(CreateStatement& node) {

		manager.createTableFile(node.table, node.columns);

	
	}

	//Select 
	void visit(SelectStatement& node) {
		
		//TODO 일단 1개 테이블 조회만 하기
		// FROM 절 처리
		Table t = manager.getTable(node.from);
		
		//t.printTable();

		std::vector<Row> filteredRows;
		// WHERE절 처리
		if (node.where != nullptr) {
			BinaryExpression* binExpr = dynamic_cast<BinaryExpression*>(node.where.get());

			
			if (binExpr) { // where절이 이항연산인 경우
				ExprPtr& left = binExpr->left;
				ExprPtr& right = binExpr->right;
				BinaryOp& op = binExpr->op;
				int leftIdx = -1;
				Value rightValue;


				//좌항이 행인 경우
				if (left->expression_class == ExpressionClass::COLUMN_REF) {
					auto* col = static_cast<ColumnExpression*>(left.get());
					leftIdx = t.getColumnIdx(col->column_name);
				}

				assert(leftIdx != -1);

				//우항은 리터럴로 가정
				if (right->expression_class == ExpressionClass::LITERAL) {
					auto* col = static_cast<LiteralExpression*>(right.get());
					rightValue = col->value;
				}

				Row row;
				Value leftValue;
				//실행기이기 때문에 연산은 100% 유효하다고 가정
				for (int i = 0; i < t.getRowCount(); i++) {
					row = t.getRow(i);
					leftValue = row.values[leftIdx];
					if (evaluateBinary(leftValue, op, rightValue)) {					
						filteredRows.push_back(row);
					}
				}
			
			}
			
		}
		else {
			Row row;
			//실행기이기 때문에 연산은 100% 유효하다고 가정
			for (int i = 0; i < t.getRowCount(); i++) {
				row = t.getRow(i);
				filteredRows.push_back(row);

			}
		}

		// 리턴셋 clear();
		lastReturnSet.clear();

		// SELECT절 처리
		// 컬럼 인덱스 배열
		std::vector<size_t> columIdxs;
		for (const std::string& colName : node.select_list) {
			if (colName == "*") {
				for (size_t i = 0; i < t.getColumnCount(); i++) {
					columIdxs.push_back(i);
					lastReturnSet.columnNames.push_back(t.getColumnName(i));
					lastReturnSet.columnTypes.push_back(t.getColumnType(i));
				}
			}
			else {
				int idx = t.getColumnIdx(colName);
				if (idx != -1) {
					columIdxs.push_back(idx);
					lastReturnSet.columnNames.push_back(t.getColumnName(idx));
					lastReturnSet.columnTypes.push_back(t.getColumnType(idx));
				}
				else {
					//실행기 중단.
					//return;
				}
			
			}
		}
		// 인덱스 - 값 매핑
		std::vector<Row> resultRows;
		for (const Row& row : filteredRows) {
			Row projectedRow;
			for (size_t idx : columIdxs) {
				projectedRow.values.push_back(row.values[idx]);
			}
			resultRows.push_back(projectedRow);
		}
		

		//리턴셋에 추가
		lastReturnSet.rows = resultRows;


	}

	//Insert 
	void visit(InsertStatement& node) {

		long offset = manager.appendRow(node.table, node.values);

		//TODO 인덱스 생성 로직 offset으로 인덱스 삽입

	}

	// expression
	void visit(ColumnExpression& node) {}; // 컬럼참조
	void visit(LiteralExpression& node) {}; // 리터럴 
	void visit(BinaryExpression& node) {};  // 이항연산
	void visit(UnaryExpression& node) {};  // 단항연산


};