#include "Table.h"
#include <iostream>
#include <variant>

//1. 생성자
Table::Table(std::string name, int columnCapacity) :
	name(name),
	columnCount(5),
	columnCapacity(columnCapacity),
	rowCount(0),
	rowCapacity(0),
	rows(nullptr)
{

	// 컬럼 배열 동적할당
	columns = new column[columnCapacity];

}

// 2. 소멸자
Table::~Table() {

	if(rows != nullptr){  // 행 배열이 존재할 때
		
		for (int i = 0 ; i< rowCount; i++){
			delete[] rows[i].values; // 각 행 내부의 배열 해제
		}
		
		// 행 배열 해제
		delete[] rows;
	}
	

	// 컬럼 배열 해제
	delete[] columns;

}


// 3. 행 추가
void Table::addRow(Value* newValues) {
	
	if (rowCount >= rowCapacity) {

		int newCapacity =(rowCapacity == 0 )? 1 : rowCapacity * 2;
		
		Row* newRows = new Row[newCapacity]; // 현재 개수보다 1개 더 큰 Row 배열을 새로 동적할당

		// 기존 행들을 새 배열로 복사
		for (int i = 0; i < rowCount; i++) {
			newRows[i] = rows[i];
		}

		//기존 행 배열 해제
		delete[] rows;

		
		rows = newRows; // 새 행 배열로 교체
		rowCapacity = newCapacity;
		
	}

	Row& row = rows[rowCount];
	
	// 새 행 추가
	// rows[rowCount].values = newValues -> newValues 배열이 사라지면 터짐.
	row.values = new Value[columnCount];
	for (int i = 0; i < columnCount; i++) {
		rows[rowCount].values[i] = newValues[i];
	}
	row.valueCount = columnCount;

	rowCount++;
	// 테이블 객체의 포인터를 업데이트 해줌. 

}


// 4. 조회
void Table::printTable() const {

	std::cout << name << " | ";
	//for (int i = 0; i < columnCount; i++) {
	//	std::cout << columns[i].name << " " << std::endl;
	//}

	std::cout << "----------------------------------" << std::endl;
	
	for (int i = 0; i < rowCount; i++){
	
		for (int j = 0; j < rows[i].valueCount; j++) {

			std::visit([](auto&& arg) {
				std::cout << arg << "\t";
				}, rows[i].values[j]);			

			// std::visit : Value 타입이 std::variant로 되어있기 때문에 어떤 값이 있든
			// 해당 값을 출력할 수 있도록 처리.
			// [](auto&& arg) 람다 함수
			// rows[i].values[j] 값이 무엇이든 arg로 받아서 출력하도록 함.

		}
		std::cout << std::endl; //행이 끝날 때 마다 줄 바꿈.
			
	}
}

// 5. 컬럼 추가
void Table::addColumn(const column& newColumn) { 
	
	if (columnCount >= columnCapacity) {
		int newColumnCapacity = (columnCapacity == 0) ? 1 : columnCapacity * 2;

		column* newColumns = new column[newColumnCapacity];

		for (int i = 0; i < columnCount; i++) {
			newColumns[i] = columns[i];
		}

		delete[] columns;
		columns = newColumns;
		columnCapacity = newColumnCapacity;
	}
	columns[columnCount] = newColumn;
	columnCount++;

	//* c6385 경고는 무시할 것. 정적 분석도구문제. */

}


