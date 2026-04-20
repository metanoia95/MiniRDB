#pragma once
#include <vector>
#include <thread>

// 레퍼런스: 모두의 코드 ThreadPool 참조. https://modoocode.com/285
class ThreadPool {

private:
	

	// 총 worker Thread 개수
	size_t num_threads_;

	// Worker 쓰레드를 보관하는 벡터
	std::vector<std::thread> worker_threads_;


};