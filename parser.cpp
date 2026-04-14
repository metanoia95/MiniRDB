#include "parser.h"
#include <stdexcept> // 예외처리 객체
#include <iostream>
#include <string> // std::stoi : string->int
std::unique_ptr<Statement> Parser::parse() {

	// 재귀하강 파서.
	// parse 함수에서 statement 호출.
	if (match(TokenType::SELECT)) return selectStatement();
	else if (match(TokenType::CREATE)) return createStatement();
	else if (match(TokenType::INSERT)) return insertStatement();
	
	throw std::runtime_error("지원되지 않는 명령문");

}

// 1. SELECT문 파서
std::unique_ptr<Statement> Parser::selectStatement() {
	std::cout << "SELECT 문 파싱" <<std::endl;

	// 0. 객체 선언
	auto stmt = std::make_unique<SelectStatement>();


	// 1. SELECT * OR COL

	while (!check(TokenType::FROM) && !isAtEnd()) { // FROM -> BREAK;
		
		// SELECT, COMMA -> IDENTIFIER, ASTERISK
		if (match(TokenType::ASTERISK) || match(TokenType::IDENTIFIER)){
			stmt->select_list.push_back(previous().value);
		}
		else {
			throw std::runtime_error("Expected ASTERISK or IDENTIFIER");
		}

		// IDENTIFIER, ASTERISK -> COMMA OR FROM
		if (match(TokenType::COMMA)) {
			continue; // 다음 구문으로 점프. 
		}

		if (check(TokenType::FROM)) {
			break;
		}

		throw std::runtime_error("UnexpectedToken: " + peek().value);
	}

	// 리스트가 비어있는지 여부 검사.
	if (stmt->select_list.empty()) { 
		throw std::runtime_error("Expected ASTERISK or IDENTIFIER after SELECT");
	}

	consume(TokenType::FROM, "Expected FROM");
	// 2. FROM절
	stmt-> from = consume(TokenType::IDENTIFIER, "Expected IDENTIFIER").value;
	// TODO 나중에 alias나 다른 요소도 처리

	//3. WHERE절 
	
	if (match(TokenType::WHERE)) {
		stmt-> where = parseExpression();
	}


	return stmt;
}

// 2. Create문 파서
std::unique_ptr<Statement> Parser::createStatement() {
	std::cout << "CREATE 문 파싱" << std::endl;

	// 0. 객체 선언
	auto stmt = std::make_unique<CreateStatement>();

	
	consume(TokenType::TABLE, "Syntax error, expected TABLE");

	
	stmt->table = consume(TokenType::IDENTIFIER, "Syntax error, expected IDENTIFIER").value;
	consume(TokenType::LPAREN, "expected LPAREN");

	do {
		Column col;
		QueryToken t1 = consume(TokenType::IDENTIFIER, "expected IDENTIFIER");
		col.name = t1.value;
		
		QueryToken t2 = peek();
		switch (t2.type) {
			case TokenType::TEXT:
				col.type = DataType::TEXT;
				break;
			case TokenType::INT:
				col.type = DataType::INT;
				break;
			default:
				throw std::runtime_error("Unexpected Type");
		}
		advance();
		
		stmt->columns.push_back(std::move(col));

	} while (match(TokenType::COMMA));

	consume(TokenType::RPAREN, " ) 필요");

	return stmt;
}

// 3. Insert문 파서
std::unique_ptr<Statement> Parser::insertStatement() {
	std::cout << "INSERT 문 파싱" << std::endl;
	// 지원 구문 예시
	//"INSERT INTO users VALUES (1, 'kim');",

	auto stmt = std::make_unique<InsertStatement>();
	consume(TokenType::INTO, "Syntax error, expected TABLE");
	stmt->table = consume(TokenType::IDENTIFIER, "테이블명이 필요합니다").value;
	//TODO 컬럼지정 -- 아직 안 만듬.
	consume(TokenType::VALUES, "VALUES 필요");
	consume(TokenType::LPAREN, "( 필요");
	
	do {
		QueryToken t = peek();

		switch (t.type) {
		case TokenType::STRING:
			stmt->values.push_back(t.value);
			break;
		case TokenType::NUMBER:
		{
			int intVal = std::stoi(t.value);
			stmt->values.push_back(intVal);
			break;
		}
		default:
			throw std::runtime_error("유효하지 않은 값");
		
		}
		advance();
	
	} while (match(TokenType::COMMA));

	consume(TokenType::RPAREN, ") 필요");


	return stmt;
}


// 이항식 파서.
ExprPtr Parser::parseExpression(){
	
	//WHERE 다음 토큰 부터 시작
	//  WHERE name = 'glory';"
	//  WHERE name >= 'glory';",
	// where절은 무조건 이항연산?
	// TODO : ||이나 and 연산은 나중에 추가

	ExprPtr leftExpr;
	ExprPtr rightExpr;
	BinaryOp op;

	
	//TODO AND문 재귀하강 처리
	while (!isAtEnd()) {

		// 1. 좌항
		leftExpr = parsePrimary();
		
		if (isAtEnd()) {
			throw std::runtime_error("Unexpected end or input");
		}

		// 2. 연산자
		QueryToken opToken = peek();
		switch (opToken.type) {
		case TokenType::EQUAL:
			op = BinaryOp::EQUAL;
			break;
		case TokenType::NEQ:
			op = BinaryOp::NEQ;
			break;
		case TokenType::LT:
			op = BinaryOp::LT;
			break;
		case TokenType::LTE:
			op = BinaryOp::LTE;
			break;
		case TokenType::GT:
			op = BinaryOp::GT;
			break;
		case TokenType::GTE:
			op = BinaryOp::GTE;
			break;
		default :
			throw std::runtime_error("Expected OPERATOR");
		}
		advance();

		if (isAtEnd()) {
			throw std::runtime_error("Unexpected end or input");
		}
		
		//3. 우항
		rightExpr = parsePrimary();

		
		// TODO AND 여부 검사
		//if (check(TokenType::AND)) {

		//	continue;
		//}
		
		break;
	}
	
	auto exp = std::make_unique<BinaryExpression>(
		op,
		std::move(leftExpr), // ! 유니크포인터 객체는 반드시 std::move로
		std::move(rightExpr)
	);
		
	return exp;
}


ExprPtr Parser::parsePrimary() {

	if (isAtEnd()) {
		throw std::runtime_error("Unexpected end or input");
	}

	QueryToken token = peek();
	switch (token.type) {
	case TokenType::IDENTIFIER:
		advance();
		return std::make_unique<ColumnExpression>(token.value);
	case TokenType::STRING:
		advance();
		return std::make_unique<LiteralExpression>(DataType::TEXT, token.value);
	case TokenType::NUMBER:
		advance();
		return std::make_unique<LiteralExpression>(DataType::INT, std::stoi(token.value));
	default:
		throw std::runtime_error("Expected IDENTIFIER or LITERAL");
	}

	
}