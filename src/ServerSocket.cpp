#include "ServerSocket.h"
#include "ThreadPool.h"
#include "App.h"
#include <sstream>


bool ServerSocket::init() {
	
	// 1. 소켓 초기화
	WSADATA wsaData;

	int iResult;

    // 윈도우 소켓 DLL초기화. 시스템에서 WinSock 지원여부 확인
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData) ;
    if (iResult != 0) return false;
    
    // 2. 주소설정
    // getaddrinfo 함수 는 sockaddr 구조체 의 값을 확인하는 데 사용
    struct addrinfo* result = NULL, * ptr = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;  //IPv4 주소패밀리 지정
    hints.ai_socktype = SOCK_STREAM; // 스트림 소켓 지정
    hints.ai_protocol = IPPROTO_TCP; // TCP 프로토콜 지정
    hints.ai_flags = AI_PASSIVE; 
    // AI_PASSIVE : 나는 서버니까, 
    // getaddrinfo의 첫 번째 인자(node)를 NULL로 줄 테니 
    // '내 컴퓨터(호스트)의 모든 IP 주소'를 자동으로 바인딩할 수 있게 주소를 만들어줘"라는 뜻입니다. 
    // 덕분에 127.0.0.1뿐만 아니라 실제 할당된 IP로도 접속이 가능해집니다.

    // getaddrinfo 함수가 char*을 받는 관계로 c_str()처리
    iResult = getaddrinfo(NULL, port_.c_str(), &hints, &result);
    if (iResult != 0) { 
        std::cout << " 주소설정 실패 " << WSAGetLastError();
        WSACleanup();
        return false;
    };
    
    // 3. SOCKET 객체 생성
    listenSocket_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSocket_ == INVALID_SOCKET) {
        std::cout << " 바인딩 실패 " << WSAGetLastError();
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }
    
    // 4. 바인딩
    // sockaddr 구조체 는 주소 패밀리, IP 주소 및 포트 번호에 관한 정보를 저장
    // 서버가 클라이언트 연결을 수락하려면 시스템 내의 네트워크 주소와 바인딩 해야됨. 
    iResult = bind(listenSocket_, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cout << "소켓 바인딩 실패 :" << WSAGetLastError();
        freeaddrinfo(result);
        closesocket(listenSocket_);
        WSACleanup();
        return false;
    }
    // 사용이 끝난 주소정보는 즉시 해제
    freeaddrinfo(result);
    
    // 5. 수신대기열 확보
    if (listen(listenSocket_, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(listenSocket_);
        WSACleanup();
        return false;
    }


    this->isRunning = true;

    return true;
    
}

void ServerSocket::run() {

    // 데이터베이스 엔진 객체
    App app;

    while (isRunning) {


        // 소켓 연결 수락
        SOCKET ClientSocket = accept(listenSocket_, NULL, NULL);
        /*
            accept 동작 매커니즘.
            1. 대기 : listenSocket_(port)열고 대기
            2. 접속 요청 -> 내부적으로 새로운 소켓 객체 생성
            3. 연결 정보 4개 복사: 클라이언트 IP, Port / 서버 IP, Port
            4. 핸들 반환. 새로 만들어진 소켓의 번호를 리턴.
        */
        if (ClientSocket == INVALID_SOCKET) { // 연결 실패. TODO : 이 코드가 필요한지 고민해볼것.
            if (!isRunning) break; // 서버가 정지중인 것이면 종료 처리
            printf("accept failed: %d\n", WSAGetLastError()); // 소켓연결 실패 로깅 
            continue; // 다음 루프로 점프.
        }
        std::cout << "소켓 연결 : " << ClientSocket << std::endl;

        
        std::string request = readRequest(ClientSocket);
        // TODO 쿼리 객체일지 아니면 추가적인 할당이 필요한지 고민.
        std::cout << "리퀘스트 객체 :" << request << std::endl;

        //* 소켓 테스트 방법 (텔넷)
        /* 0. 제어판 -> 프로그램 및 기능 -> windows 기능 켜기 / 끄기 -> 텔넷 클라이언트 체크
           1. CMD -> telnet localhost 27015 #포트는 가변
           2. 쿼리 입력 후 요청여부 체크하기
           # CTRL+] -> set localecho 
           내가 친 명령어 보기
           */

        // TODO 
        // 리턴값 보내기 & 멀티스레딩 처리. 멀티 쓰레딩은 나중에 처리. 결과부터.
        int iSendResult;
        //std::string response = "리스폰스"; // TODO 쿼리 결과 리턴.
        std::string response = app.runQuery(request); // TODO 쿼리 결과 리턴.
        send(ClientSocket, response.c_str(), response.size(), 0);

        // 커넥션 종료
        cleanUp(ClientSocket);
        std::cout << "3. 클라이언트 정리 완료. 다음 루프로 이동" << std::endl;
    
    }


}

// 
void ServerSocket::stop() { // 전체 서버 종료

    // 1. 루프 정지 플래그
    isRunning = false;

    // 2. 리스닝 소켓 닫기
    if (listenSocket_ != INVALID_SOCKET) {
        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;
    }

    WSACleanup(); // WSACleanup(): 프로세스에서 돌아가는 모든 소켓 기능을 정지함.
    printf("서버 종료\n");
}

// 소켓 정리 함수
void ServerSocket::cleanUp(SOCKET ClientSocket) {
    int iResult;
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cout << "소켓 셧다운 실패 : " << WSAGetLastError() << std::endl;
    }
    closesocket(ClientSocket); // 소켓 자원 반환
    std::cout << "커넥션 종료 : " << ClientSocket << std::endl;
}

// 요청 리딩 함수 
std::string ServerSocket::readRequest(SOCKET clientSocket) {

    char recvbuf[DEFAULT_BUFLEN]; // 버퍼 준비
    int iResult, iSendResult;
    int recvbuflen = DEFAULT_BUFLEN;  // 버퍼 길이 512. 4096도 많이 사용함.
    std::stringstream ss;

    do {
        iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
        // ClientSocket에서 데이터 받아서 버퍼에 적재
        /*
        *   1. 클라이언트가 정보 송신
        *   2. OS커널의 수신 버퍼에 적재
        *   3. recv 함수 호출 -> 커널 버퍼 확인 -> recvbuf로 데이터 복사 -> 버퍼 포인터 이동.
        *   다음 recv 호출 시 OS가 그 다음 데이터부터 복사해줌.
        *
            recvbuf: 데이터를 담을 배열의 시작 주소.
            recvbuflen: 버퍼 크기 정보. 오버플로우 방지 및 특정 길이만큼 호출.
            iResult: 실제로 버퍼 담긴 데이터 크기
        */

        if (iResult > 0) {
            ss.write(recvbuf, iResult);
            // 방금 받은 데이터 뭉치의 가장 마지막 글자 확인
            if (recvbuf[iResult - 1] == ';') {
                // TODO. 아주 우연히도 패킷의 마지막 값이 ;일 가능성이 있는데
                // 어떻게 처리할지 고민할 것.
                break;
            }
        }
    } while (iResult > 0); //

    return ss.str();

}