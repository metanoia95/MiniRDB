#pragma once
#include <vector>
#include <memory> // 유니크포인터
#include <charconv> 
#include "Tokenizer.h"
#include "Ast.h"  
#include "StatusCode.h"

class Parser {
public:

	//1. 생성자
	Parser(const std::vector<QueryToken>& tokens) 
		: tokens(tokens) 
		, hasError_(false)
		, errorCode_(StatusCode::OK)
		, errorMsg_("")
	{};

	//2. 파싱 함수
	StatusCode parse(std::unique_ptr<Statement>& outStmt);

	// TODO : 2026.04.20 - 예외처리를 전부 제거하고 예외코드를 뱉도록 수정할것.

	// 예외 코드 정보
	bool hasError_;
	StatusCode errorCode_;
	std::string errorMsg_;

private :

	StatusCode selectStatement(std::unique_ptr<Statement>& outStmt);

	StatusCode createStatement(std::unique_ptr<Statement>& outStmt);

	StatusCode insertStatement(std::unique_ptr<Statement>& outStmt);

	StatusCode parseExpression(ExprPtr& outExprPtr);

	StatusCode parsePrimary(ExprPtr& outExprPtr);


	//1. 멤버변수 ===============================================================
	const std::vector<QueryToken>& tokens; // 파싱할 토큰 배열
	size_t current = 0; // 현재 인덱스 위치. size_t : 배열과 vector의 인덱스 객체
	

	//2. 표준 파서 헬퍼 함수 ===============================================================
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

	// 2.4 현재 토큰이 특정 토큰 타입임을 기대하고, 없으면 에러 발생 및 함수 종료. 다음 전진.
	bool consume(TokenType type, std::string message) {
		if (hasError_) return false;
		if (check(type)) {
			current++; // 다음 전진
			return true;
		} 
		setErrorData(StatusCode::SYNTAX_ERROR, message);
		return false;
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

	// 3. 에러 처리 변수 세팅 함수 ================================================
	void setErrorData(StatusCode errorCode, const std::string& errorMsg) {
		hasError_ = true;
		errorCode_ = errorCode;
		errorMsg_ = errorMsg;
	}

	// 4. 문자열 -> 숫자 변환 함수. 예외 안 뱉음.
	/*
		int intVal; // stoi는 내부적으로 예외를 던져서 from_chars로 교체
			if(stringToInt(t.value, intVal)!= StatusCode::OK) return StatusCode::SYNTAX_ERROR;
			stmt->values.push_back(intVal);
	*/
	StatusCode stringToInt(const std::string& strVal, int& intVal) {

		const char* dataStart = strVal.data();
		const char* dataEnd = dataStart + strVal.size();
		auto [ptr, ec] = std::from_chars(dataStart, dataEnd, intVal);
		// ptr 변환이 멈춘 지점의 포인터
		// ec 에러코드 (성공시 0)

		// 1. 기본변환 성공여부 체크
		if (ec != std::errc()) {
			setErrorData(StatusCode::SYNTAX_ERROR, "not supported string to int");
			return StatusCode::SYNTAX_ERROR;
		}

		// 2. 찌꺼기 문자 처리
		// 숫자 문자열이 12345db로 들어오면 12345까찌만 읽고 db는 누락됨. 
		// 만약 숫자 문자열이 그렇게 온 경우 에러 처리
		if (ptr != dataEnd) { // 변환이 끝난지점의 포인터가 데이터의 끝 포인터와 다른 경우
			setErrorData(StatusCode::SYNTAX_ERROR, "숫자에 불필요 문자 포함");
			return StatusCode::SYNTAX_ERROR;
		}
		return StatusCode::OK;
	
	}


};
