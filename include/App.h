#pragma once // 헤더파일이 한 번만 포함되도록 하는 지시자
#include <string>
class App {

public:

	// 쿼리문 실행 함수
	std::string runQuery(std::string query);

	// 쿼리문 리스트 테스트
	void runQueryListTest();

	// 수동입력 테스트
	void runCliTest();


private :
	bool isRunning = true;

};