#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

std::vector<std::string> students = {
    "Williams 101 500 90 85 75",
    "Sanders 102 600 80 70 95",
    "Sanchez 101 550 85 80 88",
    "King 103 520 78 92 87",
    "Smith 102 480 65 75 80",
    "McKenzie 102 480 65 75 80",
    "Simon 102 480 65 75 80",
    "Brown 102 480 65 75 80",
    "Stevenson 102 480 65 75 80",
    "Ross 102 480 65 75 80",
    "May 102 480 65 75 80",
    "Gonzales 102 480 65 75 80",
    "Holmes 102 480 65 75 80",
    "James 102 480 65 75 80",
    "Price 102 480 65 75 80",
    "Martin 102 480 65 75 80",
    "Torres 102 480 65 75 80",
    "Thomas 102 480 65 75 80",
    "Bryant 102 480 65 75 80",
    "Sanchez 102 480 65 75 80",
    "Cox 102 480 65 75 80",
    "Smith 102 480 65 75 80",
    "Robinson 102 480 65 75 80",
    "Mason 102 480 65 75 80",
    "Gibson 102 480 65 75 80",
};

std::mutex mtx;

void HandleClient(SOCKET clientSocket) {
    char buffer[256];
    int bytesReceived;

    // Receive the requested letter from the client
    bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
        std::cerr << "Failed to receive data from the client" << std::endl;
        closesocket(clientSocket);
        return;
    }

    buffer[bytesReceived] = '\0';

    std::string requestedLetter(buffer);

    if (requestedLetter == "q") {
        // Если клиент отправил 'q', закрываем соединение
        std::cout << "Client closed the connection." << std::endl;
    }
    else {
        // Find students whose names start with the requested letter
        std::vector<std::string> result;
        for (const std::string& student : students) {
            if (student.find(requestedLetter) == 0) {
                result.push_back(student);
            }
        }

        // Send the result back to the client
        if (result.empty()) {
            // Send a special value indicating no students found
            send(clientSocket, "No students found", sizeof("No students found"), 0);
        }
        else {
            std::string response;
            for (const std::string& student : result) {
                response += student + "\n";
            }

            send(clientSocket, response.c_str(), response.size(), 0);
        }
    }

    // Shutdown перед закрытием соединения
    shutdown(clientSocket, SD_BOTH);
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize winsock" << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create server socket" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening on port 12345" << std::endl;

    // Цикл работы сервера
    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        std::thread(HandleClient, clientSocket).detach();
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
