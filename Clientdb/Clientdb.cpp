#include "Clientdb.h"

Clientdb::Clientdb(pqxx::connection* c) : connect(c)
{
}

bool Clientdb::is_open()
{
	return connect->is_open();
}

std::wstring Clientdb::dbname()
{
	return utf2wide(connect->dbname());
}

void Clientdb::createTable()
{
	const std::string pers_str{
		"CREATE TABLE IF NOT EXISTS person "
		"(id serial primary key, "
		"firstName varchar(16) not null, "
		"lastName varchar(16) not null, "
		"email varchar(32) not null);" };
	const std::string phone_str{
		"CREATE TABLE IF NOT EXISTS phone "
		"(id serial primary key, "
		"personId int references person(id) not null, "
		"phoneNumber varchar(16) null);" };

	pqxx::work tx{ *connect };
	tx.exec(pers_str);
	tx.exec(phone_str);
	tx.commit();
}

void Clientdb::addClient(const std::wstring& firstName, const std::wstring& lastName, const std::wstring& email)
{
	const std::string insPerson{ "INSERT INTO person(firstName, lastName, email) VALUES('" };

	pqxx::work tx{ *connect };
	tx.exec(insPerson + tx.esc(wide2utf(firstName)) + "', '" + tx.esc(wide2utf(lastName)) + "', '" + tx.esc(wide2utf(email)) + "');");
	tx.commit();
}

void Clientdb::showTable()
{
	printTabHeader();
	pqxx::work tx{ *connect };

	for (auto &[id, fn, ln, em, ph] :
		tx.query<int, std::string, std::string, std::string, std::optional<std::string>>
		(selectTab + "order by p.id"))
	{
		std::wstring phone = ph.has_value() ? utf2wide(*ph) : L"Нет номера";
		Table tab{ id, utf2wide(fn), utf2wide(ln), utf2wide(em), phone };
		printTab(tab);
	}
}

void Clientdb::search(Column c, const std::wstring& name)
{
	printTabHeader();
	pqxx::work tx{ *connect };

	for (auto &[id, fn, ln, em, ph] :
		tx.query<int, std::string, std::string, std::string, std::optional<std::string>>
		(selectTab + sch_col.at(c) + "'" + tx.esc(wide2utf(name)) + "'"))
	{
		std::wstring phone = ph.has_value() ? utf2wide(*ph) : L"Нет номера";
		Table tab{ id, utf2wide(fn), utf2wide(ln), utf2wide(em), phone };
		printTab(tab);
	}
}

bool Clientdb::idPersonIsAvail(const int id)
{
	std::string checkid{ "select id from person where id = " + std::to_string(id) };
	
	pqxx::work tx{ *connect };

	for (auto &[id] : tx.query<int>(checkid))
	{
		return true;
	}
	return false;
}

bool Clientdb::idPhoneIsAvail(const int idpers, const int idphone)
{
	std::string checkid{ "select ph.id from phone ph\
		join person p on p.id = ph.personid\
		where p.id = " + std::to_string(idpers) + " and ph.id = " + std::to_string(idphone) };
	
	pqxx::work tx{ *connect };

	for (auto& [id] : tx.query<int>(checkid))
	{
		return true;
	}
	return false;
}

void Clientdb::addPhone(int id, const std::wstring& phone)
{
	const std::string insPhone{ "INSERT INTO phone(personId, phoneNumber) VALUES(" };

	pqxx::work tx{ *connect };
	tx.exec(insPhone + std::to_string(id) + ", '" + tx.esc(wide2utf(phone)) + "');");
	tx.commit();
}

void Clientdb::change(Column c, const int id, const std::wstring& changestr)
{
	//printTabHeader();
	pqxx::work tx{ *connect };
	std::string updstr;
	if (c == PHONE)
	{
		updstr = "update phone set " + chg_col.at(c) + " = '" +
			tx.esc(wide2utf(changestr)) + "' where id = " + std::to_string(id);
	}
	else
	{
		updstr = "update person set " + chg_col.at(c) + " = '" +
			tx.esc(wide2utf(changestr)) + "' where id = " + std::to_string(id);
	}
	tx.exec(updstr);
	tx.commit();
}

void Clientdb::showPerson(const int idPerson)
{
	const std::string id_person{ "where p.id = " };

	printTabHeader();
	pqxx::work tx{ *connect };

	for (auto& [id, fn, ln, em, ph] :
		tx.query<int, std::string, std::string, std::string, std::optional<std::string>>
		(selectTab + id_person + std::to_string(idPerson)))
	{
		std::wstring phone = ph.has_value() ? utf2wide(*ph) : L"Нет номера";
		Table tab{ id, utf2wide(fn), utf2wide(ln), utf2wide(em), phone };
		printTab(tab);
	}
}

void Clientdb::showPhone(const int idPerson)
{
	const std::string selectPhone{
		"select ph.id, ph.phoneNumber from phone ph\
		join person p on p.id = ph.personid\
		where p.id = " + std::to_string(idPerson) };
	
	std::wcout << std::left << std::setw(4) << "id" << L"| Телефон\n";
	std::wcout << "----+------------------\n";
	pqxx::work tx{ *connect };

	for (auto& [id, ph] : tx.query<int, std::optional<std::string>>(selectPhone))
	{
		std::wstring phone = ph.has_value() ? utf2wide(*ph) : L"Нет номера";
		std::wcout << std::left << std::setw(4) << id << "| " << phone << "\n";
	}
}

void Clientdb::printTabHeader()
{
	std::wcout << std::left << std::setw(4) << "id" << "| "
		<< std::setw(17) << L"Имя" << "| "
		<< std::setw(17) << L"Фамилия" << "| "
		<< std::setw(33) << L"Эл. почта" << L"| Телефон\n";
	std::wcout << "----+------------------+------------------+----------------------------------+------------------\n";
}

void Clientdb::printTab(const Table& tab)
{
	std::wcout << std::left << std::setw(4) << tab.id << "| "
				<< std::setw(17) << tab.fname << "| "
				<< std::setw(17) << tab.lname << "| "
				<< std::setw(33) << tab.mail << "| "
				<< tab.phone << "\n";
}

// удаление всех телефонов персоны
void Clientdb::delClientPhones(const std::string& idpers)
{
	const std::string delphone{ "delete from phone \
		where id in (select ph.id from phone ph \
		join person p on p.id = ph.personid \
		where p.id = " + idpers + ");" };

	pqxx::work tx{ *connect };
	tx.exec(delphone);
	tx.commit();
}

// удаление персоны
void Clientdb::delPerson(const int idPerson)
{
	std::string id{ std::to_string(idPerson) };
	delClientPhones(id);

	pqxx::work tx{ *connect };
	tx.exec("delete from person where id = " + id + ";");
	tx.commit();
}

// удаление телефона
void Clientdb::delPhone(const int idPhone)
{
	std::string id{ std::to_string(idPhone) };
	pqxx::work tx{ *connect };
	tx.exec("delete from phone where id = " + id + ";");
	tx.commit();
}
