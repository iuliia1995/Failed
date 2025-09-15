#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <windows.h>
#include "Database.h"
#include "User.h"
#include "Task.h"

using namespace std;

void displayMenu() {
    cout << "\n Система управления задачами " << endl;
    cout << "1. Добавить пользователя" << endl;
    cout << "2. Показать всех пользователей" << endl;
    cout << "3. Добавить задачу пользователю" << endl;
    cout << "4. Показать задачи пользователя" << endl;
    cout << "5. Обновить статус задачи" << endl;
    cout << "6. Удалить задачу" << endl;
    cout << "7. Выход" << endl;
    cout << "Выберите опцию: ";
}

void addUser(shared_ptr<Database> db) {
    string firstName, lastName, email;

    cout << "\n Добавление пользователя " << endl;
    cout << "Введите имя: ";
    cin >> firstName;
    cout << "Введите фамилию: ";
    cin >> lastName;
    cout << "Введите email: ";
    cin >> email;

    if (User::addUser(db, firstName, lastName, email)) {
        cout << "Пользователь успешно добавлен!" << endl;
    }
    else {
        cout << "Ошибка при добавлении пользователя!" << endl;
    }
}

void showAllUsers(shared_ptr<Database> db) {
    cout << "\n Все пользователи " << endl;
    vector<User> users = User::getAllUsers(db);

    if (users.empty()) {
        cout << "Пользователи не найдены" << endl;
    }
    else {
        for (const auto& user : users) {
            cout << user.getInfo() << endl;
        }
    }
}

void addTaskToUser(shared_ptr<Database> db) {
    cout << "\n Добавление задачи пользователю " << endl;

    showAllUsers(db);

    int userId;
    cout << "Введите ID пользователя: ";
    cin >> userId;
    cin.ignore(); // Очистка буфера

    if (!User::userExists(db, userId)) {
        cout << "Пользователь с ID " << userId << " не существует!" << endl;
        return;
    }

    string title, description;
    cout << "Введите название задачи: ";
    getline(cin, title);
    cout << "Введите описание задачи: ";
    getline(cin, description);

    if (Task::addTask(db, title, description, userId)) {
        cout << "Задача успешно добавлена!" << endl;
    }
    else {
        cout << "Ошибка при добавлении задачи!" << endl;
    }
}

void showUserTasks(shared_ptr<Database> db) {
    cout << "\n Задачи пользователя " << endl;

    showAllUsers(db);

    int userId;
    cout << "Введите ID пользователя: ";
    cin >> userId;

    if (!User::userExists(db, userId)) {
        cout << "Пользователь с ID " << userId << " не существует!" << endl;
        return;
    }

    vector<Task> tasks = Task::getTasksByUser(db, userId);

    if (tasks.empty()) {
        cout << "Задачи не найдены" << endl;
    }
    else {
        for (const auto& task : tasks) {
            cout << task.getInfo() << endl;
        }
    }
}

void updateTaskStatus(shared_ptr<Database> db) {
    cout << "\n Обновление статуса задачи " << endl;

    int taskId;
    cout << "Введите ID задачи: ";
    cin >> taskId;
    cin.ignore(); // Очистка буфера

    Task task = Task::getTaskById(db, taskId);
    if (task.getId() == -1) {
        cout << "Задача с ID " << taskId << " не найдена!" << endl;
        return;
    }

    cout << "Текущая задача: " << task.getInfo() << endl;

    cout << "Выберите новый статус:" << endl;
    cout << "1. Новая" << endl;
    cout << "2. В работе" << endl;
    cout << "3. Выполнена" << endl;
    cout << "Выберите статус (1-3): ";

    int choice;
    cin >> choice;
    cin.ignore(); // Очистка буфера

    string newStatus;
    switch (choice) {
    case 1: newStatus = "Новая"; break;
    case 2: newStatus = "В работе"; break;
    case 3: newStatus = "Выполнена"; break;
    default:
        cout << "Неверный выбор!" << endl;
        return;
    }

    if (Task::updateTaskStatus(db, taskId, newStatus)) {
        cout << "Статус задачи успешно обновлен!" << endl;
    }
    else {
        cout << "Ошибка при обновлении статуса!" << endl;
    }
}

void deleteTask(shared_ptr<Database> db) {
    cout << "\n Удаление задачи " << endl;

    int taskId;
    cout << "Введите ID задачи для удаления: ";
    cin >> taskId;
    cin.ignore(); // Очистка буфера

    Task task = Task::getTaskById(db, taskId);
    if (task.getId() == -1) {
        cout << "Задача с ID " << taskId << " не найдена!" << endl;
        return;
    }

    if (Task::deleteTask(db, taskId)) {
        cout << "Задача успешно удалена!" << endl;
    }
    else {
        cout << "Ошибка при удалении задачи!" << endl;
    }
}

int main() {
    system("chcp 1251");
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    auto db = Database::createConnection();

    if (!db->isConnected()) {
        cout << "Не удалось подключиться к базе данных!" << endl;
        return 1;
    }

    cout << "Подключение к базе данных успешно!" << endl;

    int choice;
    do {
        displayMenu();
        cin >> choice;
        cin.ignore(); // Очистка буфера после ввода числа

        switch (choice) {
        case 1: addUser(db); break;
        case 2: showAllUsers(db); break;
        case 3: addTaskToUser(db); break;
        case 4: showUserTasks(db); break;
        case 5: updateTaskStatus(db); break;
        case 6: deleteTask(db); break;
        case 7: cout << "Выход..." << endl; break;
        default:
            cout << "Неверный выбор! Попробуйте снова." << endl;
            cin.clear(); // Сброс флагов ошибок
            cin.ignore(10000, '\n'); // Очистка буфера
        }
    } while (choice != 7);

    return 0;
}