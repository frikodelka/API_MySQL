#pragma once

#include <iostream>
#include <map>


bool compare_LP(std::string LP, std::string L, std::string P);
void my_send(std::string _mes, int connection);
std::string my_receive(int connection);
std::string get_login(std::map<int, std::string>UC, int connection);
int get_con(std::map<int, std::string>UC, std::string login);
