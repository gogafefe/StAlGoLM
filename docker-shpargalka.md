Для сборки сервера (Всё в корневой папке):
docker load -i .\stalgolm-server-image.tar

Для пересохранения докер файла:
docker load -i stalgolm-server-image.tar

Запуск сервера:
docker compose up -d --no-build

Выключение сервера:
docker compose down

Создание нового докер файла:
docker compose up --build -d