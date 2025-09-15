#include "Database.h"
#include <iostream>
#include <windows.h>

using namespace std;

Database::Database(const string& conninfo) {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    connection = PQconnectdb(conninfo.c_str());

    if (PQstatus(connection) != CONNECTION_OK) {
        cout << "Ошибка подключения: " << PQerrorMessage(connection) << endl;
    }
    else {
        cout << "Подключение успешно!" << endl;
        createTablesIfNotExist();
    }
}

void Database::createTablesIfNotExist() {
    // Создание таблицы Users
    const char* createUsersTable =
        "CREATE TABLE IF NOT EXISTS Users ("
        "id SERIAL PRIMARY KEY,"
        "first_name VARCHAR(50) NOT NULL,"
        "last_name VARCHAR(50) NOT NULL,"
        "email VARCHAR(100) UNIQUE NOT NULL,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")";

    PGresult* res = PQexec(connection, createUsersTable);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        cout << "Ошибка создания таблицы Users: " << PQerrorMessage(connection) << endl;
    }
    PQclear(res);

    // Создание таблицы Tasks
    const char* createTasksTable =
        "CREATE TABLE IF NOT EXISTS Tasks ("
        "id SERIAL PRIMARY KEY,"
        "title VARCHAR(100) NOT NULL,"
        "description TEXT,"
        "status VARCHAR(20) DEFAULT 'Новая',"
        "user_id INTEGER REFERENCES Users(id) ON DELETE CASCADE,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")";

    res = PQexec(connection, createTasksTable);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        cout << "Ошибка создания таблицы Tasks: " << PQerrorMessage(connection) << endl;
    }
    PQclear(res);
}

Database::~Database() {
    if (connection) {
        PQfinish(connection);
    }
}

PGconn* Database::getConnection() const {
    return connection;
}

bool Database::isConnected() const {
    return connection && PQstatus(connection) == CONNECTION_OK;
}

shared_ptr<Database> Database::createConnection() {
    string conninfo = "host=46.191.173.8 port=5432 dbname=studentdb1 user=postgres password=asdf=1234asdf=1234";
    return make_shared<Database>(conninfo);
}