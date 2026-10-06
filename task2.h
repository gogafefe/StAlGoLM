#ifndef TASK2_H
#define TASK2_H

#include <QString>

/** @brief Условие Task2 и правильный вариант первой итерации Ньютона. */
struct Task2Problem {
    QString text;
    int correctAnswer;
};

// Каждый номер — свой шаблон функции f(x) со своими параметрами.
// Параметры рандомизируются при каждом вызове.
// Метод Ньютона: x1 = x0 - f(x0) / f'(x0).
/**
 * @brief Создаёт задачу на вычисление первой итерации метода Ньютона.
 * @param num Номер шаблона от 1 до 10.
 */
Task2Problem task2_generate(int num);

#endif // TASK2_H
