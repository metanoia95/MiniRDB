#pragma once

enum class StatusCode : uint8_t {
	// uint8_t 1바이트 언사인드 정수

	// 정상
	OK = 0,


	// 파서 에러
	SYNTAX_ERROR, // 파서 에러
	NOT_SUPPORTED // 지원하지 않는 명령문


};