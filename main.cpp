#include <QCoreApplication>
#include "mytcpserver.h"

/**
 * @brief Точка входа серверного приложения.
 *
 * Создаёт цикл событий Qt и запускает единственный экземпляр MyTcpServer.
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MyTcpServer::getInstance();
    return a.exec();
}
