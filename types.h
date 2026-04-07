#pragma once
#include <string>
#include <variant>


enum class DataType {
	INT,
	TEXT
};

//toString함수
inline std::string dataTypeToString(DataType type) {
	switch (type) {
	case DataType::INT:  return "INT";
	case DataType::TEXT: return "TEXT";
	default:             return "UNKNOWN";
	}
}

// 컬럼
struct column {

	std::string name;// 컬럼이름
	DataType type; // 컬럼 데이터 타입

};

// Value
// std::variant<int, std::string> 라는 타입을 Value라는 이름으로 사용
using Value = std::variant<int, std::string>; 


// Row
struct Row {

	Value* values; //컬럼 값들의 배열.
	// Value* values; // 컬럼 값들의 배열. Value 타입 객체를 가리키는 포인터 변수
	// Value 배열의 시작주소를 저장하는 포인터 변수.
	int valueCount; // 컬럼 값의 개수
	
};
