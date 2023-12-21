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

double performOperation(double num1, double num2, char operation) {
    switch (operation) {
    case '+':
        return num1 + num2;
    case '-':
        return num1 - num2;
    case '*':
        return num1 * num2;
    case '/':
        if (num2 != 0) {
            return num1 / num2;
        }
        else {
            error("Division by zero");
        }
    default:
        error("Invalid operation");
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        error("Error initializing Winsock");
    }

    const int PORT = 12345;

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        error("Error creating socket");
    }

    sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        error("Error binding socket");
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        error("Error listening for incoming connections");
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    while (true) {
        SOCKET clientSocket;
        sockaddr_in clientAddress;
        int clientAddressSize = sizeof(clientAddress);

        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
        if (clientSocket == INVALID_SOCKET) {
            error("Error accepting connection");
        }

        double num1, num2, result;
        char operation[2];

        // Принимаем числа и операцию от клиента
        if (recv(clientSocket, reinterpret_cast<char*>(&num1), sizeof(num1), 0) == SOCKET_ERROR) {
            error("Error receiving number 1");
        }

        if (recv(clientSocket, reinterpret_cast<char*>(&num2), sizeof(num2), 0) == SOCKET_ERROR) {
            error("Error receiving number 2");
        }

        if (recv(clientSocket, operation, sizeof(operation), 0) == SOCKET_ERROR) {
            error("Error receiving operation");
        }

        // Выполняем операцию
        result = performOperation(num1, num2, operation[0]); // Обратите внимание на [0]

        // Отправляем результат клиенту
        if (send(clientSocket, reinterpret_cast<char*>(&result), sizeof(result), 0) == SOCKET_ERROR) {
            error("Error sending result");
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
