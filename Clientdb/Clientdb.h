#pragma once
#include <iostream>
#include <pqxx/pqxx>
#include <exception>
#include <memory>
#include <vector>
#include "SecondaryFunction.h"


class Clientdb
{
private:
	std::unique_ptr<pqxx::connection> connect;

	const std::string selectTab{
		"select p.id, p.firstName, p.lastName, p.email, ph.phoneNumber from person p\
		left join phone ph on ph.personid = p.id " };
	std::array<const std::string, 4> sch_col = { 
		"where p.firstName like ",
		"where p.lastName like ",
		"where p.email like ",
		"where ph.phoneNumber like " };
	std::array<const std::string, 4> chg_col = {
		"firstName",
		"lastName",
		"email",
		"phoneNumber" };

	struct Table
	{
		int id;
		std::wstring fname;
		std::wstring lname;
		std::wstring mail;
		std::wstring phone;
	};
	void printTabHeader();
	void printTab(const Table&);
	void delClientPhones(const std::string& idpers);

public:
	enum Column{ FIRSTNAME, LASTNAME, EMAIL, PHONE };

	Clientdb(pqxx::connection*);
	bool is_open();
	std::wstring dbname();
	void createTable();
	void addClient(const std::wstring& firstName, const std::wstring& lastName, const std::wstring& email);
	void showTable();
	void search(Column, const std::wstring&);
	bool idPersonIsAvail(const int id);
	bool idPhoneIsAvail(const int idpers, const int idphone);
	void addPhone(int, const std::wstring&);
	void change(Column c, const int id, const std::wstring& changestr);
	void showPerson(const int idPerson);
	void showPhone(const int idPerson);
	void delPerson(const int idPerson);
	void delPhone(const int idPhone);

};
