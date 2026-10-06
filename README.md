# StAlGoLM

Клиент-серверное приложение на C++/Qt: TCP-сервер (порт 33333) с аккаунтами,
ролями (user / moderator / admin) и учебными заданиями по математике со
статистикой ответов в SQLite.

## Сборка

    qmake StAlGoLM.pro
    make          # или сборка в Qt Creator (кнопка ▶)

## Структура

    main.cpp              — точка входа
    mytcpserver.*         — приём подключений, разбор сообщений
    server_functions.*    — диспетчер команд (reg/auth/taskN/...)
    databasemanager.*     — SQLite: пользователи, роли, статистика
    task1..task4.*        — генераторы заданий
    tests/                — модульные тесты Task2 и Task3 (QtTest)
    wiki/                 — страницы Wiki проекта

Подробнее — в wiki проекта и в `wiki/Архитектура.md`.
