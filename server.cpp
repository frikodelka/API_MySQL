#include<iostream>
#include<string>
#include<string.h>
#include "server.h"
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#endif

#define MESSAGE_LENGTH 1024 
extern char ch_message[MESSAGE_LENGTH];
extern SOCKET connection;

bool compare_LP(std::string LP, std::string L, std::string P)
{
    return LP == L + P;
}

#ifdef _WIN32
void my_send(std::string _mes, int connection)
{
    size_t bytes;
    memset(ch_message, 0, MESSAGE_LENGTH);
    strncpy_s(ch_message, _mes.c_str(), MESSAGE_LENGTH);
    if (_mes.size() >= MESSAGE_LENGTH)
    {
        ch_message[MESSAGE_LENGTH - 1] = '\0';
    }
    std::cout << ch_message << std::endl;
    bytes = send(connection, ch_message, sizeof(ch_message), 0);
    if (bytes < 0) std::cout << "send failed" << std::endl;

}

std::string my_receive(int connection)
{
    memset(ch_message, 0, MESSAGE_LENGTH);
    size_t bytes = recv(connection, ch_message, MESSAGE_LENGTH, 0);
    if (bytes < 0)
    {
        perror("recv");
    }
    std::string s_read = std::string(ch_message, strlen(ch_message));
    std::cout << s_read << std::endl;
    return s_read;
}

#else

void my_send(std::string _mes, int connection)
{
    ssize_t bytes;
    bzero(ch_message, MESSAGE_LENGTH);
    strncpy(ch_message, _mes.c_str(), MESSAGE_LENGTH);
    if (_mes.size() >= MESSAGE_LENGTH)
    {
        ch_message[MESSAGE_LENGTH - 1] = '\0';
    }
    bytes = send(connection, ch_message, sizeof(ch_message), 0);
    if (bytes < 0) cout << "send failed" << endl;

}


std::string my_receive(int connection)
{
    bzero(ch_message, MESSAGE_LENGTH);
    ssize_t bytes = recv(connection, ch_message, MESSAGE_LENGTH, 0);
    if (bytes < 0)
    {
        perror("recv");
    }
    string s_read = string(ch_message, strlen(ch_message));
    std::cout << s_read << std::endl;
    return s_read;
}

#endif

std::string get_login(std::map<int, std::string>UC, int connection)
{
    std::string login;
    std::map<int, std::string >::iterator it;
    for (it = UC.begin(); it != UC.end(); ++it)
        if (it->first == connection)
            login = it->second;
    return login;
}


int get_con(std::map<int, std::string>UC, std::string login)
{
    int con;
    std::map<int, std::string >::iterator it;
    for (it = UC.begin(); it != UC.end(); ++it)
        if (it->second == login)
        {
            con = it->first;
            return con;
        }
    return 0;
}