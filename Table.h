#pragma once
#include "types.h"
#include <vector>

class Table {


private :
	std::string name; // 테이블 이름

	std::vector<Column> columns; // 컬럼 배열 
	size_t columnCount; // 컬럼의 개수
	//int columnCapacity; // 컬럼 메모리 공간 크기 * 벡터로 전환하면서 불필요. raw한 배열타입 사용시 필요

	std::vector<Row> rows; // 행 배열
	size_t rowCount;// 행 개수
	//int rowCapacity; // 현재 할당된 메모리 공간의 크기 * 벡터로 전환하면서 불필요. raw한 배열타입 사용시 필요

public:

	//1. 생성자
	Table(std::string name, std::vector<Column> columns, std::vector<Row> rows);

	//2. 소멸자
	~Table(); 
	

	//3. 행 추가
	void addRow(Row& newRow);

	//4. 데이터 확인용 출력 함수
	void printTable() const; // 읽기 전용 : 이 함수는 객체의 상태를 못 바꿈 

	//5. 복사 방지(공부용)
	// 포인터를 가진 클래스는 함부로 복사하면 터짐.
	// 생성자에서 명시를 안해도 복사 생성자와 복사 대입 연산자가 컴파일러에 의해 자동으로 생성됨.
	// 따라서 명시적으로 지워줘야함.
	Table(const Table&) = delete; // 복사 생성자 삭제 
	Table& operator = (const Table&) = delete; //복사 대입연산자 삭제

	// &는 참조자. Table 객체를 참조함. 
	// 이동생성자 허용
	// [2] 이동 허용: 소유권 이전은 가능하게 함
	// noexcept를 붙여야 std::vector 등에서 안전하게 이동을 수행합니다.
	// 이동 생성자를 컴파일러가 자동으로 만들게 함
	Table(Table&& other) noexcept = default;
	Table& operator=(Table&& other) noexcept = default;



	//6. 테이블 이름 반환 함수
	std::string getName() const {
		return name;
	}

	//7. 컬럼 정보 반환 함수 
	size_t getColumnCount() const {
		return columnCount;
	}


	//8. 컬럼 추가
	void addColumn(Column& newColumn);

	// 인덱스로 컬럼명 조회
	std::string getColumnName(const size_t& idx) {
		return columns[idx].name;
	}

	//컬럼 명으로 인덱스 조회
	size_t getColumnIdx(std::string& name) {
		for (size_t i = 0; i < columns.size(); i++) {
			if (columns[i].name == name) {
				return i;
			}
		}
	}


	// 행 개수 조회
	size_t getRowCount() const {
		return rowCount;
	}
	



};