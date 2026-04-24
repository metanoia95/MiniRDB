#pragma once
#include <fstream>
#include <vector>
#include <string>
#include "types.h"

// 1. 전략 인터페이스
class FileIOStrategy {

public:
	virtual ~FileIOStrategy() = default;
	virtual void read(std::ifstream& file, std::vector<Value>& row) = 0;
	virtual void write(std::ofstream& file, const Value& value) = 0;

protected : //자식들만 사용할 헬퍼함수

	// 바이너리 읽기 함수
	template<typename T>
	void readBinary(std::ifstream& file, T& value) {
		file.read(reinterpret_cast<char*>(&value), sizeof(T));
	}

};


// 2.INT 입출력 전략
class IntIoStrategy : public FileIOStrategy {

public:
	void read(std::ifstream& file, std::vector<Value>& row) override {
		int num;
		readBinary(file, num);
		row.push_back(num);
	}

	void write(std::ofstream& file, const Value& value) {
		const int intVal = std::get<int>(value);
		file.write(reinterpret_cast<const char*>(&intVal), sizeof(int));
		
	}

};

// 3. TEXT 입출력 전략
class TextIoStrategy : public FileIOStrategy {

public:
	void read(std::ifstream& file, std::vector<Value>& row) override {
		int length;
		readBinary(file, length);

		std::string str(length, '\0');
		file.read(&str[0], length);
		row.push_back(str);
	}

	void write(std::ofstream& file, const Value& value){
		const std::string& str = std::get<std::string>(value);

		int valSize = static_cast<int>(str.size());
		file.write(reinterpret_cast<const char*>(&valSize), sizeof(int)); // 길이 입력
		file.write(str.c_str(), valSize); // 문자열 데이터 입력
	}

};