#pragma once
#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <sstream>

// 1. Value - 컬럼에서 가능한 데이터 타입 =======================================
// std::variant<int, std::string> 라는 타입을 Value라는 이름으로 사용
using Value = std::variant<int, std::string>;

#include "FileIoStrategy.h"

inline std::ostream& operator<<(std::ostream& os, const Value& v) {
	std::visit([&os](auto&& arg) {
		os << arg;
		}, v);
	return os;
}

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

//2. op - 연산자 타입 ====================================================
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


//헬퍼 함수 
// inline : 링커에게 "이 함수는 여러 번 중복되어도 괜찮으니 하나로 합쳐라
inline bool evaluateBinary(Value leftValue, BinaryOp op, Value rightValue) {
	switch (op) {
	case BinaryOp::EQUAL:  return leftValue == rightValue;
	case BinaryOp::NEQ: return leftValue!=rightValue;
	case BinaryOp::LT: return leftValue < rightValue;
	case BinaryOp::LTE: return leftValue <= rightValue;
	case BinaryOp::GT: return leftValue > rightValue;
	case BinaryOp::GTE: return leftValue >= rightValue;
	default:             return false;
	}

	return false;
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

// 3. 컬럼 ===================================================================
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



// 4. 행 ========================================================================
struct Row {

	std::vector<Value> values; //컬럼 값들의 배열.
	// Value* values; // 컬럼 값들의 배열. Value 타입 객체를 가리키는 포인터 변수
	// Value 배열의 시작주소를 저장하는 포인터 변수.
	//int valueCount; // 컬럼 값의 개수 * 벡터로 전환. 불필요

	void clear() {
		values.clear();
	}
	
};

// 5. 리턴 셋
struct ReturnSet {

	std::vector<std::string> columnNames;
	std::vector<DataType> columnTypes;

	std::vector<Row> rows;

	// json으로 출력
	std::string toJson() {
		// TODO : 일단 타입 무시하고 전부 쌍따옴표 붙여서 출력
		std::stringstream ss;

		ss << "{";
		
		// 컬럼명 표시
		ss << "\"columnNames\":["; // \" - 쌍따옴표 표시
		for (size_t i = 0; i < columnNames.size(); i++) {
			ss << "\"" << columnNames[i] << "\"" << (i == columnNames.size() - 1 ? "" : ",");
		}
		ss << "],";
		
		// rows 표시
		ss << "\"rows\":["; 
		for (size_t i = 0; i < rows.size(); i++) {
			ss << "[";
			for (size_t j = 0; j < rows[i].values.size(); j++) {
				ss << "\"" << rows[i].values[j] << "\"" << (j == rows[i].values.size() - 1 ? "" : ",");
			}
			ss << "]" << (i == rows.size()-1 ? "":",");
		}
		ss << "]";

		ss << "}";
		// 결과값.
		// {
		// columnNames : [ id, user ]
		// rows : [[1, alice], [2, brother]]
		// }

		return ss.str();
	}	

	void clear() {
		columnNames.clear();
		columnTypes.clear();
		rows.clear();
	
	}
};

