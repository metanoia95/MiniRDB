#pragma once
#include <string>
#include <vector>

class StorageManager {

public :

	// 테이블 데이터 파일 생성
	void createTableFile(const std::string& tableName);

	// 데이터를 파일 끝에 쓰고 데이터가 시작되는 파일 위치(offset)를 반환
	long appendRow(const std::string& tableName, const std::vector <std::string>& rowData);

	// 인덱스가 알려준 위치(offset)로 가서 데이터 한 줄을 읽어옴.
	const std::vector <std::string> readRow(const std::string& tableName, long offset);

};