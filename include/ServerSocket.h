#pragma once
#include <iostream>

// 소켓 코드
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015" //클라이언트가 연결할 서버와 관련된 포트
#define DEFAULT_BUFLEN 512 //버퍼 길이

class ServerSocket {
private:


    void handleClient(SOCKET clientSocket) {};

    std::string readRequest(SOCKET clientSocket);

    //정리
    void cleanUp(SOCKET clientSocket);

    std::string port_;
    SOCKET listenSocket_;
    bool isRunning;

public:

    ServerSocket(const std::string& port): port_(port), listenSocket_(INVALID_SOCKET), isRunning(false) {};
    ~ServerSocket() {};

    bool init();

    void run(); // accept 루프 실행

    void stop(); // 서버 종료



};