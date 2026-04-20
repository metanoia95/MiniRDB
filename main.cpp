
#include <filesystem>
#include <iostream>
#include "App.h"
#include "ServerSocket.h"

int main()
{
    // DB 엔진
    App app;
    //app.runQueryListTest(); // 자동 테스트 실행
    app.runCliTest();

    //
    ServerSocket socket = ServerSocket("27015");

    if (socket.init()) {
        std::cout << "socket init 성공. Listen 상태 시작" << std::endl;
        socket.run();
    };


}
