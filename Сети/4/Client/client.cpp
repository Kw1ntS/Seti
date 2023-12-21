#include <iostream>
#include <cstdlib>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void error(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        error("Error initializing Winsock");
    }

    const char* SERVER_IP = "127.0.0.1";
    const int PORT = 12345;

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        error("Error creating socket");
    }

    sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;

    if (inet_pton(AF_INET, SERVER_IP, &(serverAddress.sin_addr)) != 1) {
        error("Error converting IP address");
    }

    serverAddress.sin_port = htons(PORT);

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        error("Error connecting to server");
    }

    double num1, num2;
    char operation[2]; // Массив символов для операции

    std::cout << "Enter number 1: ";
    std::cin >> num1;

    std::cout << "Enter number 2: ";
    std::cin >> num2;

    std::cout << "Enter operation (+, -, *, /): ";
    std::cin >> operation;

    if (strlen(operation) != 1) {
        error("Invalid operation");
    }

    if (send(clientSocket, reinterpret_cast<char*>(&num1), sizeof(num1), 0) == SOCKET_ERROR) {
        error("Error sending number 1");
    }

    if (send(clientSocket, reinterpret_cast<char*>(&num2), sizeof(num2), 0) == SOCKET_ERROR) {
        error("Error sending number 2");
    }

    if (send(clientSocket, operation, strlen(operation), 0) == SOCKET_ERROR) {
        error("Error sending operation");
    }

    double result;
    if (recv(clientSocket, reinterpret_cast<char*>(&result), sizeof(result), 0) == SOCKET_ERROR) {
        error("Error receiving result");
    }

    std::cout << "Result: " << result << std::endl;

    closesocket(clientSocket);
    WSACleanup();

    system("pause");

    return 0;
}
