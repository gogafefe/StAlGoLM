#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

class QLineEdit;
class QPushButton;
class QPlainTextEdit;
class QSpinBox;
class QComboBox;
class QRadioButton;
class QButtonGroup;
class QLabel;

/**
 * @brief Главное окно TCP-клиента StAlGoLM.
 *
 * Окно подключается к серверу на порту 33333, формирует текстовые команды,
 * завершает их байтом 0x01 и отображает ответы. Интерфейс поддерживает
 * регистрацию, авторизацию, четыре типа заданий и личную статистику.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /** @brief Создаёт интерфейс клиента и начинает подключение к серверу. */
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    // Соединение
    void onConnectClicked();
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketErrorOccurred(QAbstractSocket::SocketError error);

    // Аккаунт
    void onRegClicked();
    void onAuthClicked();
    void onLogoutClicked();
    void onWhoamiClicked();

    // Задания
    void onGetTaskClicked();
    void onAnswerClicked();
    void onMystatsClicked();

private:
    /** @brief Создаёт элементы и компоновки пользовательского интерфейса. */
    void buildUi();
    /** @brief Отправляет текстовую команду с разделителем 0x01. */
    void sendCommand(const QString& command);
    /** @brief Добавляет сообщение в журнал обмена. */
    void log(const QString& who, const QString& text);
    /** @brief Обновляет элементы интерфейса при подключении или отключении. */
    void setConnectionState(bool connected);
    /** @brief Включает или отключает варианты ответа Task2 и Task3. */
    void setRadiosEnabled(bool on);
    /** @brief Сбрасывает локальное состояние текущего задания. */
    void resetTaskState();

    QTcpSocket *socket = nullptr;

    // Подключение
    QLineEdit *editHost;
    QSpinBox  *spinPort;
    QPushButton *btnConnect;

    // Аккаунт
    QLineEdit *editLogin;
    QLineEdit *editPassword;
    QLineEdit *editEmail;
    QPushButton *btnReg;
    QPushButton *btnAuth;
    QPushButton *btnLogout;
    QPushButton *btnWhoami;
    QLabel *lblUser;

    // Задания
    QComboBox *comboTask;
    QSpinBox  *spinTaskNum;
    QPushButton *btnGetTask;
    QPushButton *btnMystats;

    // Ответ (для task2/task3 — выбор варианта, для task1/task4 — число)
    QButtonGroup *optGroup;
    QLineEdit *editAnswer;
    QPushButton *btnAnswer;

    // Журнал обмена
    QPlainTextEdit *logView;

    bool loggedIn = false;
    int activeTaskType = 0;   // 0 — активного задания нет, 1..4 — номер задания
};

#endif // MAINWINDOW_H
