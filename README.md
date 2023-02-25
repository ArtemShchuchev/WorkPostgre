# Домашнее задание к занятию «Работа с PostgreSQL из C++»
Выполнив это задание, вы сможете использовать библиотеку libpq++ для подключения к базе данных PostgreSQL из С++ и выполнения запросов.

### Цель задания
1. Научиться делать SELECT-запросы к базе данных PostgreSQL из С++ для получения данных и отображения их в C++ приложении.
1. Научиться делать INSERT-, UPDATE- и DELETE-запросы к базе данных PostgreSQL из С++ для манипуляции данными из C++ приложения.

## До запуска требуются изменения!!!
- Заменить путь к библиотеке - Файл CMakeLists.txt в строке ```add_subdirectory(D:/UTIL/Lib/libpqxx-7.7.4 libpqxx-build)      # Укажите путь к libpq++```
- Изменить учетную запись БД - Файл main.cpp в строке (46) ```auto c = new pqxx::connection("host=localhost port=5432 dbname=WorkPostgre user=postgres password=postgres");```