
#include <filesystem>
#include <iostream>
#include "App.h"

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

int main()
{
    // DB 엔진
    App app;
    //app.runTest(); //  테스트 실행
    app.run();



    // 소켓
    WSADATA wsaData;

    int iResult;

    // 소켓 생성 체크
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup 실패 :" << iResult;
    }
    else {
        std::cout << "WSAStartup 성공 :" << iResult;
    }

    std::cout << std::endl;


    struct addrinfo* result = NULL, * ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;  //IPv4 주소패밀리 지정
    hints.ai_socktype = SOCK_STREAM; // 스트림 소켓 지정
    hints.ai_protocol = IPPROTO_TCP; // TCP 프로토콜 지정
    hints.ai_flags = AI_PASSIVE; // 호출자가 반환된 소켓주소 구조체를 

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo 실패 :" << iResult;
        WSACleanup();
    }
    else {
        std::cout << "getaddrinfo 성공 :" << iResult;
    }
    std::cout << std::endl;

    // SOCKET 객체 생성
    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        std::cout << "소켓 생성 실패 :" << WSAGetLastError();
        freeaddrinfo(result);
        WSACleanup();
    }
    else {
        std::cout << "소켓 생성 성공:" << ListenSocket;
    }
    std::cout << std::endl;

    // sockaddr 구조체 는 주소 패밀리, IP 주소 및 포트 번호에 관한 정보를 저장
    // 서버가 클라이언트 연결을 수락하려면 시스템 내의 네트워크 주소와 바인딩 해야됨. 
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cout << "소켓 바인딩 실패 :" << WSAGetLastError();
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
    } else {
        std::cout << "소켓 바인딩 성공 :" << iResult;
        // bind 함수가 호출 된 후에는 getaddrInfo함수가 반환하는 주소정보가 불필요.
        // 할당된 메모리 해제
        freeaddrinfo(result);
    }
    std::cout << std::endl;

    // 요청 수신 대기 하기
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    else {
        std::cout << "수신 대기 성공";
        // 만약 첫 생성인 경우 방화벽 허용 창이 뜸
    }

    // 소켓 연결요청 처리
    SOCKET ClientSocket;
    ClientSocket = INVALID_SOCKET;

    // 소켓 연결 수락
    ClientSocket = accept(ListenSocket, NULL, NULL);
    std::cout << ClientSocket;
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    // No longer need server socket
    closesocket(ListenSocket);


}
