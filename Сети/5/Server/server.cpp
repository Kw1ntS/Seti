// Сервер

#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT 12345

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr, clientAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(DEFAULT_PORT);

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port " << DEFAULT_PORT << std::endl;

    char buffer[1024];
    int clientAddrSize = sizeof(clientAddr);

    while (true) {
        int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Recvfrom failed" << std::endl;
            break;
        }

        std::string receivedString(buffer, bytesReceived);
        std::cout << "Received from client: " << receivedString << std::endl;

        // Логика обработки строки (удаление чисел, делящихся на 3)
        std::string resultString;
        for (char c : receivedString) {
            if (!isdigit(c) || (isdigit(c) && (c - '0') % 3 != 0)) {
                resultString.push_back(c);
            }
        }

        // Отправка результата клиенту
        sendto(serverSocket, resultString.c_str(), resultString.length(), 0, reinterpret_cast<sockaddr*>(&clientAddr), sizeof(clientAddr));
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
