#include "parser.h"
#include <stdexcept> // 예외처리 객체

std::unique_ptr<Statement> Parser::parse() {

	// 재귀하강 파서.
	// parse 함수에서 statement 호출.
	if (match(TokenType::SELECT)) return selectStatement();
	

}

std::unique_ptr<Statement> Parser::selectStatement() {
	
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

		throw std::runtime_error("UnexpectedToken: " + peek().value );
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


ExprPtr Parser::parseExpression(){
	
	//WHERE 다음 토큰 부터 시작
	//  WHERE name = 'glory';"
	//  WHERE name >= 'glory';",




}