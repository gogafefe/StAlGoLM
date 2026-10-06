#ifndef TASK4_H
#define TASK4_H

#include <QString>

/**
 * @file task4.h
 * @brief Заглушка задания четвёртого участника команды.
 * @details Task4 является заданием-заглушкой для четвёртого студента —
 * Иванова Алексея, который был отчислен и не реализовал своё задание.
 */

/** @brief Условие Task4 и правильная сумма двух чисел. */
struct Task4Problem {
    QString text;
    int correctAnswer;
};

/**
 * @brief Создаёт задачу на сложение двух чисел.
 * @param num Номер шаблона от 1 до 40.
 */
Task4Problem task4_generate(int num);

#endif // TASK4_H
