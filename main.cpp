
#include <filesystem>
#include <iostream>
#include "App.h"
#include "ServerSocket.h"

int main()
{
    

#ifdef _DEBUG // 디버그 모드에서만 자동 테스트 실행
    App app;
    app.runQueryListTest(); // 쿼리 테스트 실행
    app.runCliTest(); // CLI 테스트 실행
#endif
    
    //
    ServerSocket socket = ServerSocket("27015");

    if (socket.init()) {
        std::cout << "socket init 성공. Listen 상태 시작" << std::endl;
        socket.run();
    };


}
