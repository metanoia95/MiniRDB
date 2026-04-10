#include "StorageManager.h"
#include <filesystem> // c++17이상 필요
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include "Table.h"


namespace fs = std::filesystem;

// 테이블 데이터 파일 생성
void StorageManager::createTableFile(
	const std::string& tableName
	, std::vector<Column>& columns
) {
	//경로 지정
	fs::path dataPath = getDataFilePath(tableName);
	fs::path catPath = fs::path(DATA_DIR) / (tableName + ".cat");

	// 테이블 데이터 파일 생성
	std::ofstream dataFile(dataPath, std::ios::binary| std::ios::out);
	if (!dataFile) return;
	dataFile.close();
	// std::ios::out - 파일을 출력용(쓰기용)으로 염
	// std::ios::binary - 파일을 바이너리 모드로 염.
	
	// 스키마 정보 저장용 카탈로그 파일 생성
	std::ofstream catFile(catPath, std::ios::binary | std::ios::out);
	if (!catFile) return;

	// 헤더 작성 : 
	// 컬럼 개수
	int colCount = static_cast<int>(columns.size());
	catFile.write(reinterpret_cast<char*>(&colCount), sizeof(int)); 
	// reinterpret_cast<char*> : 타입캐스트 연산자. 포인터를 다른 포인터 형식으로 변환
	// TODO : 왜 int를 char타입으로 변환하는지? 이해가 잘 안됨
	// int 데이터 타입을 1바이트* 4의 데이터 뭉치로 취급
	// colCount 값을 파일에 바이너리 형태로 그대로 저장.

	// 컴퓨터 입장에서 파일에 쓴다는 건 메모리에 있는 바이트들을 순서대로 하드디스크로 복사.
	// write 함수 : 특정 메모리 시작주소~거기서 부터 몇 바이트 읽을지 지정.
	// write함수의 공통 규격 -> char* (char*은 1바이트 주소 / int는 4바이트)
	// 무조건 char*을 받음.


	// 각 컬럼의 상세 정보 기록 
	for (const Column& col : columns) {

		// 컬럼 이름 길이 입력
		// : 컬럼 타입과 이름 정보를 구분하기 위해 필요
		int nameLen = static_cast<int>(col.name.size());
		catFile.write(reinterpret_cast<const char*>(&nameLen), sizeof(int));

		// 컬럼 이름 입력
		catFile.write(col.name.c_str(), nameLen);
		// c_str : std::string의 첫문자 포인터
		// 즉 col.name의 첫 문자부터 nameLen 길이만큼 입력

		// 컬럼 타입 입력
		// 컬럼 타입 : enum 객체
		int typeRaw = static_cast<int>(col.type);
		catFile.write(reinterpret_cast<const char*>(&typeRaw), sizeof(int));

	}
	
	catFile.close();

};

// 데이터를 파일 끝에 쓰고 데이터가 시작되는 파일 위치(offset)를 반환
long StorageManager::appendRow(const std::string& tableName, const std::vector <Value>& rowData) {

	fs::path dataPath = fs::path(DATA_DIR) / (tableName + ".dat");
	
	std::vector<Column> columns = getTableCatalog(tableName);

	//ofstream
	std::ofstream dataFile(dataPath, std::ios::binary | std::ios::app); // app : 이어쓰기
	if (!dataFile.is_open()) return -1;

	//offset 조회
	long offset = static_cast<long>(dataFile.tellp());

	//파일 입력
	for (size_t i = 0; i < columns.size(); i++) {
		columns[i].fileIoStrategy->write(dataFile, rowData[i]);
	}


	dataFile.close();

	return offset;
};

// 인덱스가 알려준 위치(offset)로 가서 데이터 한 줄을 읽어옴. -> TODO 나중에 페이징으로 바꾸기
const std::vector <std::string> StorageManager::readRow(const std::string& tableName, long offset) {

	std::vector<Column> columns = getTableCatalog(tableName);

	return {};
};

// 테이블 전체 조회 버퍼
Table StorageManager::getTable(const std::string& tableName) {
	
	std::vector<Column> columns = getTableCatalog(tableName);

	std::vector<Row> allRows;

	std::ifstream dataFile (getDataFilePath(tableName), std::ios::binary);

	while (dataFile.peek() != EOF) {
		
		Row newRow;
		std::vector<Value> row;
		for (const Column& col : columns) {
			col.fileIoStrategy->read(dataFile, row);
		}
		newRow.values = std::move(row);

		allRows.push_back(newRow);
	}
	
	Table table = Table(tableName, std::move(columns), allRows);

	return table;

}


//디렉토리 생성
void StorageManager::initDirectory() {
	

	if (!fs::exists(DATA_DIR)) {
		if (fs::create_directory(DATA_DIR)) {
			std::cout << "디렉토리 생성 : " << DATA_DIR << std::endl;
		}
		else {
			std::cout << "디렉토리 생성 실패 : " << DATA_DIR << std::endl;
		}


	}
}

//테이블 카탈로그 조회
std::vector<Column> StorageManager::getTableCatalog(const std::string& tableName) {
	
	// 스키마 객체
	std::vector<Column> columns;

	// 카탈로그 파일 경로
	fs::path catPath = fs::path(DATA_DIR) / (tableName + ".cat");

	// ifstream : input stream 파일 조회
	// fstream : 양방향
	// ofstream : 쓰기
	std::ifstream catFile(catPath, std::ios::binary);
	if (!catFile) return {};
	
	int colCount;
	catFile.read(reinterpret_cast<char*>(&colCount), sizeof(int));
	// read하게 되면 해당 포인터는 그 뒤로 점프함.

	std::cout << "컬럼 카운트: " << colCount << std::endl;
	
	for (int i = 0; i < colCount; i++) {
		Column col;

		int nameLength;
		// 이름 길이 읽기
		catFile.read(reinterpret_cast<char*>(&nameLength), sizeof(int));
		//std::cout << "컬럼 이름 길이: " << nameLength << std::endl;

		// 이름 읽기
		std::string colName(nameLength, '\0'); //초기화
		catFile.read(&colName[0], nameLength); //
		//std::cout << "컬럼명 :" << colName << std::endl;
		col.name = colName;

		// 타입 읽기
		int colType;
		catFile.read(reinterpret_cast<char*>(&colType), sizeof(int));
		//std::cout << "컬럼 타입 : " << colType << std::endl;
		col.type = static_cast<DataType>(colType);
		
		// 컬럼 정보 추가
		col.setStrategy(); //컬럼 전략 세팅
		columns.push_back(std::move(col));
		

		
	}
	
	catFile.close();

	return columns;

}

// 데이터 파일 경로 조회
fs::path StorageManager::getDataFilePath(const std::string& tableName) {
	fs::path dataPath = fs::path(DATA_DIR) / (tableName + ".dat");

	return dataPath;
}