#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include "server_functions.h"

/**
 * @brief TCP-сервер приложения StAlGoLM.
 *
 * Класс принимает подключения на порту 33333, накапливает входящие байты до
 * разделителя 0x01 и передаёт готовые текстовые команды в processCommand().
 * Для каждого сокета хранится отдельная UserSession.
 *
 * MyTcpServer реализован как синглтон: экземпляр создаётся и возвращается
 * методом getInstance(), а копирование запрещено.
 */
class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    /** @return Единственный экземпляр TCP-сервера. */
    static MyTcpServer* getInstance();

    /** @brief Останавливает прослушивание порта и уничтожает сервер. */
    ~MyTcpServer();

private:
    MyTcpServer(QObject *parent = nullptr);
    MyTcpServer(const MyTcpServer&) = delete;
    MyTcpServer& operator=(const MyTcpServer&) = delete;

    QTcpServer *mTcpServer;
    QMap<long, UserSession> mSessions;
    QMap<long, QByteArray> mBuffers; // накопленные, но ещё не завершённые байты на клиента

public slots:
    /** @brief Принимает новое TCP-подключение и создаёт для него сессию. */
    void slotNewConnection();

    /** @brief Удаляет данные сессии отключившегося клиента. */
    void slotClientDisconnected();

    /** @brief Читает команды клиента, вызывает обработчик и отправляет ответ. */
    void slotServerRead();
};

#endif // MYTCPSERVER_H
