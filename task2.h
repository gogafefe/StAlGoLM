#ifndef TASK2_H
#define TASK2_H

#include <QString>

// text: текст условия (функция, x0, f'(x0), 4 варианта значения x1 на выбор).
// correctAnswer: номер правильного варианта (1-4).
struct Task2Problem {
    QString text;
    int correctAnswer;
};

// Каждый номер — свой шаблон функции f(x) со своими параметрами.
// Параметры рандомизируются при каждом вызове.
// Метод Ньютона: x1 = x0 - f(x0) / f'(x0).
Task2Problem task2_generate(int num);

#endif // TASK2_H
