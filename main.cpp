#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <winsock2.h>
//#include "C:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h"
#include "mysql.h"
#include <iomanip>
#include <sstream>
#include <string.h>
#include "server.h"
#include "my_sql.h"

#pragma warning(disable:4996)
#pragma comment (lib, "Ws2_32.lib")

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <sys/wait.h>
#include <arpa/inet.h>
#endif

#define PORT 20000 
#define MY_QUEUE 5
#define MaxUserCount 10
#define MESSAGE_LENGTH 1024 
char len_message[MESSAGE_LENGTH];

struct sockaddr_in server, client;
socklen_t length;
extern MYSQL mysql;
#ifdef _WIN32
SOCKET connection;
int activity, sd, max_sd, max_clients = 5;
#else
int socket_file_descriptor, connection, bind_status, connection_status, activity, sd, max_sd, max_clients = 5;
int opt = 1;
#endif

std::vector <int>connections;
FD_SET readfds;

int main()
{
    setlocale(LC_ALL, "rus");
    CHAT_DB_connect();
    build_db();

    std::string login, password;  
    size_t choice = 1;
    bool ch(true);  
    bool n;    
    std::stringstream ss; 
    int ERR;
    std::map<int, std::string>UC;

#ifdef _WIN32

    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0)
    {
        std::cout << "Error WSAStartup" << std::endl;
        exit(1);
    }
    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    //addr.sin_addr.s_addr = inet_addr("127.0.0.1");   
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    addr.sin_family = AF_INET;
    SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
    listen(sListen, SOMAXCONN);

