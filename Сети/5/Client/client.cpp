// Клиент

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h> // Добавлен заголовочный файл для InetPton

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT 12345

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;

    // Заменяем inet_pton на InetPton
    if (InetPton(AF_INET, L"127.0.0.1", &serverAddr.sin_addr) != 1) {
        std::cerr << "InetPton failed" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    serverAddr.sin_port = htons(DEFAULT_PORT);

    std::string inputString;

    while (true) {
        std::cout << "Enter a string (press Enter to finish): ";
        std::getline(std::cin, inputString);

        if (inputString.empty()) {
            break;
        }

        sendto(clientSocket, inputString.c_str(), inputString.length(), 0, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

        char buffer[1024];
        int bytesReceived = recvfrom(clientSocket, buffer, sizeof(buffer), 0, nullptr, nullptr);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Recvfrom failed" << std::endl;
            break;
        }

        std::string resultString(buffer, bytesReceived);
        std::cout << "Received from server: " << resultString << std::endl;
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
