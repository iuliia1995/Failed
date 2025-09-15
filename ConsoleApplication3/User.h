#pragma once
#include <string>
#include <vector>
#include <memory>

using namespace std;

class Database;

class User {
private:
    int id;
    string firstName;
    string lastName;
    string email;
    string createdAt;

public:
    User(int id, const string& firstName, const string& lastName,
        const string& email, const string& createdAt = "");

    int getId() const;
    string getFirstName() const;
    string getLastName() const;
    string getEmail() const;
    string getCreatedAt() const;
    string getInfo() const;

    static bool addUser(const shared_ptr<Database>& db, const string& firstName,
        const string& lastName, const string& email);
    static vector<User> getAllUsers(const shared_ptr<Database>& db);
    static User getUserById(const shared_ptr<Database>& db, int id);
    static bool userExists(const shared_ptr<Database>& db, int id);
};
