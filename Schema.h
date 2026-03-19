#pragma once

#include "Table.h"


class Schema {

private:
	std::string name; // 데이터베이스 이름

	Table** tablePpArr; // 테이블 포인터 배열
	// 테이블 객체가 무겁기 때문에 포인터로 관리
	// Table** Table객체의 주소를 저장하고 있는 변수의 주소
	
	int tableCount; // 테이블 개수
	int tableCapacity; // 현재 할당된 메모리 공간의 크기

public :
	// 1. 생성자
	Schema(std::string name, int tableCount);

	// 2. 소멸자
	~Schema();


	// 3. 테이블 추가
	void addTable(Table* newTable);

	// 4. 데이터 확인용 출력 함수
	void printSchema() const;
	

	// 5. 복사 방지(공부용)
	Schema(const Schema&) = delete;
	Schema& operator = (const Schema&) = delete;

	//6. 테이블 검색
	Table* findTable(const std::string& name);



};
