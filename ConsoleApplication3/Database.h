#pragma once
#include <libpq-fe.h>
#include <string>
#include <memory>

using namespace std;

class Database {
private:
    PGconn* connection;
    void createTablesIfNotExist();

public:
    Database(const string& conninfo);
    ~Database();

    PGconn* getConnection() const;
    bool isConnected() const;
    static shared_ptr<Database> createConnection();
};