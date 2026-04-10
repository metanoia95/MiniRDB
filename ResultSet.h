#pragma once
#include <vector>
#include "types.h"

class ResultSet {

	// 컬럼
	std::vector<Column> columns;

	// 데이터 row
	std::vector<Row> rows;

	// 상태 정보
	int rowCount;

	// 소요시간
	double executionTime; 

	// 프린트 
	void print();

};