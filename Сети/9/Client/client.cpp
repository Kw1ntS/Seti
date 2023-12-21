#include <iostream>
#include <winsock2.h>
#include <vector>
#include <ws2tcpip.h>
#include <thread>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize winsock." << std::endl;
        return 1;
    }

    while (true) {
        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to create client socket." << std::endl;
            WSACleanup();
            return 1;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(12345);  // Порт сервера
        inet_pton(AF_INET, "127.0.0.1", &(serverAddr.sin_addr));  // IP адрес сервера

        if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Failed to connect to server." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        size_t N;
        std::cout << "Enter a positive integer N: ";
        std::cin >> N;

        // Отправляем размер массива N серверу
        send(clientSocket, reinterpret_cast<char*>(&N), sizeof(N), 0);

        // Получаем размер массива N от сервера
        recv(clientSocket, reinterpret_cast<char*>(&N), sizeof(N), 0);

        // Получаем массив случайных чисел от сервера
        std::vector<int> randomNumbers(N);
        recv(clientSocket, reinterpret_cast<char*>(randomNumbers.data()), N * sizeof(int), 0);

        std::cout << "Random numbers from the server:" << std::endl;
        for (int num : randomNumbers) {
            std::cout << num << " ";
        }
        std::cout << std::endl;

        closesocket(clientSocket);

        // Задержка перед следующей попыткой подключения
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    WSACleanup();

    return 0;
}
