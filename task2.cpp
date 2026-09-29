#include "task2.h"
#include <QRandomGenerator>
#include <functional>
#include <cmath>
#include <QVector>

// f — функция, fp — её точная производная (как функция от x).
// Обе задаются вместе, чтобы гарантировать, что fp — это действительно f'.
struct FuncWithDerivative {
    std::function<double(double)> f;
    std::function<double(double)> fp;
    QString description;
    double x0;
};

// Вспомогательная лямбда для округления double до 4 знаков
static QString fmt4(double v)
{
    return QString::number(v, 'f', 4);
}

// Строит один из 10 шаблонов функции с рандомными параметрами.
// x0 в каждом случае подобран так, чтобы f'(x0) была заведомо
// далека от нуля — иначе шаг Ньютона получится вырожденным
// (деление на почти ноль), и задание перестанет иметь смысл.
static FuncWithDerivative buildFunction(int num)
{
    auto* rng = QRandomGenerator::global();
    FuncWithDerivative r;

    switch (num) {
    // ---- Квадратичные (1, 3) ----
    case 1: {
        double c = rng->bounded(1, 6), d = rng->bounded(1, 8), e = rng->bounded(1, 10);
        r.f  = [c, d, e](double x) { return c * x * x + d * x + e; };
        r.fp = [c, d](double x) { return 2 * c * x + d; };
        r.description = QString("%1*x^2 + %2*x + %3").arg(c).arg(d).arg(e);
        r.x0 = rng->bounded(1, 5);
        break;
    }
    case 3: {
        double c = rng->bounded(1, 6), d = rng->bounded(1, 10);
        r.f  = [c, d](double x) { return c * x * x - d; };
        r.fp = [c](double x) { return 2 * c * x; };
        r.description = QString("%1*x^2 - %2").arg(c).arg(d);
        r.x0 = rng->bounded(1, 5);
        break;
    }

    // ---- Кубические (2, 4) ----
    case 2: {
        double c = rng->bounded(1, 5), d = rng->bounded(1, 10);
        r.f  = [c, d](double x) { return c * x * x * x + d; };
        r.fp = [c](double x) { return 3 * c * x * x; };
        r.description = QString("%1*x^3 + %2").arg(c).arg(d);
        r.x0 = rng->bounded(1, 4);
        break;
    }
    case 4: {
        double c = rng->bounded(1, 5), d = rng->bounded(1, 8);
        r.f  = [c, d](double x) { return c * x * x * x + d * x; };
        r.fp = [c, d](double x) { return 3 * c * x * x + d; };
        r.description = QString("%1*x^3 + %2*x").arg(c).arg(d);
        r.x0 = rng->bounded(1, 4);
        break;
    }

    // ---- Тригонометрия (5-7) ----
    case 5: {
        // x0 в [0, 1) — далеко от pi/2, cos(x0) не близок к нулю
        double c = rng->bounded(1, 8), d = rng->bounded(1, 6);
        r.f  = [c, d](double x) { return c * std::sin(x) - d; };
        r.fp = [c](double x) { return c * std::cos(x); };
        r.description = QString("%1*sin(x) - %2").arg(c).arg(d);
        r.x0 = rng->bounded(0, 100) / 100.0;
        break;
    }
    case 6: {
        // x0 в [1.0, 1.8) — рядом с pi/2, sin(x0) близок к 1, не к нулю
        double c = rng->bounded(1, 8), d = rng->bounded(1, 6);
        r.f  = [c, d](double x) { return c * std::cos(x) - d; };
        r.fp = [c](double x) { return -c * std::sin(x); };
        r.description = QString("%1*cos(x) - %2").arg(c).arg(d);
        r.x0 = 1.0 + rng->bounded(0, 80) / 100.0;
        break;
    }
    case 7: {
        double c = rng->bounded(1, 6), d = rng->bounded(1, 4);
        r.f  = [c, d](double x) { return c * std::sin(d * x); };
        r.fp = [c, d](double x) { return c * d * std::cos(d * x); };
        r.description = QString("%1*sin(%2*x)").arg(c).arg(d);
        // d*x0 держим в [0, 0.9) — далеко от pi/2
        r.x0 = (rng->bounded(0, 30) / 100.0) / d;
        break;
    }

    // ---- Экспоненциальные (8-9) ----
    case 8: {
        // f'(x) = c*e^x всегда положительна — безопасно при любом x0
        double c = rng->bounded(1, 5), d = rng->bounded(1, 8);
        r.f  = [c, d](double x) { return c * std::exp(x) - d; };
        r.fp = [c](double x) { return c * std::exp(x); };
        r.description = QString("%1*e^x - %2").arg(c).arg(d);
        r.x0 = rng->bounded(0, 3);
        break;
    }
    case 9: {
        double c = rng->bounded(1, 4), d = rng->bounded(1, 3), e = rng->bounded(1, 8);
        r.f  = [c, d, e](double x) { return c * std::exp(d * x) - e; };
        r.fp = [c, d](double x) { return c * d * std::exp(d * x); };
        r.description = QString("%1*e^(%2*x) - %3").arg(c).arg(d).arg(e);
        r.x0 = rng->bounded(0, 2);
        break;
    }

    // ---- Линейная (10) — граничный случай, f'(x) = const ----
    case 10:
    default: {
        double c = rng->bounded(1, 8), d = rng->bounded(1, 20);
        r.f  = [c, d](double x) { return c * x - d; };
        r.fp = [c](double) { return static_cast<double>(c); };
        r.description = QString("%1*x - %2").arg(c).arg(d);
        r.x0 = rng->bounded(0, 10);
        break;
    }
    }
    return r;
}

