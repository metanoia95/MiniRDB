#pragma once
#include <string>
#include <variant>
#include <vector>
#include <memory>


// Value
// std::variant<int, std::string> 라는 타입을 Value라는 이름으로 사용
using Value = std::variant<int, std::string>;

#include "FileIoStrategy.h"

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

enum class BinaryOp {

	// 비교 연산자
	EQUAL, // =
	NEQ, // !=
	LT, // <
	LTE, // <=
	GT, // >
	GTE, // >=
	BANG, // !

	// 산술연산자
	PLUS, // +
	MINUS, // -
	ASTERISK, // *
	SLASH, // /

};

//toString함수
inline std::string binaryOpToString(BinaryOp type) {
	switch (type) {
	case BinaryOp::EQUAL:  return "=";
	case BinaryOp::NEQ: return "!=";
	case BinaryOp::LT: return "<";
	case BinaryOp::LTE: return "<=";
	case BinaryOp::GT: return ">";
	case BinaryOp::GTE: return ">=";
	case BinaryOp::BANG: return "!";
	case BinaryOp::PLUS: return "+";
	case BinaryOp::MINUS: return "-";
	case BinaryOp::ASTERISK: return "*";
	case BinaryOp::SLASH: return "/";
	default:             return "UNKNOWN";
	}
}

// 컬럼
struct Column {

	std::string name = "";// 컬럼이름
	DataType type = DataType::INT; // 컬럼 데이터 타입

	// 전략을 가리키는 포인터 
	std::unique_ptr<FileIOStrategy> fileIoStrategy;

	void setStrategy() {
		if (type == DataType::INT) {
			fileIoStrategy = std::make_unique<IntIoStrategy>();
		}
		else if (type == DataType::TEXT) {
			fileIoStrategy = std::make_unique<TextIoStrategy>();
		}
	}

	// 1. 기본 생성자를 명시적으로 추가
	Column() = default;
	// 이동 생성자/대입 연산자는 명시적으로 허용
	Column(Column&&) noexcept = default;
	Column& operator=(Column&&) noexcept = default;

	// 4. 복사 방지 (unique_ptr 때문에 필수)
	Column(const Column&) = delete;
	Column& operator=(const Column&) = delete;

};



// Row
struct Row {

	std::vector<Value> values; //컬럼 값들의 배열.
	// Value* values; // 컬럼 값들의 배열. Value 타입 객체를 가리키는 포인터 변수
	// Value 배열의 시작주소를 저장하는 포인터 변수.
	//int valueCount; // 컬럼 값의 개수 * 벡터로 전환. 불필요
	
};
