#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QAbstractButton>
#include <QRegularExpression>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected,    this, &MainWindow::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);
    connect(socket, &QTcpSocket::readyRead,    this, &MainWindow::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &MainWindow::onSocketErrorOccurred);

    buildUi();

    // Сразу пробуем подключиться: адрес и порт уже подставлены.
    // Если сервер ещё не запущен, в журнале появится ошибка — тогда
    // запускаем сервер и нажимаем «Подключиться» вручную.
    socket->connectToHost(editHost->text(), quint16(spinPort->value()));
}

// ============================ Интерфейс ============================

void MainWindow::buildUi()
{
    setWindowTitle("StAlGoLM — клиент");
    resize(780, 700);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *root = new QVBoxLayout(central);

    // --- Подключение ---
    QGroupBox *gbConn = new QGroupBox("Подключение к серверу");
    QHBoxLayout *connRow = new QHBoxLayout(gbConn);
    connRow->addWidget(new QLabel("Хост:"));
    editHost = new QLineEdit("localhost");
    connRow->addWidget(editHost);
    connRow->addWidget(new QLabel("Порт:"));
    spinPort = new QSpinBox;
    spinPort->setRange(1, 65535);
    spinPort->setValue(33333);
    connRow->addWidget(spinPort);
    btnConnect = new QPushButton("Подключиться");
    connRow->addWidget(btnConnect);
    connect(btnConnect, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    root->addWidget(gbConn);

    // --- Аккаунт ---
    QGroupBox *gbAcc = new QGroupBox("Аккаунт");
    QFormLayout *accForm = new QFormLayout(gbAcc);
    editLogin = new QLineEdit;
    editPassword = new QLineEdit;
    editPassword->setEchoMode(QLineEdit::Password);
    editEmail = new QLineEdit;
    editEmail->setPlaceholderText("нужен только для регистрации (reg)");
    accForm->addRow("Логин:", editLogin);
    accForm->addRow("Пароль:", editPassword);
    accForm->addRow("Email:", editEmail);

    QHBoxLayout *accBtns = new QHBoxLayout;
    btnReg    = new QPushButton("Зарегистрироваться (reg)");
    btnAuth   = new QPushButton("Войти (auth)");
    btnLogout = new QPushButton("Выйти (logout)");
    btnWhoami = new QPushButton("Кто я? (whoami)");
    accBtns->addWidget(btnReg);
    accBtns->addWidget(btnAuth);
    accBtns->addWidget(btnLogout);
    accBtns->addWidget(btnWhoami);
    accForm->addRow(accBtns);

    lblUser = new QLabel("Вход не выполнен");
    QFont bold = lblUser->font();
    bold.setBold(true);
    lblUser->setFont(bold);
    accForm->addRow(lblUser);

    connect(btnReg,    &QPushButton::clicked, this, &MainWindow::onRegClicked);
    connect(btnAuth,   &QPushButton::clicked, this, &MainWindow::onAuthClicked);
    connect(btnLogout, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    connect(btnWhoami, &QPushButton::clicked, this, &MainWindow::onWhoamiClicked);
    root->addWidget(gbAcc);

    // --- Задание ---
    QGroupBox *gbTask = new QGroupBox("Задание");
    QHBoxLayout *taskRow = new QHBoxLayout(gbTask);
    comboTask = new QComboBox;
    comboTask->addItems({"task1", "task2", "task3", "task4"});
    taskRow->addWidget(comboTask);
    taskRow->addWidget(new QLabel("№ шаблона:"));
    spinTaskNum = new QSpinBox;
    spinTaskNum->setRange(1, 40);
    taskRow->addWidget(spinTaskNum);
    btnGetTask = new QPushButton("Получить задание");
    taskRow->addWidget(btnGetTask);
    btnMystats = new QPushButton("Моя статистика (mystats)");
    taskRow->addWidget(btnMystats);
    taskRow->addStretch();

    // У task2 и task3 по 10 шаблонов, у task1 и task4 — по 40.
    connect(comboTask, &QComboBox::currentTextChanged, this, [this](const QString &t) {
        spinTaskNum->setRange(1, (t == "task2" || t == "task3") ? 10 : 40);
    });
    connect(btnGetTask, &QPushButton::clicked, this, &MainWindow::onGetTaskClicked);
    connect(btnMystats, &QPushButton::clicked, this, &MainWindow::onMystatsClicked);
    root->addWidget(gbTask);

    // --- Ответ ---
    QGroupBox *gbAns = new QGroupBox("Ответ");
    QVBoxLayout *ansCol = new QVBoxLayout(gbAns);
    QHBoxLayout *optRow = new QHBoxLayout;
    optRow->addWidget(new QLabel("task2 / task3 — выберите верный вариант:"));
    optGroup = new QButtonGroup(this);
    for (int i = 1; i <= 4; i++) {
        QRadioButton *r = new QRadioButton(QString("Вариант %1").arg(i));
        optGroup->addButton(r, i);
        optRow->addWidget(r);
    }
    optRow->addStretch();
    ansCol->addLayout(optRow);
    editAnswer = new QLineEdit;
    editAnswer->setPlaceholderText("task1: 1 или 2;   task4: посчитанная сумма");
    ansCol->addWidget(editAnswer);
    btnAnswer = new QPushButton("Отправить ответ");
    ansCol->addWidget(btnAnswer);

    // Пока задание не получено — ответить нельзя
    editAnswer->setEnabled(false);
    setRadiosEnabled(false);
    connect(btnAnswer, &QPushButton::clicked, this, &MainWindow::onAnswerClicked);
    root->addWidget(gbAns);

    // --- Журнал ---
    QGroupBox *gbLog = new QGroupBox("Журнал обмена с сервером");
    QVBoxLayout *logCol = new QVBoxLayout(gbLog);
    logView = new QPlainTextEdit;
    logView->setReadOnly(true);
    QFont mono("Consolas");
    mono.setStyleHint(QFont::Monospace);
    logView->setFont(mono);
    logCol->addWidget(logView);
    root->addWidget(gbLog, 1);
}

// ============================ Соединение ============================

void MainWindow::onConnectClicked()
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
    } else {
        log("клиент", QString("Подключаюсь к %1:%2 ...")
                        .arg(editHost->text()).arg(spinPort->value()));
        socket->connectToHost(editHost->text(), quint16(spinPort->value()));
    }
}

