#pragma once
#include <string>
#include <vector>
#include <memory>

using namespace std;

class Database;

class Task {
private:
    int id;
    string title;
    string description;
    string status;
    int userId;
    string createdAt;

public:
    Task(int id, const string& title, const string& description,
        const string& status, int userId, const string& createdAt);

    int getId() const;
    string getTitle() const;
    string getDescription() const;
    string getStatus() const;
    int getUserId() const;
    string getCreatedAt() const;
    string getInfo() const;

    static bool addTask(const shared_ptr<Database>& db, const string& title,
        const string& description, int userId);
    static vector<Task> getTasksByUser(const shared_ptr<Database>& db, int userId);
    static bool updateTaskStatus(const shared_ptr<Database>& db, int taskId, const string& newStatus);
    static bool deleteTask(const shared_ptr<Database>& db, int taskId);
    static Task getTaskById(const shared_ptr<Database>& db, int taskId);
};