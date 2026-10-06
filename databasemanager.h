#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCryptographicHash>
#include <QVector>

/** @brief Роль пользователя и уровень доступа к серверным командам. */
enum UserRole {
    ROLE_USER,
    ROLE_MODERATOR,
    ROLE_ADMIN,
    ROLE_ERR
};

/** @brief Преобразует строковое имя роли в UserRole. */
UserRole stringToRole(const QString& roleStr);

/** @brief Возвращает строковое имя роли. */
QString roleToString(UserRole role);

/** @brief Краткая информация о пользователе для административного списка. */
struct UserInfo {
    QString login;
    QString email;
    UserRole role;
    QString registeredAt;
    QString lastAuth;
};

/** @brief Агрегированная статистика пользователей сервера. */
struct UserStats {
    int totalUsers;
    int newUsersMonth;      // за последний месяц
    int activeUsersMonth;   // активные за месяц
};

/**
 * @brief Синглтон для работы с базой данных SQLite.
 *
 * Менеджер создаёт таблицу пользователей и четыре таблицы статистики,
 * выполняет регистрацию и авторизацию, управляет ролями и результатами
 * решения учебных заданий. База хранится в файле server.db текущего каталога.
 */
class DatabaseManager {
public:
    /** @return Единственный экземпляр менеджера базы данных. */
    static DatabaseManager* getInstance();

    // === Пользователи ===
    /** @brief Регистрирует пользователя; пароль сохраняется как SHA-256. */
    bool regUser(const QString& login, const QString& password, const QString& email, UserRole role);
    /** @brief Проверяет логин и пароль пользователя. */
    bool authUser(const QString& login, const QString& password);
    /** @brief Проверяет учётные данные и возвращает роль пользователя. */
    UserRole authUserWithRole(const QString& login, const QString& password);
    /** @brief Возвращает роль пользователя по логину. */
    UserRole getUserRole(const QString& login);
    /** @brief Возвращает электронную почту пользователя. */
    QString getUserEmail(const QString& login);
    /** @brief Удаляет пользователя и связанные строки статистики. */
    bool deleteUser(const QString& login);
    /** @brief Изменяет роль существующего пользователя. */
    bool updateUserRole(const QString& login, UserRole newRole);
    /** @brief Проверяет существование пользователя с заданным логином. */
    bool userExists(const QString& login);

    // === Для админа/модератора ===
    /** @brief Возвращает список всех пользователей. */
    QVector<UserInfo> getAllUsers();
    /** @brief Возвращает общую, месячную и активную статистику пользователей. */
    UserStats getStats();

    // Создать строки со значениями 0 во всех 4 таблицах статистики для нового пользователя
    /** @brief Создаёт нулевые строки пользователя во всех таблицах заданий. */
    bool createStatisticRows(const QString& login);
    // Обновить ячейку: taskNum (1-4), problemNum (1-40), delta (+1 или -1)
    /** @brief Изменяет результат конкретного шаблона задания на delta. */
    bool updateStatistic(const QString& login, int taskNum, int problemNum, int delta);
    // Получить всю строку статистики (40 значений) для пользователя
    /** @brief Возвращает 40 значений статистики указанного типа задания. */
    QVector<int> getStatisticRow(const QString& login, int taskNum);
    // Удалить строки статистики пользователя (при удалении аккаунта)
    /** @brief Удаляет статистику пользователя из всех четырёх таблиц. */
    bool deleteStatisticRows(const QString& login);

private:
    DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase db;
    void initDatabase();
    // Создание 4 таблиц статистики (StatisticTask1..4)
    void initStatisticTables();
    void updateLastAuth(const QString& login);
};

#endif // DATABASEMANAGER_H
