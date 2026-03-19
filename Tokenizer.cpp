#include "Tokenizer.h"
#include <cctype>
#include <algorithm>

std::vector<QueryToken> Tokenizer::tokenize(const std::string& query) {

	//예시 쿼리
	/*

			"SELECT * FROM users;",
			"SELECT name, id FROM users;",
			"SELECT name, id FROM users WHERE name = 'glory';"
			"SELECT name, id FROM users WHERE name >= 'glory';",
			"CREATE TABLE users (id INT, name TEXT);",
			"INSERT INTO users VALUES (1, 'kim');",
			"INSERT INTO users VALUES (1, 'It''s a nice day');",
	*/

	/* cctype 함수
	* 함수	의미
		isalpha()	알파벳인지
		isdigit()	숫자인지
		isalnum()	알파벳 또는 숫자인지
		isspace()	공백인지
		islower()	소문자인지
		isupper()	대문자인지
	
	*/


	std::vector<QueryToken> tokens;
	// split을 사용하지 않고 문자단위로 읽음.
	for (int i = 0; i < query.size(); i++) {
	
		char c = query[i];
		
		if (isspace(c)) continue; //공백인 경우 
		


		if (c == '\'') { //시작 따옴표 ' 감지 
			std::string literal;
			i++; //시작 따옴표 다음칸으로 이동
			while (i < query.size()) {
				if (query[i] == '\'') {
					// 그 다음 글자도 따옴표인지 확인. 표준 SQL에서는 문자열 중간의 '는 ''로 치환되서 보내짐
					// ex It's a nice day -> 'It''s a nice day'
					if (i + 1 < query.size() && query[i + 1] == '\'') {
						
						literal += query[i];
						i += 2;
						continue;
					}
					else {
						i++;
						break;
					}					
				}

				literal += query[i];
				i++;
			}
			
			// TODO
			// '가 안닫혔을 때 로직 필요

			tokens.push_back({ TokenType::STRING, literal });

		}
		else if (isalpha(c)){ //알파벳인 경우
			
			std::string word;
			while (i < query.size() && isalnum(query[i])) { 
				word += query[i];
				i++;

			}
			i--;
			

			std::string upperWord = word;
			for (char& ch : upperWord) { //대문자 변환
				ch = std::toupper(static_cast<unsigned char>(ch));
			}

			auto it = keywords.find(upperWord);
			// unordered

			if (it != keywords.end()) { //.end() 못찾았다를 의미하는 기준값.
				tokens.push_back({ it->second , word });
			}
			else {
				tokens.push_back({ TokenType::IDENTIFIER, word });
			}
			
		}
		else if (isdigit(c)) { //숫자인 경우
			
			std::string num;
			while (i < query.size() && isdigit(query[i])) {
				num += query[i];
				i++;
			}
			i--;
			tokens.push_back({ TokenType::NUMBER, num });
		
		}
		else if (isOperator(c) || isSymbol(c)) { // 연산자, 기호인 경우

			switch (c) {
			case ',':
				tokens.push_back({ TokenType::COMMA, std::string(1,c) });
				break;
			case ';':
				tokens.push_back({ TokenType::SEMICOLON, std::string(1,c) });
				break;
			case '(':
				tokens.push_back({ TokenType::LPAREN, std::string(1,c) });
				break;
			case ')':
				tokens.push_back({ TokenType::RPAREN, std::string(1,c) });
				break;
			case '=':
				tokens.push_back({ TokenType::EQUAL, std::string(1,c) });
				break;
			case '+':
				tokens.push_back({ TokenType::PLUS, std::string(1,c) });
				break;
			case '-':
				tokens.push_back({ TokenType::MINUS, std::string(1,c) });
				break;
			case '*':
				tokens.push_back({ TokenType::ASTERISK, std::string(1,c) });
				break;
			case '/':
				tokens.push_back({ TokenType::SLASH, std::string(1,c) });
				break;
			case '<':
				if (i+1 <query.size() && query[i + 1] == '=') {
					tokens.push_back({ TokenType::LTE, std::string("<=") });
					i++;
					break;
				}
				tokens.push_back({ TokenType::LT, std::string(1,c) });
				break;
			case '>':
				if (i + 1 < query.size() && query[i + 1] == '=') {
					tokens.push_back({ TokenType::GTE, std::string(">=") });
					i++;
					break;
				}
				tokens.push_back({ TokenType::GT, std::string(1,c) });
				break;
			case '!':
				if (i + 1 < query.size() && query[i + 1] == '=') {
					tokens.push_back({ TokenType::GTE, std::string("!=") });
					i++;
					break;
				}
				tokens.push_back({ TokenType::BANG, std::string(1,c) });
				break;
			}

		}
		else {
			tokens.push_back({TokenType::UNKNOWN, std::string(1,c)});
		}
		
	
	}

	// 쿼리 끝
	tokens.push_back({ TokenType::END_OF_FILE, "" });
	

	return tokens;
}


bool Tokenizer::isOperator(char c) {
	return c == '=' || c == '+' || c == '-' || c == '<' || 
		c == '>' || c == '*' || c == '/' || c == '!';

}

bool Tokenizer::isSymbol(char c) {
	return c == ';' || c == ',' || c == '(' || c == ')';

}