#else

    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_file_descriptor == -1)
    {
        cout << "Socket creation failed!" << endl;
        exit(1);
    }
    if (setsockopt(socket_file_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&opt,
        sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    
    serveraddress.sin_port = htons(PORT);
   
    serveraddress.sin_family = AF_INET;
   
    bind_status = bind(socket_file_descriptor, (struct sockaddr*)&serveraddress,
        sizeof(serveraddress));
    if (bind_status == -1)
    {
        cout << "Socket binding failed!" << endl;
        exit(1);
    }
    
    connection_status = listen(socket_file_descriptor, MY_QUEUE);
    if (connection_status == -1)
    {
        cout << "Socket is unable to listen for new connections!" << endl;
        exit(1);
    }
    else cout << "Server is listening for new connection: " << endl;
#endif 

    while (ch)
    {
        switch (choice)
        {
        case 1:

            FD_ZERO(&readfds);
#ifdef _WIN32
           
            FD_SET(sListen, &readfds);
            max_sd = sListen;
           
            for (int i = 0; i < connections.size(); i++)
            {
                
                sd = connections[i];

                if (sd > 0)
                    FD_SET(sd, &readfds);

                if (sd > max_sd)
                    max_sd = sd;
            }

#else

            FD_SET(socket_file_descriptor, &readfds);
            max_sd = socket_file_descriptor;

            for (int i = 0; i < connections.size(); i++)
            {
              
                sd = connections[i];

                if (sd > 0)
                    FD_SET(sd, &readfds);

                if (sd > max_sd)
                    max_sd = sd;
            }
#endif
#ifdef _WIN32

            activity = select(NULL, &readfds, NULL, NULL, NULL);
            if (activity == 0)
            {
                printf("Select() returned with error %d\n", WSAGetLastError());
                return 1;
            }
            else std::cout << "Select() is ok" << std::endl;

#else              
            activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
            {
                printf("Select error");
            }
#endif

#ifdef _WIN32
            
            SOCKET connection;
            if (FD_ISSET(sListen, &readfds))
            {
                connection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
                if (connection <= 0)
                {
                    perror("Accept");
                    exit(EXIT_FAILURE);
                }
                else std::cout << "Accept() is ok" << std::endl;

                printf("New connection , socket fd is %d , ip is : %s , port : %d \n", (int)connection, inet_ntoa(addr.sin_addr), ntohs
                (addr.sin_port));
                connections.push_back(connection);
            }
            else
            {
                for (int i = 0; i < connections.size(); i++)
                    my_receive(connections[i]);
                choice = 2;
                break;
            }

#else 

            if (FD_ISSET(socket_file_descriptor, &readfds))
            {
                if ((connection = accept(socket_file_descriptor,
                    (struct sockaddr*)&client, &length)) < 0)
                {
                    perror("Accept");
                    exit(EXIT_FAILURE);
                }
               
                printf("New connection , socket fd is %d , ip is : %s , port : %d \n", connection, inet_ntoa(serveraddress.sin_addr), ntohs
                (serveraddress.sin_port));
                connections.push_back(connection);
            }

            else
            {
                for (int i = 0; i < connections.size(); i++)
                    my_receive(connections[i]);
                choice = 2;
                break;
            }
#endif


            n = true;
            while (n)
            {
                login.clear(); password.clear();

                my_send(ss.str() + "Enter login: ", connection);
                ss.str("");
                while (login.empty())
                {
                    login = my_receive(connection);
                }


                my_send("Enter password:", connection);
                while (password.empty())
                {
                    password = my_receive(connection);
                }


                ERR = registration(login, password);
                if (ERR == 1)
                {
                    ss << "Error of insert into users: login wasn't UNIQUE\n";

                }
                else if (ERR == 2)
                {
                    ss << "Error of insert into HASH\n";

                }

                else
                {
                    UC.insert(make_pair(connection, login));
                    ss << get_users();
                    my_send(ss.str() + "Enter SPACE to continue \n", connection);
                    n = false;
                }
            }

            break;

        case 2:

            n = true;
            {
                int i = 0;
                while (n)
                {
                    while (i < connections.size())
                    {
                        connection = connections[i];
                        login.clear(); password.clear();
                        my_send(ss.str() + "Register new user. Enter login:", connection);
                        ss.str("");
                        while (login.empty())
                        {
                            login = my_receive(connection);
                        }

                        my_send("Enter password:", connection);
                        while (password.empty())
                        {
                            password = my_receive(connection);
                        }

                        ss << "\nHello, " + login + "\n";
                        if (i < connections.size() - 1)
                        {
                            my_send(ss.str() + "Please wait until all the users are authorized, ENTER SPACE\n", connection);
                            ss.str("");
                        }
                        else
                        {
                            my_send(ss.str() + "All the users are authorized, ENTER SPACE\n", connection);
                            ss.str("");
                            choice = 3;
                            n = 0;
                        }
                        my_receive(connection);
                        i++;
                    }
                }
            }
            break;

        case 3:
        {

            ss << "Your messages:\nid | sender | receiver | text | data):\n" << get_message(get_login(UC, connection));
            my_send(ss.str() + "Enter 1 to choose the login of your friend to text to\nEnter 2 to text to everyone\nEnter 3 to delete your account\nEnter # to quit chat\n", connection);
            ss.str("");
            std::string result;
            while (result == "")
            {
                result = my_receive(connection);
            }

            if (result == "1")
            {
                choice = 4; break;
            }
            else if (result == "2")
            {
                choice = 5; break;
            }
            else if (result == "3")
            {
                choice = 6; break;
            }
            if (len_message[0] == '#')
            {
                choice = 7; break;

            }
            else
            {
                ss << "Wrong sign, try again\n";
                result.clear();
            }
            break;
        }
        case 4:
        {
            std::string l;
            while (1)
            {
                my_send(ss.str() + "Enter the login of your friend", connection);
                ss.str("");
                l = my_receive(connection);
                if (check_login(l))
                    break;
                ss << "Unregistered user, try again\n";
            }

            my_send("text your message to " + l, connection);
            std::string m = my_receive(connection);

            ERR = store_message(get_login(UC, connection), l, m);
            if (ERR)
            {
                std::cout << "Error of store_message\n";
            }




            std::map<int, std::string >::iterator it = UC.find(get_con(UC, l));
            if (it == UC.end())
            {
                ss << "User is offline, message was stored\n";
                my_send(ss.str() + "\nENTER SPACE to continue", connection);
                my_receive(connection);
            }
            else
            {
                connection = it->first;
                my_send(m + "\nENTER SPACE to continue", connection);
                my_receive(connection);
            }

        }
        choice = 3;
        break;

        case 5:           

        {
            my_send("Text your message for everyone\n", connection);
            std::string m = my_receive(connection);
            store_message(get_login(UC, connection), "common chat", m);
            for (int i = 0; i < connections.size(); i++)
            {
                if (connections[i] == connection && i < connections.size() - 1)
                {
                    i++;
                    my_send(m + "\nENTER SPACE to continue", connections[i]);
                    my_receive(connections[i]);
                }
                else if (connections[i] != connection)
                {
                    my_send(m + "\nENTER SPACE to continue", connections[i]);
                    my_receive(connections[i]);
                }
                else break;
            }
        }

        choice = 3;
        break;

        case 6:
        {    delete_user(get_login(UC, connection));
        my_send("#Account has been deleted, connection has been stopped by server", connection);

        std::map<int, std::string >::iterator it;
        it = UC.find(connection);
        UC.erase(it);
        if (connections.size() == 1)
        {
            choice = 7; break;
        }
        for (int i = 0; i < connections.size(); i++)
        {
            if (connections[i] == connection)
            {
                connections.erase(connections.begin() + i);
                connection = connections[0];
                choice = 3;
                break;
            }
        }

        }
        break;

        case 7:
#ifdef _WIN32  

            if (connections.size() == 1)
            {
                std::cout << "Server is to close" << std::endl;
                my_send("# BYE!", connection);
                closesocket(connection);
            }
            else
            {
                my_send("# BYE!", connection);
                for (int i = 0; i < connections.size(); i++)
                {

                    if (connections[i] == connection)
                    {
                        std::map<int, std::string >::iterator it;
                        it = UC.find(connection);
                        UC.erase(it);

                        closesocket(connection);
                        connections.erase(connections.begin() + i);
                        connection = connections[0];
                        choice = 3;
                        break;
                    }

                }
            }if (choice == 3) break;

            WSACleanup();
#else

            if (ch == false)
            {
                for (int i = 0; i < connections.size(); i++)
                {
                    if (connections[i] == connection) i++;
                    my_send("#", connections[i]);
                }
                std::cout << "Server is to close" << std::endl;
                for (int i = 0; i < connections.size(); i++)
                {
                    if (connections[i] == connection) i++;
                    close(connections[i]);
                }
            }

#endif
            ch = false;
            break;


        }

    }
    mysql_close(&mysql);
    return 0;
}
