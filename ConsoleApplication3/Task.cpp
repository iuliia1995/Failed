#include "Task.h"
#include "Database.h"
#include <iostream>
#include <libpq-fe.h>
#include <windows.h>

using namespace std;

Task::Task(int id, const string& title, const string& description,
    const string& status, int userId, const string& createdAt)
    : id(id), title(title), description(description),
    status(status), userId(userId), createdAt(createdAt) {
}

int Task::getId() const { return id; }
string Task::getTitle() const { return title; }
string Task::getDescription() const { return description; }
string Task::getStatus() const { return status; }
int Task::getUserId() const { return userId; }
string Task::getCreatedAt() const { return createdAt; }

string Task::getInfo() const {
    return "ID: " + to_string(id) +
        ", Заголовок: " + title +
        ", Описание: " + description +
        ", Статус: " + status +
        ", ID пользователя: " + to_string(userId) +
        ", Создана: " + createdAt;
}

bool Task::addTask(const shared_ptr<Database>& db, const string& title,
    const string& description, int userId) {
    if (!db->isConnected()) return false;

    string userIdStr = to_string(userId);
    const char* paramValues[3] = { title.c_str(), description.c_str(), userIdStr.c_str() };

    PGresult* res = PQexecParams(db->getConnection(),
        "INSERT INTO Tasks (title, description, user_id) VALUES ($1, $2, $3)",
        3, NULL, paramValues, NULL, NULL, 0);

    bool success = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!success) {
        cout << "Ошибка добавления задачи: " << PQerrorMessage(db->getConnection()) << endl;
    }
    PQclear(res);
    return success;
}

bool Task::updateTaskStatus(const shared_ptr<Database>& db, int taskId, const string& newStatus) {
    if (!db->isConnected()) return false;

    string taskIdStr = to_string(taskId);
    const char* paramValues[2] = { newStatus.c_str(), taskIdStr.c_str() };

    PGresult* res = PQexecParams(db->getConnection(),
        "UPDATE Tasks SET status = $1 WHERE id = $2",
        2, NULL, paramValues, NULL, NULL, 0);

    bool success = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!success) {
        cout << "Ошибка обновления статуса: " << PQerrorMessage(db->getConnection()) << endl;
    }
    PQclear(res);
    return success;
}

bool Task::deleteTask(const shared_ptr<Database>& db, int taskId) {
    if (!db->isConnected()) return false;

    string taskIdStr = to_string(taskId);
    const char* paramValues[1] = { taskIdStr.c_str() };

    PGresult* res = PQexecParams(db->getConnection(),
        "DELETE FROM Tasks WHERE id = $1",
        1, NULL, paramValues, NULL, NULL, 0);

    bool success = PQresultStatus(res) == PGRES_COMMAND_OK;
    PQclear(res);
    return success;
}

Task Task::getTaskById(const shared_ptr<Database>& db, int taskId) {
    if (!db->isConnected()) return Task(-1, "", "", "", -1, "");

    string taskIdStr = to_string(taskId);
    const char* paramValues[1] = { taskIdStr.c_str() };

    PGresult* res = PQexecParams(db->getConnection(),
        "SELECT id, title, description, status, user_id, created_at FROM Tasks WHERE id = $1",
        1, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        int id = stoi(PQgetvalue(res, 0, 0));
        string title = PQgetvalue(res, 0, 1);
        string description = PQgetvalue(res, 0, 2);
        string status = PQgetvalue(res, 0, 3);
        int userId = stoi(PQgetvalue(res, 0, 4));
        string createdAt = PQgetvalue(res, 0, 5);

        PQclear(res);
        return Task(id, title, description, status, userId, createdAt);
    }

    PQclear(res);
    return Task(-1, "", "", "", -1, "");
}

vector<Task> Task::getTasksByUser(const shared_ptr<Database>& db, int userId) {
    vector<Task> tasks;

    if (!db->isConnected()) return tasks;

    string userIdStr = to_string(userId);
    const char* paramValues[1] = { userIdStr.c_str() };

    PGresult* res = PQexecParams(db->getConnection(),
        "SELECT id, title, description, status, user_id, created_at FROM Tasks WHERE user_id = $1 ORDER BY id",
        1, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int rows = PQntuples(res);
        for (int i = 0; i < rows; i++) {
            int id = stoi(PQgetvalue(res, i, 0));
            string title = PQgetvalue(res, i, 1);
            string description = PQgetvalue(res, i, 2);
            string status = PQgetvalue(res, i, 3);
            int taskUserId = stoi(PQgetvalue(res, i, 4));
            string createdAt = PQgetvalue(res, i, 5);
            tasks.emplace_back(id, title, description, status, taskUserId, createdAt);
        }
    }

    PQclear(res);
    return tasks;
}