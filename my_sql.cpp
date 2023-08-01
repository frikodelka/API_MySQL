#include <iostream>
//#include "C:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h"
#include <winsock2.h>
#include <mysql.h>
#include <sstream>
#include <iomanip>
#include "my_sql.h"
MYSQL mysql;
MYSQL_RES* res;
MYSQL_ROW row;

void CHAT_DB_connect()
{
	
	mysql_init(&mysql);
	if (&mysql == nullptr) 
	{
		
		std::cout << "Error: can't create MySQL-descriptor" << std::endl;
	}

	
	if (!mysql_real_connect(&mysql, "localhost", "root", "root", "testdb", NULL, NULL, 0)) 
	{
		
		std::cout << "Error: can't connect to database " << mysql_error(&mysql) << std::endl;
	}
	else 
	{
		
		std::cout << "MySQL connection is ok" << std::endl;
	}

	mysql_set_character_set(&mysql, "utf8");
	
	std::cout << "connection characterset: " << mysql_character_set_name(&mysql) << std::endl;
}

std::string query()
{
	std::stringstream ss;
	int i = 0;
	mysql_query(&mysql, "SELECT * FROM test"); 

	
	if (res = mysql_store_result(&mysql)) 
	{
		while (row = mysql_fetch_row(res)) 
		{
			for (i = 0; i < mysql_num_fields(res); i++) 
			{
				ss << row[i] << "  ";
			}
			ss << '\n';
		}
	}
	else
		std::cout << "Error MySql ¹ " << mysql_error(&mysql);
	return ss.str();

}
void build_db()
{
	mysql_query(&mysql, "CREATE DATABASE IF NOT EXISTS chat default charset cp1251");
	if (mysql_errno(&mysql))
		std::cout << "Error of CREATE\n";
	mysql_query(&mysql, "USE chat");
	if (mysql_errno(&mysql))
		std::cout << "Error of USE\n";
	mysql_query(&mysql, "CREATE TABLE IF NOT EXISTS users(user_id SERIAL NOT NULL AUTO_INCREMENT PRIMARY KEY, login varchar(255) UNIQUE)");
	if (mysql_errno(&mysql))
		std::cout << "Error of creating tabl USERS\n";
	mysql_query(&mysql, "CREATE TABLE IF NOT EXISTS hash(hash_id SERIAL NOT NULL AUTO_INCREMENT PRIMARY KEY, hash  varchar (40) NOT NULL, FOREIGN KEY (hash_id)  REFERENCES users (user_id) ON DELETE CASCADE)");
	if (mysql_errno(&mysql))
		std::cout << "Error of creating tabl HASH\n";
	mysql_query(&mysql, "CREATE TABLE IF NOT EXISTS message (message_id SERIAL NOT NULL AUTO_INCREMENT PRIMARY KEY, sender varchar (255), receiver varchar (255) , text TEXT (255), Timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
	if (mysql_errno(&mysql))
		std::cout << "Error of creating tabl MESSAGE\n";
	mysql_query(&mysql, "CREATE TRIGGER  hash_insert AFTER INSERT ON users FOR EACH ROW INSERT INTO hash (hash_id, hash) VALUES(new.user_id, 'empty')");
	if (mysql_errno(&mysql))
		std::cout << "Error of creating trigger hash_insert\n";

}


int registration(std::string login, std::string hash)
{
	std::string for_users = "INSERT users (login) VALUES('" + login + "')";
	mysql_query(&mysql, for_users.c_str());

	if (mysql_errno(&mysql))
		return 1;

	std::string for_update = "UPDATE hash SET hash = '" + hash + "' where hash_id = (SELECT MAX(user_id) from users)";
	mysql_query(&mysql, for_update.c_str());
	if (mysql_errno(&mysql))
		return 2;
	return 0;
}

std::string get_users()
{
	mysql_query(&mysql, "select login from users");
	int i; std::stringstream ss;
	ss << "REGISTERED USERS:\n";

	if (res = mysql_store_result(&mysql))
	{
		while (row = mysql_fetch_row(res))
		{
			for (i = 0; i < mysql_num_fields(res); i++)
			{
				ss << row[i] << "  ";
			}
			ss << std::endl;
		}
	}
	else std::cout << "Error MySQL registration1() " << mysql_error(&mysql);

	return ss.str();
}

bool check_login(std::string login)
{
	int i;

	std::stringstream ss;
	std::string for_name = "Select count(*) FROM users WHERE name ='" + login + "'";
	mysql_query(&mysql, for_name.c_str());
	if (res = mysql_store_result(&mysql)) 
	{
		while (row = mysql_fetch_row(res)) 
		{
			for (i = 0; i < mysql_num_fields(res); i++) 
			{
				ss << row[i];
			}
		}
	}
	else
		std::cout << "Îøèáêà MySql in check_login " << mysql_error(&mysql);
	return ss.str() == "1";
}

int store_message(std::string  sender, std::string receiver, std::string text)
{
	std::string str = "INSERT message (sender, receiver, text) VALUES('" + sender + "', '" + receiver + "', '" + text + "')";
	mysql_query(&mysql, str.c_str());

	if (mysql_errno(&mysql))
		return 1;
	return 0;

}

int  get_id(std::string login)
{
	std::stringstream ss;
	std::string str = "select user_id from users where login = '" + login + "'";
	mysql_query(&mysql, str.c_str());
	if (res = mysql_store_result(&mysql)) 
	{
		while (row = mysql_fetch_row(res)) 
		{
			for (int i = 0; i < mysql_num_fields(res); i++) 
			{
				ss << row[i];
			}
		}
	}
	else
		std::cout << "Îøèáêà MySql in get_id " << mysql_error(&mysql);
	return stoi(ss.str());
}

void delete_user(std::string login)
{
	std::string str = "UPDATE message SET sender = 'deleted' WHERE sender = '" + login + "'";
	mysql_query(&mysql, str.c_str());
	if (mysql_errno(&mysql))
		std::cout << "error 'sender' of delete_user";
	str = "UPDATE message SET receiver = 'deleted' WHERE receiver = '" + login + "'";
	mysql_query(&mysql, str.c_str());
	if (mysql_errno(&mysql))
		std::cout << "error 'receiver' of delete_user";
	str = "delete from users where login = '" + login + "'";
	mysql_query(&mysql, str.c_str());
	if (mysql_errno(&mysql))
		std::cout << "error of delete_user";
}

std::string get_message(std::string cur_user)
{
	std::stringstream ss;
	std::string str = "select * from message where sender = '" + cur_user + "'";
	mysql_query(&mysql, str.c_str());
	if (res = mysql_store_result(&mysql)) 
	{
		while (row = mysql_fetch_row(res)) 
		{
			for (int i = 0; i < mysql_num_fields(res); i++) 
			{
				ss << row[i] << " | ";
			}
			ss << std::endl;
		}
	}
	else
		std::cout << "ERROR of get_messages sender; " << mysql_error(&mysql);

	str = "select * from message where receiver = '" + cur_user + "'";
	mysql_query(&mysql, str.c_str());
	if (res = mysql_store_result(&mysql)) 
	{
		while (row = mysql_fetch_row(res)) 
		{
			for (int i = 0; i < mysql_num_fields(res); i++) {
				ss << row[i] << " | ";
			}ss << std::endl;
		}
	}
	else
		std::cout << "ERROR of get_messages receiver; " << mysql_error(&mysql);

	str = "select * from message where receiver = 'common chat'";
	mysql_query(&mysql, str.c_str());
	if (res = mysql_store_result(&mysql)) 
	{
		while (row = mysql_fetch_row(res)) 
		{
			for (int i = 0; i < mysql_num_fields(res); i++) {
				ss << row[i] << " | ";
			}ss << std::endl;
		}
	}
	else
		std::cout << "ERROR of get_messages common chat; " << mysql_error(&mysql);

	return ss.str();
}