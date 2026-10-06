#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include <QString>
#include <QStringList>
#include <QTcpSocket>
#include "databasemanager.h"

/**
 * @brief Состояние одного подключённого пользователя.
 *
 * Помимо данных авторизации структура хранит текущее задание и правильный
 * ответ, поэтому разные TCP-клиенты могут решать задания независимо.
 */
struct UserSession {
    QString login;
    QString email;
    UserRole role;
    QTcpSocket* msock;

    int currentTaskType;    // 0 = нет активного задания, 1-4 = тип задания
    int currentTaskNum;     // 1-40 = номер задания в рамках типа
    int currentCorrectAnswer;
};

/**
 * @brief Разбирает текстовую команду и вызывает соответствующий обработчик.
 * @param command Команда без завершающего байта 0x01.
 * @param session Сессия клиента, которую обработчик может изменить.
 * @return Текст ответа для отправки клиенту.
 */
QString processCommand(const QString& command, UserSession& session);

/** @name Обработчики команд аккаунта и администрирования */
///@{
QString cmd_reg(const QStringList& parts);
QString cmd_auth(const QStringList& parts, UserSession& session);
QString cmd_logout(UserSession& session);
QString cmd_whoami(const UserSession& session);
QString cmd_del(const QStringList& parts, UserSession& session);
QString cmd_role(const QStringList& parts, const UserSession& session);
QString cmd_users(const UserSession& session);
QString cmd_stats(const UserSession& session);
QString cmd_help();
///@}

/**
 * @brief Генерирует задание выбранного типа и сохраняет ответ в сессии.
 * @param taskType Тип задания от 1 до 4.
 * @param taskNum Номер шаблона задания.
 * @param session Сессия авторизованного пользователя.
 */
QString cmd_task(int taskType, int taskNum, UserSession& session);

/** @brief Проверяет ответ и обновляет статистику активного задания. */
QString cmd_answer(int userAnswer, UserSession& session);

/** @brief Возвращает личную статистику по одному или всем типам заданий. */
QString cmd_mystats(const QStringList& parts, const UserSession& session);

#endif // SERVER_FUNCTIONS_H
