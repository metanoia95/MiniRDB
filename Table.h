#pragma once
#include "types.h"

class Table {


private :
	std::string name; // 테이블 이름

	column* columns; // 컬럼 배열 
	int columnCount; // 컬럼의 개수
	int columnCapacity; // 컬럼 메모리 공간 크기

	Row* rows; // 행 배열
	int rowCount;// 행 개수
	int rowCapacity; // 현재 할당된 메모리 공간의 크기

public:

	//1. 생성자
	Table(std::string name, int columnCount);

	//2. 소멸자
	~Table(); 
	

	//3. 행 추가
	void addRow(Value* values);

	//4. 데이터 확인용 출력 함수
	void printTable() const; // 읽기 전용 : 이 함수는 객체의 상태를 못 바꿈 

	//5. 복사 방지(공부용)
	// 포인터를 가진 클래스는 함부로 복사하면 터짐.
	// 생성자에서 명시를 안해도 복사 생성자와 복사 대입 연산자가 컴파일러에 의해 자동으로 생성됨.
	// 따라서 명시적으로 지워줘야함.
	Table(const Table&) = delete; // 복사 생성자 삭제 
	Table& operator = (const Table&) = delete; //복사 대입연산자 삭제

	// &는 참조자. Table 객체를 참조함. 


	//6. 테이블 이름 반환 함수
	std::string getName() const {
		return name;
	}

	//7. 컬럼 정보 반환 함수 
	int getColumnCount() const {
		return columnCount;
	}

	//8. 컬럼 추가
	void addColumn(const column& newColumn);




};