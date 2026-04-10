#include "Table.h"
#include <iostream>
#include <variant>

//1. 생성자
Table::Table(std::string name
	, std::vector<Column> columns
	, std::vector<Row> rows) :
	name(name),
	columns(std::move(columns)),
	columnCount(columns.size()),
	rows(rows),
	rowCount(rows.size())
{}

// 2. 소멸자
Table::~Table() {}


// 3. 행 추가
void Table::addRow(Row& newRow) {
	rows.push_back(newRow);
	rowCount++;
}


// 4. 조회
void Table::printTable() const {

	std::cout << name << " | ";

	std::cout << "----------------------------------" << std::endl;
	
	for (int i = 0; i < rows.size(); i++){
		for (int j = 0; j < rows[i].values.size(); j++) {
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
void Table::addColumn(Column& newColumn) { 
	
	columns.push_back(std::move(newColumn));
	columnCount++;

}


