#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem> // c++17이상 필요
#include "Ast.h"
#include "Table.h"

class StorageManager {

public:

	StorageManager() {
		initDirectory();
	}

	// 테이블 데이터 파일 생성
	void createTableFile(const std::string& tableName, std::vector<Column>& columns);

	// 데이터를 파일 끝에 쓰고 데이터가 시작되는 파일 위치(offset)를 반환
	long appendRow(const std::string& tableName, const std::vector <Value>& rowData);

	// 인덱스가 알려준 위치(offset)로 가서 데이터 한 줄을 읽어옴.
	const Row readRow(const std::string& tableName, long offset);

	// 테이블 버퍼
	Table getTable(const std::string& tableName);
	

private:
	const std::string DATA_DIR = "db_data";

	// 테이블 생성
	void initDirectory();

	// 스키마 정보 조회
	std::vector<Column> getTableCatalog(const std::string& tableName);

	// 데이터 파일조회
	std::filesystem::path getDataFilePath(const std::string& tableName);


}; 