#include "task3.h"
#include <QRandomGenerator>
#include <QVector>
#include <functional>
#include <cmath>
#include <algorithm>

struct BisectionTemplate {
    std::function<double(double)> f;
    QString description;
    double a;
    double b;
};

struct BisectionResult {
    double root;
    int iterations;
};

static QString fmt4(double value)
{
    return QString::number(value, 'f', 4);
}

// Выполняет классический метод половинного деления. До вызова должно быть
// выполнено условие f(a) * f(b) <= 0.
static BisectionResult solveBisection(const std::function<double(double)>& f,
                                     double a, double b, double epsilon)
{
    double fa = f(a);
    int iterations = 0;

    while ((b - a) / 2.0 > epsilon && iterations < 200) {
        double middle = (a + b) / 2.0;
        double fm = f(middle);
        iterations++;

        if (std::fabs(fm) < 1e-12) {
            return {middle, iterations};
        }

        if (fa * fm <= 0.0) {
            b = middle;
        } else {
            a = middle;
            fa = fm;
        }
    }

    return {(a + b) / 2.0, iterations};
}

// Десять шаблонов непрерывных функций. Границы выбираются так, чтобы
// значения функции на концах имели разные знаки.
static BisectionTemplate buildTemplate(int num)
{
    auto* rng = QRandomGenerator::global();
    BisectionTemplate t;

    switch (num) {
    case 1: {
        double c = rng->bounded(1, 8);
        double d = rng->bounded(1, 15);
        double exactRoot = d / c;
        double width = rng->bounded(1, 4);
        t.f = [c, d](double x) { return c * x - d; };
        t.description = QString("%1*x - %2").arg(c).arg(d);
        t.a = exactRoot - width;
        t.b = exactRoot + width;
        break;
    }
    case 2: {
        double c = rng->bounded(2, 20);
        t.f = [c](double x) { return x * x - c; };
        t.description = QString("x^2 - %1").arg(c);
        t.a = 0.0;
        t.b = c + 1.0;
        break;
    }
    case 3: {
        double c = rng->bounded(2, 15);
        t.f = [c](double x) { return x * x * x - c; };
        t.description = QString("x^3 - %1").arg(c);
        t.a = 0.0;
        t.b = c + 1.0;
        break;
    }
    case 4: {
        double k = rng->bounded(1, 9) / 10.0;
        t.f = [k](double x) { return std::sin(x) - k; };
        t.description = QString("sin(x) - %1").arg(fmt4(k));
        t.a = 0.0;
        t.b = 1.5707963267948966;
        break;
    }
    case 5: {
        double k = rng->bounded(1, 9) / 10.0;
        t.f = [k](double x) { return std::cos(x) - k; };
        t.description = QString("cos(x) - %1").arg(fmt4(k));
        t.a = 0.0;
        t.b = 1.5707963267948966;
        break;
    }
    case 6: {
        double c = rng->bounded(2, 9);
        t.f = [c](double x) { return std::exp(x) - c; };
        t.description = QString("e^x - %1").arg(c);
        t.a = 0.0;
        t.b = std::log(c) + 1.0;
        break;
    }
    case 7: {
        double c = rng->bounded(1, 11);
        t.f = [c](double x) { return x * x * x + x - c; };
        t.description = QString("x^3 + x - %1").arg(c);
        t.a = 0.0;
        t.b = c;
        break;
    }
    case 8: {
        double c = rng->bounded(1, 21);
        t.f = [c](double x) { return x * x + x - c; };
        t.description = QString("x^2 + x - %1").arg(c);
        t.a = 0.0;
        t.b = c;
        break;
    }
    case 9: {
        double c = rng->bounded(1, 4);
        t.f = [c](double x) { return std::log(x) - c; };
        t.description = QString("ln(x) - %1").arg(c);
        t.a = 0.5;
        t.b = std::exp(c) + 1.0;
        break;
    }
    case 10:
    default: {
        double c = rng->bounded(2, 11);
        t.f = [c](double x) { return std::pow(x, 5) - c; };
        t.description = QString("x^5 - %1").arg(c);
        t.a = 0.0;
        t.b = c + 1.0;
        break;
    }
    }

    return t;
}

Task3Problem task3_generate(int num)
{
    Task3Problem problem;
    problem.correctAnswer = 0;

    if (num < 1 || num > 10) {
        problem.text = "error: invalid task number (1-10)\r\n";
        problem.correctAnswer = -1;
        return problem;
    }

    auto* rng = QRandomGenerator::global();
    BisectionTemplate t = buildTemplate(num);
    const QVector<double> epsilons = {0.01, 0.005, 0.001};
    double epsilon = epsilons[rng->bounded(epsilons.size())];

    if (t.f(t.a) * t.f(t.b) > 0.0) {
        problem.text = "error: function does not change sign on [a, b]\r\n";
        problem.correctAnswer = -1;
        return problem;
    }

    BisectionResult result = solveBisection(t.f, t.a, t.b, epsilon);

    // Три заведомо неверных значения находятся дальше epsilon от корня.
    // Минимальный шаг 0.02 гарантирует визуальное различие при выводе 4 знаков.
    double offset = std::max(4.0 * epsilon, 0.02);
    QVector<double> values = {
        result.root,
        result.root + offset,
        result.root - offset,
        result.root + 2.0 * offset
    };

    QVector<int> order = {0, 1, 2, 3};
    for (int i = order.size() - 1; i > 0; --i) {
        int j = rng->bounded(0, i + 1);
        std::swap(order[i], order[j]);
    }

    int correctSlot = 0;
    QString optionsText;
    for (int slot = 0; slot < 4; ++slot) {
        int valueIndex = order[slot];
        if (valueIndex == 0)
            correctSlot = slot + 1;
        optionsText += QString("%1) x = %2\r\n")
                           .arg(slot + 1)
                           .arg(fmt4(values[valueIndex]));
    }

    problem.text = QString(
        "=== Задание Task3 (вариант №3 из списка) #%1 ===\r\n"
        "f(x) = %2\r\n"
        "Отрезок: [%3, %4]\r\n"
        "Точность epsilon = %5\r\n"
        "Метод половинного деления: на каждом шаге выбирайте половину отрезка, где функция меняет знак.\r\n"
        "Какое приближённое значение корня получено?\r\n"
        "%6"
    ).arg(num)
     .arg(t.description)
     .arg(fmt4(t.a))
     .arg(fmt4(t.b))
     .arg(QString::number(epsilon, 'f', 4))
     .arg(optionsText);

    problem.correctAnswer = correctSlot;
    return problem;
}
