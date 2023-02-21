#include <pqxx/pqxx>
#include "SecondaryFunction.h"
#include "Clientdb/Clientdb.h"
#include "main.h"


/*
Создайте программу для управления клиентами на C++.

Требуется хранить персональную информацию о клиентах:

имя
фамилия
email
телефон
Сложность в том, что телефон у клиента может быть не один, а два, три и даже больше.
 А может и вообще не быть телефона (например, он не захотел его оставлять).

Вам необходимо разработать структуру БД для хранения информации и написать
 класс на С++ для управления данными, со следующими методами:

Метод, создающий структуру БД (таблицы)
Метод, позволяющий добавить нового клиента
Метод, позволяющий добавить телефон для существующего клиента
Метод, позволяющий изменить данные о клиенте
Метод, позволяющий удалить телефон для существующего клиента
Метод, позволяющий удалить существующего клиента
Метод, позволяющий найти клиента по его данным (имени, фамилии, email-у или телефону)
Методы выше являются обязательными, но это не значит что должны быть только они.
При необходимости можете создавать дополнительные методы и классы.

Также предоставьте код, демонстрирующий работу написанного вами класса.
*/

int main(int argc, char** argv)
{
	//system("chcp 1251");
	printHeader(L"Работа с PostgreSQL из C++");
	//setlocale(LC_ALL, "ru_RU.UTF-8");
	try
	{
		auto c = new pqxx::connection(
			"host=localhost "
			"port=5432 "
			"dbname=WorkPostgre "
			"user=postgres "
			"password=postgres");
		std::wcout << L"Соединение с БД " << c->dbname() << "...\n";
		Clientdb client(c);
		client.createTable();
		
		while (true)	// заполняю БД клиентами
		{
			std::wstring qwery;
			do
			{
				std::wcout << L"Добавить нового клиента? д/н "; std::wcin >> qwery;
			} while (qwery != L"д" and qwery != L"н");
			if (qwery == L"н") break;
			std::wstring name, lname, mail;
			std::wcout << L"Введите нового клиента:\nИмя: "; std::wcin >> name;
			std::wcout << L"Фамилия: "; std::wcin >> lname;
			std::wcout << L"@mail: "; std::wcin >> mail;
			client.addCl(utf8_encode(name), utf8_encode(lname), utf8_encode(mail));
		}
		
	}
	catch (std::exception const& err)
	{
		consoleCol(col::br_red);
		std::wcerr << L"\nОшибка: " << utf8_decode(err.what()) << '\n';
		std::wcerr << L"\nОшибка: " << ansi2unicode(err.what()) << '\n';
		consoleCol(col::cancel);
	}

	std::wcout << "\n";
	//system("pause"); // Команда задержки экрана

	return 0;
}
