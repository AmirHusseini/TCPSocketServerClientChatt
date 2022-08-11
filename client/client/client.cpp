#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#include <iostream>
#include <string>

#pragma comment(lib,"ws2_32.lib")

int main(int argc, char** argv) {
    WSADATA wsa;
    SOCKET s = INVALID_SOCKET;
    struct sockaddr_in server_addr, client_addr;
    unsigned short server_port = 4444;
    const char* server_ip = "192.168.1.68";
    int client_addr_len;
    char message[2000];
    int r, exitCode = 1;

    system("cls");
    printf("\t\tTCP ECHO CLIENT\n\n");
    printf("Connecting to TCP echo server on the IP %s\n", server_ip);

    printf("Initialising WinSock....\n");
    if ((r = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0) {
        printf("ERROR: Initialising failed with error Code : %d", r);
        exit(1);
    }
    printf("Successfully initialised.\n");

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("ERROR: Socket creating failed with error code: %d", WSAGetLastError());
        goto cleanup;
    }
    printf("Socket created successfully.\n");

    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (connect(s, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("ERROR: Cannot connect to server %s", server_ip);
        goto cleanup;
    }
    printf("Connected to server %s on port %d\n", server_ip, server_port);

    do {

        std::cout << "Write something to the server: ";
        std::cin.getline(message, sizeof(message));
        strcpy_s(message, sizeof(message), message);
        if (send(s, message, strlen(message), 0) == SOCKET_ERROR) {
            printf("ERROR: Message sending failed with error code %d", WSAGetLastError());
            goto cleanup;
        }
        printf("Message sending was successful\n");

        if ((r = recv(s, message, sizeof(message), 0)) == SOCKET_ERROR) {
            printf("ERROR: Message receiving failed with error code %d", WSAGetLastError());
            goto cleanup;
        }

        if (r == 0) {
            printf("Server disconnected");
            goto cleanup;
        }

        printf("Message echoed back from server: %.*s\n", r, message);
        //getchar();
    } while (true);
    exitCode = 0;

cleanup:

    if (s != INVALID_SOCKET) closesocket(s);
    WSACleanup();

    return exitCode;
}