void MainWindow::onConnected()
{
    setConnectionState(true);
    log("клиент", "Соединение установлено, ждём приветствие сервера...");
}

void MainWindow::onDisconnected()
{
    setConnectionState(false);
    loggedIn = false;
    lblUser->setText("Вход не выполнен");
    resetTaskState();
    log("клиент", "Соединение закрыто.");
}

void MainWindow::onSocketErrorOccurred(QAbstractSocket::SocketError error)
{
    // Обрыв соединения — нормальное событие (обрабатывается в onDisconnected)
    if (error == QAbstractSocket::RemoteHostClosedError)
        return;
    log("клиент", "Ошибка сокета: " + socket->errorString()
                + " — запущен ли сервер? (проверь порт и нажми «Подключиться»)");
}

void MainWindow::setConnectionState(bool connected)
{
    btnConnect->setText(connected ? "Отключиться" : "Подключиться");
}

// ============================ Обмен ============================

void MainWindow::sendCommand(const QString &command)
{
    if (socket->state() != QAbstractSocket::ConnectedState) {
        log("клиент", "Нет подключения к серверу.");
        return;
    }
    // Протокол: текст команды + байт-разделитель 0x01
    socket->write(command.toUtf8() + QByteArray(1, '\x01'));
    log("я", command);
}

void MainWindow::log(const QString &who, const QString &text)
{
    QString t = text;
    t.replace("\r\n", "\n");
    logView->appendPlainText(QString("[%1] %2").arg(who, t));
}

