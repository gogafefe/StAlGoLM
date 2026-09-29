#include "mytcpserver.h"
#include "server_functions.h"
#include <QDebug>

MyTcpServer* MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return &instance;
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent)
{
    mTcpServer = new QTcpServer(this);
    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "server is not started";
    } else {
        qDebug() << "server is started";
    }
}

MyTcpServer::~MyTcpServer()
{
    mTcpServer->close();
}

void MyTcpServer::slotNewConnection()
{
    QTcpSocket *clientSocket = mTcpServer->nextPendingConnection();
    clientSocket->write("Hello, World!!! I am echo server!\r\n");

    // currentTaskType = 0 (нет активного задания),
    // currentTaskNum = 0, currentCorrectAnswer = 0
    mSessions[clientSocket->socketDescriptor()] = UserSession{
        "", "", ROLE_USER, nullptr,
        0,  // currentTaskType — нет активного задания
        0,  // currentTaskNum
        0   // currentCorrectAnswer
    };

    connect(clientSocket, &QTcpSocket::readyRead,
            this, &MyTcpServer::slotServerRead);
    connect(clientSocket, &QTcpSocket::disconnected,
            this, &MyTcpServer::slotClientDisconnected);
}

void MyTcpServer::slotServerRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    long idsock = clientSocket->socketDescriptor();

    while(clientSocket->bytesAvailable() > 0)
    {
        QByteArray array = clientSocket->readAll();
        qDebug() << array << "\n";

        // Раньше здесь было "if(array == \"\\x01\")" — это работало,
        // только если разделитель приходил ОТДЕЛЬНЫМ, самостоятельным
        // куском. На практике текст команды и завершающий байт часто
        // приходят слитно за одно чтение (особенно на localhost),
        // и такая проверка их не ловила — байт \x01 просто прилипал
        // к последнему аргументу команды.
        //
        // Теперь ищем разделитель ВНУТРИ пришедших байт, а не сравниваем
        // с ними целиком, и копим неполные сообщения в mBuffers,
        // пока разделитель не найдётся (в том числе если он придёт
        // отдельным чтением позже).
        int sep = array.indexOf('\x01');
        if (sep == -1) {
            mBuffers[idsock].append(array);
        } else {
            mBuffers[idsock].append(array.left(sep));
            QString command = QString::fromUtf8(mBuffers[idsock]).trimmed();
            mBuffers[idsock].clear();

            if (!command.isEmpty()) {
                UserSession &session = mSessions[idsock];
                QString response = processCommand(command, session);
                clientSocket->write(response.toUtf8());
            }

            // Если после разделителя в этом же чтении есть ещё байты
            // (например, клиент отправил два сообщения подряд одним
            // пакетом) — они остаются в буфере для следующего прохода.
            if (sep + 1 < array.size())
                mBuffers[idsock].append(array.mid(sep + 1));
        }
    }
}

void MyTcpServer::slotClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    long idsock = clientSocket->socketDescriptor();
    //if (clientSocket) {
        mSessions.remove(idsock);
        mBuffers.remove(idsock);
        clientSocket->close();
        clientSocket->deleteLater();
    //}
}
