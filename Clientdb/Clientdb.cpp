#include "Clientdb.h"

Clientdb::Clientdb(pqxx::connection* c) : connect(c)
{
}

void Clientdb::createTable()
{
	pqxx::work tx{ *connect };
	tx.exec(pers_str);
	tx.exec(phone_str);
	tx.commit();
}

void Clientdb::addCl(const std::string& firstName, const std::string& lastName, const std::string& email)
{
	pqxx::work tx{ *connect };
	tx.exec(insPerson + "('" + firstName + "', '" + lastName + "', '" + email + "');");
	tx.commit();
}
