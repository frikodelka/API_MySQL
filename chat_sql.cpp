#include <iostream>
#include <mysql.h>

using namespace std;

int main() {
	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;

	int i = 0;

	mysql_init(&mysql);
	if (&mysql == nullptr) {

		cout << "Error: can't create MySQL-descriptor" << endl;
	}

	if (!mysql_real_connect(&mysql, "localhost", "root", "8888Frikodel.ka8888", "testdb", NULL, NULL, 0)) {
		cout << "Error: can't connect to database " << mysql_error(&mysql) << endl;
	}

	mysql_query(&mysql, "CREATE TABLE testtable(id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255), email VARCHAR(255))");
	mysql_query(&mysql, "INSERT INTO testtable(id, name, email) values(default, 'Testerov Tester', 'test@test.ru')");
	mysql_query(&mysql, "INSERT INTO testtable(id, name, email) values(default, 'Viking Eric', 'Eric@north.se')");
	mysql_query(&mysql, "INSERT INTO testtable(id, name, email) values(default, 'Viking Olaf', 'Olaf@north.se')");
	mysql_query(&mysql, "INSERT INTO testtable(id, name, email) values(default, 'Viking Baleog', 'aleog@north.se')");
	mysql_query(&mysql, "INSERT INTO testtable(id, name, email) values(default, 'ТипичнаяФамилия ТипичноеИмя', 'typical@typical.ru')");
	mysql_query(&mysql, "INSERT INTO testtable(id, name, email) values(default, 'Проверка_Поля_на_достаточно_длинную_и_нестандартную_фамилию такое_же_длинное_имя_с_нестандартными_символами_#!@$#?\_', 'почта@русская.ру')");

	mysql_query(&mysql, "SELECT * FROM testtable");

	if (res = mysql_store_result(&mysql)) {
		while (row = mysql_fetch_row(res)) {
			for (i = 0; i < mysql_num_fields(res); i++) {
				cout << row[i] << "  ";
			}
			cout << endl;
		}
	}
	else
		cout << "Error MySql " << mysql_error(&mysql);

	mysql_close(&mysql);

	system("Pause");

	return 0;
}
