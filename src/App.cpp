#include "App.h"
#include <iostream>
#include <string>
#include <vector>
#include "Schema.h"
#include "Tokenizer.h"
#include "parser.h"
#include "AstVisitor.h"

// 메인 실행 코드 ===============================================
std::string App::runQuery(std::string query) {

	// 토크나이저 검증 코드
	Tokenizer tokenizer;

	std::vector < QueryToken > tokens = tokenizer.tokenize(query);

	//토큰 프린트문.
	tokenizer.printTokenizedQuery(tokens);

	// 파서
	Parser parser(tokens);

	// 상태 객체.
	std::unique_ptr<Statement> stmt;
	//auto stmt = parser.parse();
	parser.parse(stmt);

	// 추상구문트리 프린트
	AstPrinter printer;
	//stmt->accept(printer);

	std::cout << std::endl;


	// 실행기
	AstExecutor executor;
	stmt->accept(executor);

	std::string json = executor.lastReturnSet.toJson();

	return json;
}



// 테스트 코드들  =================================================
// 쿼리문 입력 후 테스트
void App::runQueryListTest() {

	while (App::isRunning) {

		std::vector<std::string> testQuerys = {
			//"SELECT * FROM users;",
			//"CREATE TABLE users (id INT, name TEXT);",
			//"INSERT INTO users VALUES (144, 'kimsuki');",
			//"INSERT INTO users VALUES (1667, 'It''s a good day to die');",
			//"SELECT name, id FROM users;",
			//"SELECT id FROM users;",
			//"SELECT name, id FROM users WHERE name = 'kimsuki';"
			"SELECT name, id FROM users WHERE id >= 5;",
			"SELECT name, id FROM users WHERE 5 > id;"
		};

		std::string query;
		// "" :문자열, '' 문자



		for (std::string testQuery : testQuerys) {

			std::string json = runQuery(testQuery);

			std::cout << json << std::endl;
		}


		App::isRunning = false; // 테스트 후 종료

	};

};

// 수동 쿼리 입력 테스트
void App::runCliTest() {

	while (App::isRunning) {

		std::cout << std::endl;
		//명령어 출력
		std::string query;
		std::cout << "쿼리 입력 : " << std::endl;

		std::getline(std::cin, query);

		if (query == "exit") {
			isRunning = false;
			break;
		}


		std::string json = runQuery(query);

		std::cout << json << std::endl;


	}
}