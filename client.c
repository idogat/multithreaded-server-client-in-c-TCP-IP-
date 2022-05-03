#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512 // buffer size
#define DEFAULT_PORT "12345" //port numder

//functions prototypes
void startup();
void addrinfo_param(struct addrinfo* param);
void addrinfo(struct addrinfo* param, struct addrinfo* result);
void creat_socket(SOCKET* ListenSocket, struct addrinfo* param);
void connect_to_server(SOCKET ConnectSocket, struct addrinfo* ptr);
void send_and_receive(void);
void shutdown_socket(SOCKET ClientSocket);
{
    iResult = shutdown(ConnectSocket, SD_BOTH);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
    }
    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();
}

int main(int argc, char** argv)
{
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL, * ptr = NULL, hints;

    //buffer's variables
    char sendbuf[20];
    char recvbuf[DEFAULT_BUFLEN];

    //variables for general purposes
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Validate the parameters
    if (argc != 2)
    {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    WSADATA wsaData;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
    }

    //set addrinfo members
    addrinfo_param(&param);

    // Resolve the server address and port
    addrinfo(&param, &result);


    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {

        // Create a SOCKET for connecting to server
        creat_socket(SOCKET * ListenSocket, struct addrinfo* param)

        // Connect to server
        connect_to_server(SOCKET ConnectSocket, struct addrinfo* ptr)

    }


    // Receive & send until the peer closes the connection
    while (1)
    {
        send_and_receive(void)
        
    }
cleanup:
    // shutdown the connection since no more data will be sent
    shutdown_socket(SOCKET ClientSocket)
}

// Initialize Winsock
/*
void startup()
{
    WSADATA wsaData;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        goto cleanup;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        printf("Could not find a usable version of Winsock.dll\n");
        goto cleanup;
    }
    else
        printf("The Winsock 2.2 dll was found okay\n");
cleanup:
    WSACleanup();
}
*/
//set addrinfo members
void addrinfo_param(struct addrinfo* param)
{
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    *param = hints;
}

// Resolve the server address and port
void addrinfo(struct addrinfo* param, struct addrinfo* result)
{
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, *param, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
//        goto cleanup;
    }
//cleanup:
//    WSACleanup();
}

// Create a SOCKET for connecting to server
void creat_socket(SOCKET* ListenSocket, struct addrinfo* param)
{
    *ListenSocket = socket(param->ai_family, param->ai_socktype, param->ai_protocol);
    if (*ListenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        goto cleanup;
    }
    else
    {
        printf("Createing a socket..\n");
    }
cleanup:
    freeaddrinfo(param);
    WSACleanup();
}

// Connect to server
void connect_to_server(SOCKET ConnectSocket, struct addrinfo* ptr)
{
    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        continue;
    }
    else if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);
    break;
}

void send_and_receive(void)
{
    printf("enter message: ");
    fgets(sendbuf, 1024, stdin);

    //send data function
    iResult = send(ConnectSocket, sendbuf, sizeof(sendbuf), 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        goto cleanup;
    }
    //receive data function
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0)
    {
        printf("[from server]- %s\n", recvbuf);
    }
    else if (iResult == 0)
    {
        printf("Connection closed\n");
        goto cleanup;
    }
    else
    {
        printf("recv failed with error: %d\n", WSAGetLastError());
        goto cleanup;
    }
}

// shutdown the connection since no more data will be sent
void shutdown_socket(SOCKET ClientSocket)
{
    iResult = shutdown(ConnectSocket, SD_BOTH);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
    }
    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();
}