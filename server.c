#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "12345" //port numder

//global variables for general purposes


//buffer's variables
char recvbuf[DEFAULT_BUFLEN];
int recvbuflen = DEFAULT_BUFLEN;
char sendbuf[DEFAULT_BUFLEN];
int sendbuflen = DEFAULT_BUFLEN;

enum my_statuss
{
    SUCCESS = 0,
    FAILED = 1
};

//functions prototypes
DWORD WINAPI thread_to_connection(LPVOID lpParameter);
void addrinfo(struct addrinfo* param, struct addrinfo** result);
void creat_socket(SOCKET* ListenSocket, struct addrinfo* result);
void binding(SOCKET* ListenSocket, struct addrinfo* result);
void listen_func(SOCKET* ListenSocket);
SOCKET get_socket(SOCKET ListenSocket, SOCKET ClientSocket);
void shutdown_socket(SOCKET ClientSocket);
void addrinfo_param(struct addrinfo* param);
void memory_Allocation(struct addrinfo* param, struct addrinfo** result);

//thread's variables
DWORD dwThread, dwExitCode;
HANDLE hThread;

int main(void)
{


    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        WSACleanup();
    }
    else
        printf("The Winsock 2.2 dll was found okay\n");


    //allocating memory to struct addrinfo
    struct addrinfo* result;
    struct addrinfo param;
    memory_Allocation(&param, &result);

    //set addrinfo members
    addrinfo_param(&param);

    // Resolve the server address and port
    addrinfo(&param, &result);

    // Create a SOCKET for connecting to server
    SOCKET ClientSocket = INVALID_SOCKET;
    SOCKET ListenSocket = INVALID_SOCKET;
    creat_socket(&ListenSocket, result);

    // binding local address and port with a socket
    binding(&ListenSocket, result);

    //listen on IP address and port for incoming connection requests
    listen_func(&ListenSocket);

    while (1)
    {
        // (loop) Accept a client socket and pass the accepted client socket to a thread
        get_socket(ListenSocket, ClientSocket);
    }

    return 0;
}





//allocating memory to struct addrinfo
void memory_Allocation(struct addrinfo* param, struct addrinfo** result)
{
    enum my_status status = SUCCESS;
    *result = (struct addrinfo*)malloc(sizeof(struct addrinfo));
    if (NULL == *result)
    {
        printf("No memory was allocated to result");
        status = FAILED;
        goto cleanup;

    }
    param = (struct addrinfo*)malloc(sizeof(struct addrinfo));
    if (NULL == param)
    {
        printf("No memory was allocated to param");
        status = FAILED;
        goto cleanup;
    }
cleanup:
    if (FAILED == status)
    {
        free(param);
        free(*result);
    }
}

//set addrinfo members
void addrinfo_param(struct addrinfo* param)
{
    enum my_status status = SUCCESS;
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    *param = hints;
    if (NULL == param) 
    {
        status = FAILED;
            goto cleanup;
    }
cleanup:
    if (FAILED == status)
    {
        freeaddrinfo(param);
    }
}

// Resolve the server address and port
void addrinfo(struct addrinfo* param, struct addrinfo** result)

{
    enum my_status status = SUCCESS;
    int iResult = getaddrinfo(NULL, DEFAULT_PORT, param, result);
    if (iResult != 0)
    {
        printf("Getaddrinfo failed with error: %d\n", iResult);
        status = FAILED;
        goto cleanup;
    }
cleanup:
    if (FAILED == status)
    {
        freeaddrinfo(*result);
        freeaddrinfo(param);
    }
    
}

// Create a SOCKET for connecting to server
void creat_socket(SOCKET* ListenSocket, struct addrinfo* result)
{
    enum my_status status = SUCCESS;
    *ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (INVALID_SOCKET == *ListenSocket)
    {
        printf("Socket failed with error: %ld\n", WSAGetLastError());
        status = FAILED;
        goto cleanup;
    }
    else
    {
        printf("Createing a socket..\n");
    }
cleanup:
    if (FAILED == status) 
    {
        freeaddrinfo(result);
        closesocket(*ListenSocket);
    }
    
}

