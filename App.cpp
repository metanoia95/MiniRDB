#include "App.h"
#include <iostream>
#include <string>
#include <vector>
#include "Schema.h"
#include "Tokenizer.h"
#include "parser.h"
#include "AstVisitor.h"

void App::run() {

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
		};

		std::string query;
		// "" :문자열, '' 문자


		
		for (std::string testQuery : testQuerys) {
			query = testQuery;

			// 토크나이저 검증 코드
			Tokenizer tokenizer;

			std::vector < QueryToken > tokens = tokenizer.tokenize(query);

			for (QueryToken token : tokens) {
				std::string type = tokenizer.typeToString(token.type);
				std::cout << '['<< type << ',' << token.value << ']';
			}
			std::cout<< std::endl;

			// 파서
			Parser parser(tokens);

			// 상태 객체.
			auto stmt = parser.parse();

			// 방문자 객체
			AstPrinter printer;

			// 이중 디스패치
			stmt->accept(printer);
			std::cout << std::endl;


			// 실행 방문자
			AstExecutor executor;
			stmt->accept(executor);

			std::string json = executor.lastReturnSet.toJson();
			std::cout << json << std::endl;

			
		}

		
		App::isRunning = false; // 테스트 후 종료
	
	}

}


void App::runTest() {

	std::cout << "testing..." << std::endl;

	Schema myDb("TestDB", 2); // 스키마 객체 사용

	// 테이블 생성 및 추가


	myDb.printSchema();// 스키마 정보 출력

	while (App::isRunning) {

		std::cout << std::endl;
		//명령어 출력
		std::string command;
		std::cout << "main command: " << std::endl;
		std::cin >> command;



		if(command == "exit"){
			App::isRunning = false; // 테스트 후 종료
		}
		else if (command == "create") {
			
			std::string name;
			std::cout << "table name: " << std::endl;
			std::cin >> name;
		}else if(command == "print_schema"){
			myDb.printSchema();// 스키마 정보 출력	

		}else if (command == "find_table"){
			std::string name;
			std::cout << "table name: " << std::endl;
			std::cin >> name;
			
			Table* table = myDb.findTable(name); // 테이블 검색

			if (table != nullptr) {
				
				bool isTableMenu = true;
				while (isTableMenu) {
					
					std::cout << "current table : " << table->getName() << std::endl;
					std::cout << "-------------------------------------" << std::endl;
					
					std::string command2;
					std::cout << "table command: " << std::endl;
					std::cin >> command2;
					
					if (command2 == "exit") {
						isTableMenu = false;
					}
					else if (command2 == "info") {
						table->printTable();
					}
					else if (command2 == "insert") {
						
						Value* newValues = new Value[5];
						for (int i = 0; i < 5; i++) {
							newValues[i] = i;
						
						}
						
						table->printTable();
					}
					
					;



				
				}
							
			}
			else if (table == nullptr) {
				std::cout << "not found" << std::endl;
			}


		}

	
		

	}

}