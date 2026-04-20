#pragma once
#include <vector>
#include <memory> // 유니크포인터
#include "Tokenizer.h"
#include "Ast.h"  
#include <stdexcept> // 예외처리 객체

class Parser {
public:

	//1. 생성자
	Parser(const std::vector<QueryToken>& tokens) : tokens(tokens) {}

	//2. 파싱 함수
	std::unique_ptr<Statement> parse(std::unique_ptr<Statement>& outStmt);

	// TODO : 2026.04.20 - 예외처리를 전부 제거하고 예외코드를 뱉도록 수정할것.


private :

	std::unique_ptr<Statement> selectStatement(std::unique_ptr<Statement>& outStmt);

	std::unique_ptr<Statement> createStatement(std::unique_ptr<Statement>& outStmt);

	std::unique_ptr<Statement> insertStatement(std::unique_ptr<Statement>& outStmt);

	ExprPtr parseExpression();

	ExprPtr parsePrimary();



	// 헬퍼 함수 ===============================================================
	//1. 멤버변수
	const std::vector<QueryToken>& tokens;

	size_t current = 0; // 현재 인덱스 위치. size_t : 배열과 vector의 인덱스 객체
	
	//2. 표준 파서 헬퍼 함수
	// 2.1 현재 토큰 반환
	const QueryToken& peek() const { return tokens[current]; }

	// 2.2 끝 확인
	bool isAtEnd() const { 
		return current >= tokens.size() || peek().type == TokenType::END_OF_FILE; 
	}

	// 2.3 현재 토큰이 특정타입인지 확인하고 맞으면 다음으로 전진
	bool match(TokenType type) {
		if ( isAtEnd() || !check(type)) return false; 
		current++;
		return true;
	}

	// 2.4 현재 토큰이 특정 토큰 타입임을 기대하고, 없으면 에러 발생. 다음 전진. 토큰 객체 리턴
	const QueryToken& consume(TokenType type, std::string message) {
		if (check(type)) return tokens[current++]; // 다음 토큰 넘어감.
		throw std::runtime_error(message);
	}

	// 2.5 방금 소비한 토큰을 반환.
	const QueryToken& previous() {
		if (current == 0) return tokens[0];
		return tokens[current - 1];
	}

	// 2.6 무조건 다음토큰 이동. 현재 토큰 리턴
	const QueryToken& advance() { 
		if (!isAtEnd())	return tokens[current++];
		else return tokens[current];
	}
	
	// 2.7 타입체크
	bool check(TokenType type) {
		if (peek().type == type) return true;
		return false;
	}

	// 2.8 다음 토큰 조회
	const QueryToken& peekNext() {
		if (!isAtEnd()) return tokens[current + 1];
		return tokens[current];
	}

	

};
