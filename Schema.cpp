#include "Schema.h"
#include <iostream>


// 1. 생성자
Schema::Schema(std::string name, int tableCapacity) :
	name(name),
	tablePpArr(nullptr),
	tableCount(0),
	tableCapacity(tableCapacity)
{
	
	// 테이블 포인터 배열 동적할당
	tablePpArr = new Table*[tableCapacity]; //Table 객체의 주소를 저장하는 포인터 배열을 동적할당.

	// Table** -> Table* -> Table 객체

}

// 2. 소멸자
Schema::~Schema() {

	// 테이블 포인터 배열 해제
	for (int i = 0; i < tableCount; i++) {
		delete tablePpArr[i];// 각 테이블 객체  해제

	}

	delete[] tablePpArr;

}

// 3. 테이블 추가
void Schema::addTable(Table* newTable) {

	// 새로운 포인터 배열 동적할당
	Table** newTblPpArr = new Table* [tableCount + 1];

	// 기존 테이블 포인터 배열 복사
	for (int i = 0; i < tableCount; i++) {
		newTblPpArr[i] = tablePpArr[i];
	}

	// 새 테이블 추가
	newTblPpArr[tableCount] = newTable;

	// 기존 테이블 배열 해제
	delete[] tablePpArr;

	// 새 테이블 배열로 교체
	tablePpArr = newTblPpArr;
	
	// 테이블 개수 증가
	tableCount++;

}

// 4. 데이터 확인용 출력 함수
void Schema::printSchema() const {

	std::cout << "Table       /  column count" << std::endl;
	std::cout << "---------------------------" << std::endl;
	for (int i = 0; i < tableCount; i++) {
		std::cout << tablePpArr[i]->getName() << "     " << tablePpArr[i]->getColumnCount() << std::endl;
	
	}

}


Table* Schema::findTable(const std::string& name) {
	
	for (int i = 0; i < tableCount; i++) {
		if (tablePpArr[i]->getName() == name) {
			return tablePpArr[i]; // 테이블명이 일치하는 경우 해당 테이블 객체의 주소 리턴
		}
	}

	return nullptr; // 일치하는 테이블이 없는 경우 nullptr 리턴.
}
