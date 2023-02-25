#include <pqxx/pqxx>
#include "SecondaryFunction.h"
#include "Clientdb/Clientdb.h"

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

void fillbd(Clientdb&);
void addphone(Clientdb&);
void changedata(Clientdb&);
void searchdata(Clientdb&);
void deldata(Clientdb&);

int main(int argc, char** argv)
{
	printHeader(L"Работа с PostgreSQL из C++");
	
	try
	{
		auto c = new pqxx::connection(
			"host=localhost "
			"port=5432 "
			"dbname=WorkPostgre "
			"user=postgres "
			"password=postgres");
		Clientdb client(c);
		std::wcout << L"Соединение с БД...\n";
		if (client.is_open()) std::wcout << L"База данных \"" << client.dbname() << L"\" успешно подключена.\n";
		else throw std::runtime_error("Не удалось подключиться к БД");
		std::wcout << L"\nСоздаю таблицы...\n";
		client.createTable();
		system("pause");	// Команда задержки экрана
		
		fillbd(client);		// заполнение БД

		addphone(client);	// добавить телефон

		changedata(client);	// изменение записей

		searchdata(client);	// поиск данных

		deldata(client);	// удаление данных
	}
	catch (std::exception const& err)
	{
		/*
			Я сдаюсь, как победить этот дурдом не знаю...
			Если сделать ошибку в учетной записи - он выводит
			сообщение об ошибке в одной кодировке (ANSI).
			А если допустить ошибку в таблице, ну например
			для связи указать не существующий слолбец, то 
			он сообщит об этом в другой кодировке (UTF-8). o|
		*/
		std::string errstr = err.what();
		consoleCol(col::br_red);
		std::wcout << L"\nОшибка типа: " << typeid(err).name() << "\n";

		std::wcerr << L"\nСообщение об ошибке:\n";
		std::wcerr << utf2wide(errstr) << '\n';

		std::wcerr << L"\nОшибка в учетке:\n";
		std::wcerr << ansi2wide(errstr) << '\n';
		consoleCol(col::cancel);
	}

	std::wcout << "\n";

	return 0;
}



void fillbd(Clientdb& client)
{
	while (true)	// заполняю БД клиентами
	{
		printHeader(L"Работа с PostgreSQL из C++");
		client.showTable();
		std::wstring qwery;
		do
		{
			std::wcout << L"\nДобавить нового клиента? д/н "; std::wcin >> qwery;
		} while (qwery != L"д" and qwery != L"н");
		if (qwery == L"н") break;
		std::wstring name, lname, mail;
		std::wcout << L"Введите нового клиента:\nИмя: "; std::wcin >> name;
		std::wcout << L"Фамилия: "; std::wcin >> lname;
		std::wcout << L"@mail: "; std::wcin >> mail;
		client.addClient(name, lname, mail);
	}
}

void addphone(Clientdb& client)
{
	while (true)	// добавить телефон
	{
		printHeader(L"Работа с PostgreSQL из C++");
		client.showTable();
		std::wstring qwery;
		do
		{
			std::wcout << L"\nДобавить телефон? д/н "; std::wcin >> qwery;
		} while (qwery != L"д" and qwery != L"н");
		if (qwery == L"н") break;
		int id(0);
		do
		{
			std::wcout << L"Введите id клиента или \"0\" для отмены: "; std::wcin >> id;
		} while (id and !client.idPersonIsAvail(id));
		if (!id) break;
		std::wstring phone;
		std::wcout << L"Введите номер телефона: "; std::wcin >> phone;
		client.addPhone(id, phone);
	}
}

void changedata(Clientdb& client)
{
	while (true)	// меняю данные
	{
		printHeader(L"Работа с PostgreSQL из C++");
		client.showTable();
		std::wcout << L"\nОбновление данных клиента\n";
		int id(-1), idphone(-1);
		do
		{
			std::wcout << L"Введите id клиента или \"0\" для отмены: "; std::wcin >> id;
		} while (id and !client.idPersonIsAvail(id));
		if (!id) break;
		printHeader(L"Работа с PostgreSQL из C++");
		client.showPerson(id);
		std::wcout << L"\nЧто менять?\n1. Имя\n2. Фамилия\n3. @mail\n4. Телефон ";
		int choice(0);
		while (choice <= 0 or choice > 4) std::wcin >> choice;
		if (choice == 4)	// смена номера телефона
		{
			printHeader(L"Работа с PostgreSQL из C++");
			client.showPhone(id);
			do
			{
				std::wcout << L"\nВведите id телефона или \"0\" для отмены: ";
				std::wcin >> idphone;
			} while (idphone and !client.idPhoneIsAvail(id, idphone));
		}
		if (!idphone) break;

		std::wstring user;
		std::wcout << L"На что менять: "; std::wcin >> user;
		if (idphone != -1) id = idphone;
		client.change(static_cast<Clientdb::Column>(choice - 1), id, user);
	}
}

void searchdata(Clientdb& client)
{
	while (true)
	{
		printHeader(L"Работа с PostgreSQL из C++");
		client.showTable();
		std::wcout << L"\nПоиск клиента по данным\n";
		std::wcout << L"Что искать?\n1. Имя\n2. Фамилия\n3. @mail\n4. Телефон\n0 - далее...";
		int choice(-1);
		while (choice < 0 or choice > 4) std::wcin >> choice;
		if (!choice) break;
		
		std::wstring userinp;
		Clientdb::Column col(Clientdb::FIRSTNAME);
		switch (choice)
		{
		case 1:
			std::wcout << L"Имя: "; std::wcin >> userinp;
			col = Clientdb::FIRSTNAME;
			break;
		case 2:
			std::wcout << L"Фамилия: "; std::wcin >> userinp;
			col = Clientdb::LASTNAME;
			break;
		case 3:
			std::wcout << L"@mail: "; std::wcin >> userinp;
			col = Clientdb::EMAIL;
			break;
		case 4:
			std::wcout << L"Телефон: "; std::wcin >> userinp;
			col = Clientdb::PHONE;
			break;

		default:
			return;
			break;
		}
		client.search(col, userinp);
		system("pause");
	}
}

void deldata(Clientdb& client)
{
	while (true)	// удаляю данные
	{
		printHeader(L"Работа с PostgreSQL из C++");
		client.showTable();
		std::wcout << L"\nУдаление данных\n";
		int id(-1), idphone(-1), choice(-1);

		std::wcout << L"Что удалить?\n1. Клиента\n2. Телефон\n0 - выход\n";
		while (choice < 0 or choice > 2) std::wcin >> choice;
		if (!choice) break;

		do
		{
			std::wcout << L"Введите id клиента или \"0\" для отмены: "; std::wcin >> id;
		} while (id and !client.idPersonIsAvail(id));
		if (!id) break;

		if (choice == 2)	// удаление номера телефона
		{
			client.showPhone(id);
			do
			{
				std::wcout << L"Введите id телефона или \"0\" для отмены: ";
				std::wcin >> idphone;
			} while (idphone and !client.idPhoneIsAvail(id, idphone));
		}
		if (!idphone) break;

		if (idphone != -1) client.delPhone(idphone);
		else client.delPerson(id);
	}
}