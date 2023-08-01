#include <iostream>
#include <string.h>
#include <string>
#include <cstring>
#include "sha1.h"

#pragma warning(disable: 4996)
#pragma comment (lib, "Ws2_32.lib")

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

#else
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#endif

#define MESSAGE_LENGTH 1024 
#define PORT 20000 

int socket_file_descriptor, connection;
struct sockaddr_in serveraddress, client;
char message[MESSAGE_LENGTH];

int main()
{

#ifdef _WIN32
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0)
    {
        std::cout << "Error" << std::endl;
        exit(1);
    }
    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(20000);
    addr.sin_family = AF_INET;

    SOCKET socket_file_descriptor = socket(AF_INET, SOCK_STREAM, NULL);
    if (connect(socket_file_descriptor, (SOCKADDR*)&addr, sizeof(addr)) != 0)
    {
        std::cout << "Error: failed connect to server.\n";
        return 1;
    }
    std::cout << "Connected\n";

#else

    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_file_descriptor == -1) 
    {
        cout << "Creation of Socket failed!" << endl;
        exit(1);
    }
    
    serveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    serveraddress.sin_port = htons(PORT);
    
    serveraddress.sin_family = AF_INET;
    
    connection = connect(socket_file_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress));
    if (connection == -1) 
    {
        cout << "Connection with the server failed!" << endl;
        exit(1);
    }

#endif

    while (1)
    {
        bool flag = false;

        memset(message, 0, MESSAGE_LENGTH);
        recv(socket_file_descriptor, message, MESSAGE_LENGTH, 0);
        std::cout << message << std::endl;
      
        if (!strcmp(message, "Enter PASSWORD"))
            flag = true;
        if ((strncmp(message, "#", 1)) == 0)
        {

            std::cout << "Client Exit." << std::endl;
#ifdef _WIN32
            closesocket(socket_file_descriptor);
#else
            
#endif
            return 0;
        }

        memset(message, 0, MESSAGE_LENGTH);
        std::string snd_message;
        while (snd_message.empty())                                                           
        {
            snd_message.clear();
            getline(std::cin, snd_message);
        }
        if (flag == true)
        {
            snd_message = get_hash(snd_message);
        }

        strncpy(message, snd_message.c_str(), MESSAGE_LENGTH);
        if (snd_message.size() >= MESSAGE_LENGTH)
        {
            message[MESSAGE_LENGTH - 1] = '\0';
        }

        if ((strncmp(message, "#", 1)) == 0)
        {
            send(socket_file_descriptor, message, MESSAGE_LENGTH, 0);
            std::cout << "Client Exit." << std::endl;
            break;
        }
        size_t bytes = send(socket_file_descriptor, message, MESSAGE_LENGTH, 0);
        if (bytes < 0)
            perror("send");

    }
   
#ifdef _WIN32
    closesocket(socket_file_descriptor);
    WSACleanup();
#else
    close(socket_file_descriptor);
#endif

    return 0;
}