Task2Problem task2_generate(int num)
{
    Task2Problem problem;
    problem.correctAnswer = 0;

    if (num < 1 || num > 10) {
        problem.text = "error: invalid task number (1-10)\r\n";
        problem.correctAnswer = -1;
        return problem;
    }

    auto* rng = QRandomGenerator::global();
    FuncWithDerivative fn = buildFunction(num);

    double x0  = fn.x0;
    double fx0 = fn.f(x0);
    double fpx0 = fn.fp(x0);

    // Страховка: если производная всё же оказалась близка к нулю
    // (не должно случаться при нашем подборе x0, но лучше проверить),
    // сдвигаем x0 на небольшой шаг и пересчитываем.
    if (std::fabs(fpx0) < 0.05) {
        x0 += 0.5;
        fx0 = fn.f(x0);
        fpx0 = fn.fp(x0);
    }

    // Правильный ответ: формула Ньютона x1 = x0 - f(x0)/f'(x0)
    double correctX1 = x0 - fx0 / fpx0;

    // Типичные ошибки при подстановке в формулу Ньютона:
    double wrong1 = x0 + fx0 / fpx0;                 // перепутан знак
    double wrong3 = fx0 / fpx0;                       // забыли вычесть из x0

    double wrong2;
    if (std::fabs(fx0) > 1e-6) {
        wrong2 = x0 - fpx0 / fx0;                      // перепутаны числитель и знаменатель
    } else {
        wrong2 = x0 - fpx0 * fx0;                      // запасной вариант ошибки (умножили вместо деления)
    }

    QVector<double> values = { correctX1, wrong1, wrong2, wrong3 };

    // Гарантируем, что все 4 варианта визуально различимы после округления до 4 знаков
    for (int i = 1; i < values.size(); i++) {
        for (int j = 0; j < i; j++) {
            while (std::fabs(values[i] - values[j]) < 0.01) {
                values[i] += 0.37 + i;
            }
        }
    }

    // Перемешиваем порядок вариантов (Fisher-Yates), запоминая,
    // куда попал правильный ответ (values[0] до перемешивания)
    QVector<int> order = {0, 1, 2, 3};
    for (int i = order.size() - 1; i > 0; i--) {
        int j = rng->bounded(0, i + 1);
        std::swap(order[i], order[j]);
    }

    int correctSlot = 0;
    QString optionsText;
    for (int slot = 0; slot < 4; slot++) {
        int valueIndex = order[slot];
        if (valueIndex == 0)
            correctSlot = slot + 1; // нумерация вариантов с 1
        optionsText += QString("%1) x1 = %2\r\n").arg(slot + 1).arg(fmt4(values[valueIndex]));
    }

    problem.correctAnswer = correctSlot;

    problem.text = QString(
        "=== Задание Task2 (вариант №6 из списка) #%1 ===\r\n"
        "f(x) = %2\r\n"
        "x0 = %3\r\n"
        "f(x0) = %4\r\n"
        "f'(x0) = %5\r\n"
        "Метод Ньютона: x1 = x0 - f(x0)/f'(x0)\r\n"
        "Какое из значений x1 правильное?\r\n"
        "%6"
    ).arg(num)
     .arg(fn.description)
     .arg(fmt4(x0))
     .arg(fmt4(fx0))
     .arg(fmt4(fpx0))
     .arg(optionsText);

    return problem;
}