void MainWindow::onReadyRead()
{
    QString text = QString::fromUtf8(socket->readAll());
    log("сервер", text);
    QString trimmed = text.trimmed();

    // --- Статус входа ---
    if (trimmed.startsWith("auth+")) {
        loggedIn = true;
        lblUser->setText(QString("Вы вошли: %1 (%2)")
                         .arg(editLogin->text(), trimmed.section(' ', 1, 1)));
    } else if (trimmed.startsWith("auth-") || trimmed.startsWith("logout+")
               || trimmed.startsWith("del+")) {
        loggedIn = false;
        lblUser->setText("Вход не выполнен");
        resetTaskState();
    }

    // --- Пришло задание ---
    if (text.contains("Задание Task2") || text.contains("Задание Task3")) {
        activeTaskType = text.contains("Задание Task2") ? 2 : 3;
        // Для task2/task3 включаем выбор одного из четырёх вариантов.
        setRadiosEnabled(true);
        editAnswer->setEnabled(false);
        // Подставляем реальные значения вариантов из строк
        // "1) x1 = ..." (Task2) или "1) x = ..." (Task3).
        QRegularExpression re("(?m)^([1-4])\\)\\s+(?:x1|x)\\s*=\\s*(\\S+)");
        auto it = re.globalMatch(text);
        while (it.hasNext()) {
            auto m = it.next();
            QAbstractButton *b = optGroup->button(m.captured(1).toInt());
            if (b)
                b->setText(m.captured(0).trimmed());
        }
    } else if (text.contains("Задание Task1") || text.contains("Задание Task4")) {
        activeTaskType = text.contains("Задание Task1") ? 1 : 4;
        // Для task1/task4 ответ вводится числом.
        setRadiosEnabled(false);
        editAnswer->setEnabled(true);
        editAnswer->clear();
        editAnswer->setFocus();
    }

    // --- Ответ на задание обработан, задание закрыто ---
    if (text.contains("Верно!") || text.contains("Неверно!")) {
        resetTaskState();
    }
}

void MainWindow::resetTaskState()
{
    activeTaskType = 0;
    setRadiosEnabled(false);
    editAnswer->setEnabled(false);
    editAnswer->clear();
}

void MainWindow::setRadiosEnabled(bool on)
{
    optGroup->setExclusive(false);          // чтобы можно было снять отметки
    for (QAbstractButton *b : optGroup->buttons()) {
        b->setEnabled(on);
        if (!on) b->setChecked(false);
    }
    optGroup->setExclusive(true);           // выбрать можно только один вариант
}

// ============================ Аккаунт ============================

void MainWindow::onRegClicked()
{
    if (editLogin->text().isEmpty() || editPassword->text().isEmpty()
            || editEmail->text().isEmpty()) {
        log("клиент", "Для регистрации нужны логин, пароль и email.");
        return;
    }
    sendCommand(QString("reg %1 %2 %3")
                .arg(editLogin->text(), editPassword->text(), editEmail->text()));
}

void MainWindow::onAuthClicked()
{
    if (editLogin->text().isEmpty() || editPassword->text().isEmpty()) {
        log("клиент", "Для входа нужны логин и пароль.");
        return;
    }
    sendCommand(QString("auth %1 %2").arg(editLogin->text(), editPassword->text()));
}

void MainWindow::onLogoutClicked()
{
    sendCommand("logout");
}

void MainWindow::onWhoamiClicked()
{
    sendCommand("whoami");
}

// ============================ Задания ============================

void MainWindow::onGetTaskClicked()
{
    sendCommand(comboTask->currentText() + " " + QString::number(spinTaskNum->value()));
}

void MainWindow::onAnswerClicked()
{
    if (activeTaskType == 0) {
        log("клиент", "Сначала получите задание.");
        return;
    }
    if (activeTaskType == 2 || activeTaskType == 3) {
        int id = optGroup->checkedId();
        if (id < 1) {
            log("клиент", "Выберите вариант ответа (1-4).");
            return;
        }
        sendCommand(QString::number(id));
    } else {
        bool ok = false;
        editAnswer->text().trimmed().toInt(&ok);
        if (!ok) {
            log("клиент", "Ответ должен быть целым числом.");
            return;
        }
        sendCommand(editAnswer->text().trimmed());
    }
}

void MainWindow::onMystatsClicked()
{
    sendCommand("mystats");
}
