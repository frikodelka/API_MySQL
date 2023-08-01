#pragma once

void CHAT_DB_connect();
std::string query();
void build_db();
void default_filling();
bool check_login(std::string login);
int registration(std::string login, std::string hash);
std::string get_name_from_DB(std::string login, std::string password);
std::string get_users();
int  get_id(std::string login);
int store_message(std::string sender_id, std::string receiver_id, std::string text);
std::string get_message(std::string cur_user);
void delete_user(std::string login);
