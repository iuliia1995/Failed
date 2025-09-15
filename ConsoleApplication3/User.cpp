#include "User.h"
#include "Database.h"
#include <iostream>
#include <libpq-fe.h>
#include <windows.h>

using namespace std;

User::User(int id, const string& firstName, const string& lastName,
    const string& email, const string& createdAt)
    : id(id), firstName(firstName), lastName(lastName),
    email(email), createdAt(createdAt) {
}

int User::getId() const { return id; }
string User::getFirstName() const { return firstName; }
string User::getLastName() const { return lastName; }
string User::getEmail() const { return email; }
string User::getCreatedAt() const { return createdAt; }

string User::getInfo() const {
    return "ID: " + to_string(id) +
        ", Имя: " + firstName + " " + lastName +
        ", Email: " + email +
        ", Создан: " + createdAt;
}

bool User::addUser(const shared_ptr<Database>& db, const string& firstName,
    const string& lastName, const string& email) {
    if (!db->isConnected()) return false;

    const char* paramValues[3] = { firstName.c_str(), lastName.c_str(), email.c_str() };

    PGresult* res = PQexecParams(db->getConnection(),
        "INSERT INTO Users (first_name, last_name, email) VALUES ($1, $2, $3)",
        3, NULL, paramValues, NULL, NULL, 0);

    bool success = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!success) {
        cout << "Ошибка добавления пользователя: " << PQerrorMessage(db->getConnection()) << endl;
    }
    PQclear(res);
    return success;
}

vector<User> User::getAllUsers(const shared_ptr<Database>& db) {
    vector<User> users;

    if (!db->isConnected()) {
        cout << "Нет подключения к базе данных!" << endl;
        return users;
    }

    PGresult* res = PQexec(db->getConnection(),
        "SELECT id, first_name, last_name, email, created_at FROM Users ORDER BY id");

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int rows = PQntuples(res);
        for (int i = 0; i < rows; i++) {
            int id = stoi(PQgetvalue(res, i, 0));
            string firstName = PQgetvalue(res, i, 1);
            string lastName = PQgetvalue(res, i, 2);
            string email = PQgetvalue(res, i, 3);
            string createdAt = PQgetvalue(res, i, 4);
            users.emplace_back(id, firstName, lastName, email, createdAt);
        }
    }
    else {
        cout << "Ошибка при выполнении запроса: " << PQerrorMessage(db->getConnection()) << endl;
    }

    PQclear(res);
    return users;
}

User User::getUserById(const shared_ptr<Database>& db, int id) {
    if (!db->isConnected()) {
        cout << "Нет подключения к базе данных!" << endl;
        return User(-1, "", "", "");
    }

    string idStr = to_string(id);
    const char* paramValues[1] = { idStr.c_str() };

    PGresult* res = PQexecParams(db->getConnection(),
        "SELECT id, first_name, last_name, email, created_at FROM Users WHERE id = $1",
        1, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        int userId = stoi(PQgetvalue(res, 0, 0));
        string firstName = PQgetvalue(res, 0, 1);
        string lastName = PQgetvalue(res, 0, 2);
        string email = PQgetvalue(res, 0, 3);
        string createdAt = PQgetvalue(res, 0, 4);
        PQclear(res);
        return User(userId, firstName, lastName, email, createdAt);
    }

    PQclear(res);
    return User(-1, "", "", "");
}

bool User::userExists(const shared_ptr<Database>& db, int id) {
    return getUserById(db, id).getId() != -1;
}