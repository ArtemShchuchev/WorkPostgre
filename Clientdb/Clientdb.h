#pragma once
#include <iostream>
#include <pqxx/pqxx>
#include <exception>
#include <memory>


class Clientdb
{
private:
	std::unique_ptr<pqxx::connection> connect;
	const std::string pers_str{
		"CREATE TABLE IF NOT EXISTS Person "
		"(id serial primary key, "
		"firstName varchar(32) not null, "
		"lastName varchar(32) not null, "
		"email varchar(64) not null);" };
	const std::string phone_str{
		"CREATE TABLE IF NOT EXISTS Phone "
		"(id serial primary key, "
		"personId int references Person(id), "
		"phoneNumber varchar(16) not null);" };
	const std::string insPerson{ "INSERT INTO Person(firstName, lastName, email) VALUES" };
	const std::string insPhone{ "INSERT INTO Phone(personId, phoneNumber) VALUES" };

public:
	Clientdb(pqxx::connection*);
	void createTable();
	void addCl(const std::string& firstName, const std::string& lastName, const std::string& email);
};
