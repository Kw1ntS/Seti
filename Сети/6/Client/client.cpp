#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize winsock" << std::endl;
        return 1;
    }

    while (true) {
        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to create client socket" << std::endl;
            WSACleanup();
            return 1;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(12345);
        inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

        if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Connection failed" << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Enter a letter (or 'q' to quit): ";
        char letter;
        std::cin >> letter;

        // Send the requested letter to the server
        if (send(clientSocket, &letter, sizeof(letter), 0) == SOCKET_ERROR) {
            std::cerr << "Failed to send data to the server" << std::endl;
            break;
        }

        // Check if the user entered 'q' to quit
        if (letter == 'q') {
            std::cout << "Exiting the client." << std::endl;
            break;
        }

        // Receive and print the result from the server
        char buffer[1024];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived == SOCKET_ERROR) {
            int errorCode = WSAGetLastError();
            if (errorCode == WSAECONNRESET) {
                std::cerr << "The connection was reset by the server" << std::endl;
            }
            else {
                std::cerr << "Failed to receive data from the server. Error code: " << errorCode << std::endl;
            }
            break;
        }
        else if (bytesReceived == 0) {
            // Server closed the connection
            std::cout << "Server closed the connection." << std::endl;
            break;
        }
        else {
            buffer[bytesReceived] = '\0';

            if (strcmp(buffer, "No students found") == 0) {
                std::cout << "No students found for the given letter." << std::endl;
            }
            else {
                std::cout << "Students whose names start with '" << letter << "':" << std::endl;
                std::cout << buffer << std::endl;
            }
        }

        closesocket(clientSocket);
    }

    WSACleanup();

    return 0;
}