// binding local address and port with a socket
void binding(SOCKET* ListenSocket, struct addrinfo* result)

{
    enum my_status status = SUCCESS;
    int iResult = bind(*ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (SOCKET_ERROR == iResult)
    {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        status = FAILED;
        goto cleanup;
    }
    else
    {
        printf("Binding local address and port with a socket..\n");
        //done useing addrinfo Structure
        freeaddrinfo(result);
    }
cleanup:
    if (FAILED == status) 
    {
        closesocket(*ListenSocket);
        freeaddrinfo(result);
    }

}

//listen on IP address and port for incoming connection requests
void listen_func(SOCKET* ListenSocket)
{   
    enum my_status status = SUCCESS;
    int iResult = listen(*ListenSocket, SOMAXCONN);
    if (SOCKET_ERROR == iResult)
    {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        status = FAILED;
        goto cleanup;
    }
    else
    {
        printf("Listen on IP address and port for incoming connection requests..\n");
    }
cleanup:
    if (FAILED == status) 
    {
        closesocket(*ListenSocket);
    }
    
}

// (loop)Accept a client socket and pass the accepted client socket to a thread
SOCKET get_socket(SOCKET ListenSocket, SOCKET ClientSocket)
{   
    enum my_status status = SUCCESS;
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (INVALID_SOCKET == ClientSocket)
    {
        printf("Accept failed with error: %d\n", WSAGetLastError());
        status = FAILED;
        goto cleanup1;
    }
    hThread = CreateThread(0, 0, thread_to_connection, (LPVOID)ClientSocket, 0, &dwThread);

    if (INVALID_HANDLE_VALUE == hThread) 
    {
        printf("Thread not succeeded..");
        status = FAILED;
        goto cleanup2;
    }
    
cleanup2:
    if (FAILED == status) 
    {
        GetExitCodeThread(hThread, &dwExitCode);
        ExitThread(dwExitCode);
    }
cleanup1:
    if (FAILED == status) 
    {
        closesocket(ListenSocket);
        closesocket(ClientSocket);
    }
    if (SUCCESS == status) 
    {
        return ClientSocket;
    }
    return 0;
}


//Function to be executed by a thread
DWORD WINAPI thread_to_connection(LPVOID lpParameter)
{
    printf("New connection! Thread ID %d joined\n", GetCurrentThreadId());
    SOCKET ClientSocket = (SOCKET)lpParameter;
    while (1)
    {
        //The recv function receives data from a connected socket
        int iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            printf("From thread ID %d - %s", GetCurrentThreadId(), recvbuf);
        }
        else
        {
            int client_shutdown = WSAGetLastError();
            if (0 == client_shutdown)
            {
                printf("Thread id %d shutdown the connection..\n", GetCurrentThreadId());
                break;
            }
            else
            {
                printf("Recv failed with error: %d\n", WSAGetLastError());
                break;
            }
        }
        printf("Enter message to thread ID %d [type DONE to shutdown the connection]: ", GetCurrentThreadId());
        fgets(sendbuf, 1024, stdin);
        int shutdown;
        shutdown = strcmp(sendbuf, "DONE\n");
        if (0 == shutdown)
        {
            //shutdown the connection since no more data will be sent
            printf("shutdown the connection..\n");
            shutdown_socket(ClientSocket);
        }
        else
        {
            //The send function sends data on a connected socket
            int iSendResult = send(ClientSocket, sendbuf, sendbuflen, 0);
            if (SOCKET_ERROR == iSendResult)
            {
                printf("Send failed with error: %d to thread id %d\n", WSAGetLastError(), GetCurrentThreadId());
                break;
            }
        }
    }


    return 0;
}


// shutdown the connection since we're done
void shutdown_socket(SOCKET ClientSocket)
{
    int iResult = shutdown(ClientSocket, SD_BOTH);
    if (SOCKET_ERROR == iResult)
    {
        printf("Shutdown failed with error: %d\n", WSAGetLastError());
        exit(0);
    }
    closesocket(ClientSocket);
    WSACleanup();
    printf("Connection is down ..\n");
    exit(0);

}