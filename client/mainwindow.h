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

// Оконный клиент StAlGoLM.
// Подключается к серверу по TCP (порт 33333), отправляет те же текстовые
// команды, что и PuTTY, и показывает ответы. Каждая команда завершается
// байтом-разделителем 0x01 — как требует протокол сервера.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
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
    void buildUi();
    void sendCommand(const QString& command);
    void log(const QString& who, const QString& text);
    void setConnectionState(bool connected);
    void setRadiosEnabled(bool on);
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

    // Ответ (для task2 — выбор варианта, для остальных — число)
    QButtonGroup *optGroup;
    QLineEdit *editAnswer;
    QPushButton *btnAnswer;

    // Журнал обмена
    QPlainTextEdit *logView;

    bool loggedIn = false;
    int activeTaskType = 0;   // 0 — активного задания нет, 1..4 — номер задания
};

#endif // MAINWINDOW_H
