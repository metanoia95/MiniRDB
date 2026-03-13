#pragma once
#include <string>
#include <stack>
#include <vector>
#include "types.h"

enum class TokenType {

	KEYWORD, // 예약어 - SELECT, FROM, WHERE 등
	IDENTIFIER, // 식별자 - 테이블 이름, 컬럼 이름 등
	LITERAL, // 리터럴 값. 숫자, 문자열 등
	OPERATOR, //연산자
	SYMBOL //구분기호


};



struct QueryToken {

	TokenType type; //토큰 타입
	std::string value; // 토큰 값
};


class Tokenizer{

public :
	std::vector<QueryToken> tokenize(const std::string& query);

	std::string typeToString(TokenType c) {

		switch (c) {

		case TokenType::KEYWORD: return "KEYWORD";
		case TokenType::IDENTIFIER: return "IDENTIFIER";
		case TokenType::LITERAL: return "LITERAL";
		case TokenType::OPERATOR: return "OPERATOR";
		case TokenType::SYMBOL: return "SYMBOL";
		default: return "Unknown";

		}

	}





private :
	bool isOperator(char c);

	bool isKeyword(std::string word);
	
	bool isSymbol(char c);
};

