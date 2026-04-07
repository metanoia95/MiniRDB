#include "StorageManager.h"

// 테이블 데이터 파일 생성
void StorageManager::createTableFile(const std::string& tableName) {
};

// 데이터를 파일 끝에 쓰고 데이터가 시작되는 파일 위치(offset)를 반환
long StorageManager::appendRow(const std::string& tableName, const std::vector <std::string>& rowData) {


};

// 인덱스가 알려준 위치(offset)로 가서 데이터 한 줄을 읽어옴.
const std::vector <std::string> StorageManager::readRow(const std::string& tableName, long offset) {

};