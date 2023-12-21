#include <iostream>
#include <winsock2.h>
#include <vector>
#include <thread>
#include <mutex>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")

std::mutex mutex;

std::vector<int> generateRandomNumbers(int N) {
    std::vector<int> randomNumbers;
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // Инициализация генератора случайных чисел
    for (int i = 0; i < N; ++i) {
        randomNumbers.push_back(std::rand() % N + 1);
    }
    return randomNumbers;
}

void handleClient(SOCKET clientSocket) {
    size_t N;
    recv(clientSocket, reinterpret_cast<char*>(&N), sizeof(N), 0);

    std::vector<int> randomNumbers = generateRandomNumbers(static_cast<int>(N));

    // Отправляем размер массива N клиенту
    send(clientSocket, reinterpret_cast<char*>(&N), sizeof(N), 0);

    // Отправляем массив случайных чисел клиенту
    send(clientSocket, reinterpret_cast<char*>(randomNumbers.data()), randomNumbers.size() * sizeof(int), 0);

    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize winsock." << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create server socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);  // Порт для прослушивания
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind server socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error in listen()." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port 12345..." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to accept client connection." << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        std::thread(handleClient, clientSocket).detach();
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
