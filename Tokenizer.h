#pragma once
#include <string>
#include <stack>
#include <vector>
#include "types.h"
#include <unordered_map>

enum class TokenType {

	// 키워드 - SELECT, FROM, WHERE 등
	SELECT, CREATE, INSERT, TABLE, DELETE, FROM, WHERE, VALUES, INTO,

	IDENTIFIER, // 식별자 - 테이블 이름, 컬럼 이름 등
	LITERAL, NUMBER, STRING, // 리터럴 값. 숫자, 문자열 등

	OPERATOR, //연산자

	SYMBOL, //구분기호

	// 기호
	COMMA, // ,
	SEMICOLON, // ;
	LPAREN,  // (
	RPAREN,  // )

	// 연산자
	EQUAL, // =
	PLUS, // +
	MINUS, // -
	ASTERISK, // *
	SLASH, // /
	LT, // <
	GT, // >
	BANG, // !

	

	// 기타
	END_OF_FILE,
	UNKNOWN

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

		case TokenType::SELECT: return "SELECT";
		case TokenType::CREATE: return "CREATE";
		case TokenType::INSERT: return "INSERT";
		case TokenType::TABLE: return "TABLE";
		case TokenType::DELETE: return "DELETE";
		case TokenType::FROM: return "FROM";
		case TokenType::WHERE: return "WHERE";
		case TokenType::VALUES: return "VALUES";
		case TokenType::INTO: return "INTO";

			// 식별자 / 리터럴
		case TokenType::IDENTIFIER: return "IDENTIFIER";
		case TokenType::LITERAL: return "LITERAL";
		case TokenType::NUMBER: return "NUMBER";
		case TokenType::STRING: return "STRING";

			// 연산자 / 심볼
		case TokenType::OPERATOR: return "OPERATOR";
		case TokenType::SYMBOL: return "SYMBOL";

			// 기호
		case TokenType::COMMA: return "COMMA";
		case TokenType::SEMICOLON: return "SEMICOLON";
		case TokenType::LPAREN: return "LPAREN";
		case TokenType::RPAREN: return "RPAREN";

			// 연산자
		case TokenType::EQUAL: return "EQUAL";
		case TokenType::PLUS: return "PLUS";
		case TokenType::MINUS: return "MINUS";
		case TokenType::ASTERISK: return "ASTERISK";
		case TokenType::SLASH: return "SLASH";
		case TokenType::LT: return "LT";
		case TokenType::GT: return "GT";
		case TokenType::BANG: return "BANG";

			// 기타
		case TokenType::END_OF_FILE: return "END_OF_FILE";
		case TokenType::UNKNOWN: return "UNKNOWN";

		default:
			return "INVALID_TOKEN";
		

		}

	}

	std::unordered_map<std::string, TokenType> keywords = {


		// 키워드
		{"SELECT", TokenType::SELECT},
		{"CREATE", TokenType::CREATE},
		{"INSERT", TokenType::INSERT},
		{"TABLE", TokenType::TABLE},
		{"DELETE", TokenType::DELETE},
		{"FROM", TokenType::FROM},
		{"WHERE", TokenType::WHERE},
		{"VALUES", TokenType::VALUES},
		{"INTO", TokenType::INTO},
		

	};




private :
	bool isOperator(char c);

	bool isKeyword(std::string word);
	
	bool isSymbol(char c);
};

