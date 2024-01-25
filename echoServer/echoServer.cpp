/*
## 소켓 서버 : 1 v 1 - Single Process
1. socket()            : 소켓생성
2. bind()            : 소켓설정
3. listen()            : 수신대기열생성
4. accept()            : 연결대기
5. read()&write()
    WIN recv()&send    : 데이터 읽고쓰기
6. close()
    WIN closesocket    : 소켓종료
*/

#include "echoServer.h"

#include <deque>

bool WsaStart(WSAData& WSAData)
{
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		printf("Error - Can not load 'winsock.dll' file\n");
		return false;
	}

	return true;
}

bool getListenSocket(SOCKET& listenSocket)
{
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("Error - Invalid socket\n");
        return false;
	}
	return true;
}

SOCKADDR_IN getServerAddr(const int addrLen)
{
    SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, addrLen);
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    return serverAddr;
}

int socketError(const char* errorMsg, SOCKET listenSocket)
{
	printf(errorMsg);
	closesocket(listenSocket);
	WSACleanup();
	return 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
    std::cout << "server" << std::endl;
    // Winsock Start - windock.dll 로드
    WSAData WSAData;
    if (!WsaStart(WSAData)) return 0;

    // 1. 소켓생성
    SOCKET listenSocket;
    if (getListenSocket(listenSocket) == false) return 0;

    // 서버정보 객체설정
    // *** 이렇게 복사하는 방식도 사용하는지..? ***
    SOCKADDR_IN serverAddr = getServerAddr(sizeof(SOCKADDR_IN));

    // 2. bind
    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
        return socketError("Error - Fail to bind\n", listenSocket);
    }


    // 3. listen
    if (listen(listenSocket, 5) == SOCKET_ERROR)
    {
        return socketError("Error - Fail to listen\n", listenSocket);
    }

    // 4. accept (client addr 생성 -> accept)
    SOCKADDR_IN clientAddr;
    int addrLen = sizeof(clientAddr);
    memset(&clientAddr, 0, addrLen);
    SOCKET clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &addrLen);

    // 5. send recv
    while (true) {
        if (clientSocket != INVALID_SOCKET)
        {
            // 5-1. 데이터 읽기
            char messageBuffer[MAX_BUFFER];
            const int receiveBytes = recv(clientSocket, messageBuffer, MAX_BUFFER, 0);
            if (receiveBytes > 0)
            {
                std::cout << "TRACE - Receive message: " << messageBuffer << " (" << receiveBytes << " bytes)\n* Enter Message\n->";

            	// 5-2. 데이터 쓰기
                const int sendBytes = send(clientSocket, messageBuffer, strlen(messageBuffer), 0);
                if (sendBytes > 0)
                {
                    printf("TRACE - Send message : %s (%d bytes)\n", messageBuffer, sendBytes);
                }
            }
        }
    }

    // 6-1. 클라이언트 소켓종료
    closesocket(clientSocket);

    // 6-2. 리슨 소켓종료
    closesocket(listenSocket);

    // Winsock End
    WSACleanup();
    return 0;
}
