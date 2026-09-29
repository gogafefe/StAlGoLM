#include <QtTest>
#include <cmath>
#include <QVector>
#include "../task2.h"
// add necessary includes here

// Вспомогательная структура: то, что можно вытащить из текста задания
struct ParsedTask2 {
    double x0 = 0.0;
    double fx0 = 0.0;
    double fpx0 = 0.0;
    QVector<double> options;   // значения x1 из четырёх вариантов
};

// Разбирает текст задания: вытаскивает x0, f(x0), f'(x0) и значения вариантов
static bool parseTask2(const QString& text, ParsedTask2* out)
{
    bool haveX0 = false, haveF = false, haveFp = false;
    const QStringList lines = text.split("\r\n");

    for (const QString& line : lines) {
        bool ok = false;
        if (line.startsWith("x0 = ")) {
            out->x0 = line.mid(5).trimmed().toDouble(&ok);
            haveX0 = ok;
        } else if (line.startsWith("f(x0) = ")) {
            out->fx0 = line.mid(8).trimmed().toDouble(&ok);
            haveF = ok;
        } else if (line.startsWith("f'(x0) = ")) {
            out->fpx0 = line.mid(9).trimmed().toDouble(&ok);
            haveFp = ok;
        } else {
            for (int slot = 1; slot <= 4; slot++) {
                QString prefix = QString("%1) x1 = ").arg(slot);
                if (line.startsWith(prefix)) {
                    double v = line.mid(prefix.length()).trimmed().toDouble(&ok);
                    if (ok && out->options.size() == slot - 1)
                        out->options.append(v);
                }
            }
        }
    }
    return haveX0 && haveF && haveFp && out->options.size() == 4;
}

class Task2_Test : public QObject
{
    Q_OBJECT

public:
    Task2_Test();
    ~Task2_Test();

private slots:
    void invalid_task_number_returns_error();
    void valid_task_number_generates_problem();
    void correct_option_matches_newton_formula();
    void options_are_pairwise_distinct();

};

Task2_Test::Task2_Test()
{

}

Task2_Test::~Task2_Test()
{

}

// Номера вне диапазона 1-10 должны давать ошибку, а не падать
void Task2_Test::invalid_task_number_returns_error()
{
    QVERIFY2(task2_generate(0).text.startsWith("error"),
             "task2_generate(0) must return error text");
    QVERIFY2(task2_generate(0).correctAnswer == -1,
             "task2_generate(0): correctAnswer must be -1");
    QVERIFY2(task2_generate(11).text.startsWith("error"),
             "task2_generate(11) must return error text");
    QVERIFY2(task2_generate(11).correctAnswer == -1,
             "task2_generate(11): correctAnswer must be -1");
}

// Для каждого валидного номера задание сформировано и полно
void Task2_Test::valid_task_number_generates_problem()
{
    for (int num = 1; num <= 10; num++) {
        Task2Problem p = task2_generate(num);
        QString msg = QString("num = %1").arg(num);

        QVERIFY2(!p.text.isEmpty(), qPrintable("empty text, " + msg));
        QVERIFY2(p.text.contains("Метод Ньютона"),
                 qPrintable("text must mention Newton's method, " + msg));
        QVERIFY2(p.correctAnswer >= 1 && p.correctAnswer <= 4,
                 qPrintable("correctAnswer must be 1-4, " + msg));
        for (int slot = 1; slot <= 4; slot++) {
            QVERIFY2(p.text.contains(QString("%1) x1 = ").arg(slot)),
                     qPrintable(QString("missing option %1, ").arg(slot) + msg));
        }
    }
}

// Главный математический тест: вариант, помеченный как правильный,
// действительно совпадает с формулой Ньютона x1 = x0 - f(x0)/f'(x0).
// Генерация случайна, поэтому прогоняем много раз.
void Task2_Test::correct_option_matches_newton_formula()
{
    const int RUNS = 200;
    for (int i = 0; i < RUNS; i++) {
        int num = (i % 10) + 1;
        Task2Problem p = task2_generate(num);

        ParsedTask2 t;
        QVERIFY2(parseTask2(p.text, &t),
                 qPrintable(QString("cannot parse task text, num = %1").arg(num)));

        double expected = t.x0 - t.fx0 / t.fpx0;
        double marked = t.options.value(p.correctAnswer - 1);

        QVERIFY2(std::fabs(marked - expected) < 0.01,
                 qPrintable(QString("num = %1: marked option %2 = %3, "
                                    "but Newton formula gives %4")
                            .arg(num).arg(p.correctAnswer).arg(marked).arg(expected)));
    }
}

// Все 4 варианта ответа должны визуально различаться
void Task2_Test::options_are_pairwise_distinct()
{
    const int RUNS = 200;
    for (int i = 0; i < RUNS; i++) {
        int num = (i % 10) + 1;
        Task2Problem p = task2_generate(num);

        ParsedTask2 t;
        QVERIFY2(parseTask2(p.text, &t),
                 qPrintable(QString("cannot parse task text, num = %1").arg(num)));

        for (int a = 0; a < 4; a++) {
            for (int b = a + 1; b < 4; b++) {
                QVERIFY2(std::fabs(t.options[a] - t.options[b]) > 0.009,
                         qPrintable(QString("num = %1: options %2 and %3 are too close: %4 vs %5")
                                    .arg(num).arg(a + 1).arg(b + 1)
                                    .arg(t.options[a]).arg(t.options[b])));
            }
        }
    }
}

QTEST_APPLESS_MAIN(Task2_Test)

#include "tst_task2_test.moc"
