#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char** argv) {
    WSADATA wsa;
    SOCKET s, cs;
    struct sockaddr_in server_addr, client_addr;
    unsigned short server_port = 4444;
    const char* server_ip;
    int client_addr_len;
    char message[2000];
    int r, exitCode = 1;

    server_ip = "192.168.1.68";

    //Display stuff
    printf("\t\tTCP SERVER\n\n");
    printf("Creating TCP echo server on the IP %s\n", server_ip);

    //Initialise WinSock
    printf("Initialising WinSock....\n");
    if ((r = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0) {
        printf("ERROR: Initialising failed with error Code : %d", r);
        exit(1);
    }
    printf("Successfully initialised.\n");

    //Create socket
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("ERROR: Socket creating failed with error code: %d", WSAGetLastError());
        goto cleanup;
    }
    printf("Socket created successfully.\n");

    //Setting the struct sockaddr_in server_addr
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    //Binding
    if (bind(s, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("ERROR: Could not bind to port with error code: %d", WSAGetLastError());
        goto cleanup;
    }
    printf("Bound to port %d\n", server_port);

    //Start listening for client
    printf("\nListening for client connection on %s:%d\n", server_ip, server_port);
    if (listen(s, 1) == SOCKET_ERROR) {
        printf("ERROR: Cannot listen for client connection with error code: %d", WSAGetLastError());
        goto cleanup;
    }

    //Accepting connection
    client_addr_len = sizeof(client_addr);
    if ((cs = accept(s, (struct sockaddr*)&client_addr, &client_addr_len)) == INVALID_SOCKET) {
        printf("ERROR: Cannot accept connection with error code: %d", WSAGetLastError());
        goto cleanup;
    }

    printf("Connection accepted from %s\n", inet_ntoa(client_addr.sin_addr));

    //No need of SOCKET s anymore
    closesocket(s);
    s = INVALID_SOCKET;
    while (true)
    {
        //Receive messages
        if ((r = recv(cs, message, 2000, 0)) == SOCKET_ERROR) {
            printf("ERROR: Cannot receive message with error code: %d", WSAGetLastError());
            goto cleanup;
        }

        if (r == 0) {
            printf("Client disconnected");
            goto cleanup;
        }

        printf("Message received: %.*s\n", r, message);
        std::cout << "Write back something: ";
        std::cin.getline(message, sizeof(message));
        //Send back the message
        strcpy_s(message, sizeof(message), message);
        if (send(cs, message, r, 0) == SOCKET_ERROR) {
            printf("ERROR: Cannot send message with error code: %d", WSAGetLastError());
            goto cleanup;
        }

        //printf("Message echoed back to client: %.*s\n", r, message);
    }
    exitCode = 0;

cleanup:

    //Close and clearing
    if (cs != INVALID_SOCKET) closesocket(cs);
    if (s != INVALID_SOCKET) closesocket(s);
    WSACleanup();

    return exitCode;
}