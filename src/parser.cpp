#include "parser.h"
#include <stdexcept> // 예외처리 객체
#include <iostream>


StatusCode Parser::parse(std::unique_ptr<Statement>& outStmt) {

	// 재귀하강 파서.
	// parse 함수에서 statement 호출.
	if (match(TokenType::SELECT)) return selectStatement(outStmt);
	else if (match(TokenType::CREATE)) return createStatement(outStmt);
	else if (match(TokenType::INSERT)) return insertStatement(outStmt);
	
	setErrorData(StatusCode::NOT_SUPPORTED, "지원하지 않는 구문입니다");
	return StatusCode::NOT_SUPPORTED;

	//TODO 
	// 일일히 리턴처리 하지말고 Parser 클래스에 멤버변수 추가해서 파서 객체를 검사해서 처리. 함수 종료 후 플래그 검사

}

// 1. SELECT문 파서
StatusCode Parser::selectStatement(std::unique_ptr<Statement>& outStmt) {
	std::cout << "SELECT 문 파싱" <<std::endl;

	// 0. 객체 선언
	auto stmt = std::make_unique<SelectStatement>();

	// 1. SELECT * OR COL
	do {
		// SELECT, COMMA -> IDENTIFIER, ASTERISK
		if (match(TokenType::ASTERISK) || match(TokenType::IDENTIFIER)) {
			stmt->select_list.push_back(previous().value);
		}
		else {
			setErrorData(StatusCode::SYNTAX_ERROR, "Expected * or IDENTIFIER");
			return StatusCode::SYNTAX_ERROR;
		}
	} while (match(TokenType::COMMA));

	// 2. FROM절
	if (!consume(TokenType::FROM, "Expected FROM")) return StatusCode::SYNTAX_ERROR;

	if(!consume(TokenType::IDENTIFIER, "Expected IDENTIFIER")) return StatusCode::SYNTAX_ERROR;
	stmt->from = previous().value;
	// TODO 나중에 alias나 다른 요소, 여러 테이블 조회 처리.

	//3. WHERE절 
	if (match(TokenType::WHERE)) {
		ExprPtr wherePtr;
		if(parseExpression(wherePtr)!=StatusCode::OK) return StatusCode::SYNTAX_ERROR;
		stmt-> where = std::move(wherePtr);
	}

	outStmt = std::move(stmt);

	return StatusCode::OK;
}

// 2. Create문 파서
StatusCode Parser::createStatement(std::unique_ptr<Statement>& outStmt) {
	std::cout << "CREATE 문 파싱" << std::endl;

	// 0. 객체 선언
	auto stmt = std::make_unique<CreateStatement>();

	if(!consume(TokenType::TABLE, "Syntax error, expected TABLE")) 

	if(!consume(TokenType::IDENTIFIER, "Syntax error, expected IDENTIFIER")) return StatusCode::SYNTAX_ERROR;
	stmt->table = previous().value;
	if(!consume(TokenType::LPAREN, "expected LPAREN")) return StatusCode::SYNTAX_ERROR;

	do {
		Column col;
		if(!consume(TokenType::IDENTIFIER, "expected IDENTIFIER")) return StatusCode::SYNTAX_ERROR;
		QueryToken t1 = previous();
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
				return StatusCode::SYNTAX_ERROR;
		}
		advance();
		
		stmt->columns.push_back(std::move(col));

	} while (match(TokenType::COMMA));

	if (!consume(TokenType::RPAREN, "expected RPAREN")) return StatusCode::SYNTAX_ERROR;

	outStmt = std::move(stmt);

	return StatusCode::OK;
}

// 3. Insert문 파서
StatusCode Parser::insertStatement(std::unique_ptr<Statement>& outStmt) {
	std::cout << "INSERT 문 파싱" << std::endl;
	// 지원 구문 예시
	//"INSERT INTO users VALUES (1, 'kim');",

	auto stmt = std::make_unique<InsertStatement>();
	if (!consume(TokenType::INTO, "expected INTO")) return StatusCode::SYNTAX_ERROR;
	if (!consume(TokenType::IDENTIFIER, "테이블명이 필요합니다.")) return StatusCode::SYNTAX_ERROR;
	stmt->table = previous().value;
	//TODO 컬럼지정 추후 작성.
	if (!consume(TokenType::VALUES, "expected VALUES")) return StatusCode::SYNTAX_ERROR; // VALUES 
	if (!consume(TokenType::LPAREN, "expected LPAREN")) return StatusCode::SYNTAX_ERROR; // (
	
	do {
		QueryToken t = peek();

		switch (t.type) {
		case TokenType::STRING:
			stmt->values.push_back(t.value);
			break;
		case TokenType::NUMBER:
		{	
			int intVal; // stoi는 내부적으로 예외를 던져서 from_chars로 교체
			if(stringToInt(t.value, intVal)!= StatusCode::OK) return StatusCode::SYNTAX_ERROR;
			stmt->values.push_back(intVal);
			break;
		}
		default:
			return StatusCode::SYNTAX_ERROR;
		}
		advance();
	
	} while (match(TokenType::COMMA));

	if (!consume(TokenType::RPAREN, "expected RPAREN")) return StatusCode::SYNTAX_ERROR; // )

	outStmt = std::move(stmt);

	return StatusCode::OK;
}


// 이항식 파서.
StatusCode Parser::parseExpression(ExprPtr& outExprPtr){
	
	// WHERE 다음 토큰 부터 시작
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
		parsePrimary(leftExpr);
		
		
		if (isAtEnd()) {
			setErrorData(StatusCode::SYNTAX_ERROR, "Unexpected end or input");
			return StatusCode::SYNTAX_ERROR;
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
			setErrorData(StatusCode::SYNTAX_ERROR, "Expected OPERATOR");
			return StatusCode::SYNTAX_ERROR;
		}
		advance();

		if (isAtEnd()) {
			setErrorData(StatusCode::SYNTAX_ERROR, "Unexpected end or input");
			return StatusCode::SYNTAX_ERROR;
		}
		
		//3. 우항
		parsePrimary(rightExpr);

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
	
	outExprPtr = std::move(exp);

	return StatusCode::OK;
}


StatusCode Parser::parsePrimary(ExprPtr& outExprPtr) {

	if (isAtEnd()) {
		setErrorData(StatusCode::SYNTAX_ERROR, "Unexpected end or input");
		return StatusCode::SYNTAX_ERROR;
	}

	QueryToken token = peek();
	switch (token.type) {
	case TokenType::IDENTIFIER:
		advance();
		outExprPtr = std::make_unique<ColumnExpression>(std::move(token.value));
		return StatusCode::OK;
	case TokenType::STRING:
		advance();
		outExprPtr = std::make_unique<LiteralExpression>(DataType::TEXT, token.value);
		return StatusCode::OK;
	case TokenType::NUMBER:
		advance();
		int intVal;
		if (stringToInt(token.value, intVal) != StatusCode::OK) return StatusCode::SYNTAX_ERROR;
		outExprPtr = std::make_unique<LiteralExpression>(DataType::INT, intVal);
		return StatusCode::OK;
	default:
		setErrorData(StatusCode::SYNTAX_ERROR, "Expected IDENTIFIER or LITERAL");
		return StatusCode::SYNTAX_ERROR;
	}

	
}