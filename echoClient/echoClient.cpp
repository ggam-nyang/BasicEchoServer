/*
## 소켓 클라이언트 : 1 v 1
1. socket()            : 소켓생성
2. connect()        : 연결요청
3. read()&write()
    WIN recv()&send    : 데이터 읽고쓰기
4. close()
    WIN closesocket    : 소켓종료
*/

#include <cstdio>
#include <iostream>
#include <string>
#include <Windows.h>
#include <tchar.h>

#pragma comment(lib, "Ws2_32.lib")

#define MAX_BUFFER        1024
#define SERVER_IP        "127.0.0.1"
#define SERVER_PORT        3500

int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "client" << std::endl;
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
    {
        std::cerr << "Error - Can not load 'winsock.dll' file\n";
        return 1;
    }

    // 1. listen socket 생성
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        std::cerr << "Error - Invalid socket\n";
        return 1;
    }

    // server addr 설정
    SOCKADDR_IN serverAddr;
    memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);

    // 2. connect
    if (connect(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Error - Fail to connect\n";
        // 4. 소켓종료
        closesocket(listenSocket);
        // Winsock End
        WSACleanup();
        return 1;
    }
    else
    {
        std::cout << "Server Connected\n* Enter Message\n->";
    }

    while (true)
    {
        // Input message using std::string
        std::string message;
        std::getline(std::cin, message);

        // Send message
        int sendBytes = send(listenSocket, message.c_str(), message.size() + 1, 0);  // Include null terminator
        if (sendBytes > 0) {
            std::cout << "TRACE - Send message: " << message << " (" << sendBytes << " bytes)\n";

            // Receive message
            char buffer[MAX_BUFFER];  // Keep a buffer for raw receive
            int receiveBytes = recv(listenSocket, buffer, MAX_BUFFER, 0);
            if (receiveBytes > 0) {
                std::string receivedMessage(buffer, receiveBytes);
                std::cout << "TRACE - Receive message: " << receivedMessage << " (" << receiveBytes << " bytes)\n* Enter Message\n->";
            }
        }
    }

    // 4. 소켓종료
    closesocket(listenSocket);

    // Winsock End
    WSACleanup();

    return 0;
}