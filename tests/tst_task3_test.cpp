#include <QtTest>
#include <QRegularExpression>
#include <QVector>
#include <cmath>
#include <functional>
#include "../task3.h"

struct ParsedTask3 {
    QString functionText;
    double a = 0.0;
    double b = 0.0;
    double epsilon = 0.0;
    QVector<double> options;
};

static bool parseTask3(const QString& text, ParsedTask3* out)
{
    QString normalized = text;
    normalized.replace("\r\n", "\n");

    QRegularExpression functionRe("(?m)^f\\(x\\) = (.+)$");
    QRegularExpression intervalRe(
        "(?m)^Отрезок: \\[([-+0-9.]+), ([-+0-9.]+)\\]$");
    QRegularExpression epsilonRe("(?m)^Точность epsilon = ([0-9.]+)$");
    QRegularExpression optionRe("(?m)^([1-4])\\) x = ([-+0-9.]+)$");

    auto functionMatch = functionRe.match(normalized);
    auto intervalMatch = intervalRe.match(normalized);
    auto epsilonMatch = epsilonRe.match(normalized);
    if (!functionMatch.hasMatch() || !intervalMatch.hasMatch()
            || !epsilonMatch.hasMatch()) {
        return false;
    }

    bool okA = false, okB = false, okEpsilon = false;
    out->functionText = functionMatch.captured(1).trimmed();
    out->a = intervalMatch.captured(1).toDouble(&okA);
    out->b = intervalMatch.captured(2).toDouble(&okB);
    out->epsilon = epsilonMatch.captured(1).toDouble(&okEpsilon);

    auto it = optionRe.globalMatch(normalized);
    while (it.hasNext()) {
        auto match = it.next();
        bool ok = false;
        double value = match.captured(2).toDouble(&ok);
        if (!ok || match.captured(1).toInt() != out->options.size() + 1)
            return false;
        out->options.append(value);
    }

    return okA && okB && okEpsilon && out->options.size() == 4;
}

static std::function<double(double)> parseFunction(const QString& text)
{
    const QString number = "([-+0-9.]+)";
    QRegularExpressionMatch match;

    match = QRegularExpression("^" + number + "\\*x - " + number + "$").match(text);
    if (match.hasMatch()) {
        double c = match.captured(1).toDouble();
        double d = match.captured(2).toDouble();
        return [c, d](double x) { return c * x - d; };
    }

    match = QRegularExpression("^x\\^2 - " + number + "$").match(text);
    if (match.hasMatch()) {
        double c = match.captured(1).toDouble();
        return [c](double x) { return x * x - c; };
    }

    match = QRegularExpression("^x\\^3 - " + number + "$").match(text);
    if (match.hasMatch()) {
        double c = match.captured(1).toDouble();
        return [c](double x) { return x * x * x - c; };
    }

    match = QRegularExpression("^sin\\(x\\) - " + number + "$").match(text);
    if (match.hasMatch()) {
        double k = match.captured(1).toDouble();
        return [k](double x) { return std::sin(x) - k; };
    }

    match = QRegularExpression("^cos\\(x\\) - " + number + "$").match(text);
    if (match.hasMatch()) {
        double k = match.captured(1).toDouble();
        return [k](double x) { return std::cos(x) - k; };
    }

    match = QRegularExpression("^e\\^x - " + number + "$").match(text);
    if (match.hasMatch()) {
        double c = match.captured(1).toDouble();
        return [c](double x) { return std::exp(x) - c; };
    }

    match = QRegularExpression("^x\\^3 \\+ x - " + number + "$").match(text);
    if (match.hasMatch()) {
        double c = match.captured(1).toDouble();
        return [c](double x) { return x * x * x + x - c; };
    }

    match = QRegularExpression("^x\\^2 \\+ x - " + number + "$").match(text);
    if (match.hasMatch()) {
        double c = match.captured(1).toDouble();
        return [c](double x) { return x * x + x - c; };
    }

    match = QRegularExpression("^ln\\(x\\) - " + number + "$").match(text);
    if (match.hasMatch()) {
        double c = match.captured(1).toDouble();
        return [c](double x) { return std::log(x) - c; };
    }

    match = QRegularExpression("^x\\^5 - " + number + "$").match(text);
    if (match.hasMatch()) {
        double c = match.captured(1).toDouble();
        return [c](double x) { return std::pow(x, 5) - c; };
    }

    return {};
}

static double highPrecisionRoot(const std::function<double(double)>& f,
                                double a, double b)
{
    double fa = f(a);
    for (int i = 0; i < 100; ++i) {
        double middle = (a + b) / 2.0;
        double fm = f(middle);
        if (fa * fm <= 0.0) {
            b = middle;
        } else {
            a = middle;
            fa = fm;
        }
    }
    return (a + b) / 2.0;
}

class Task3Test : public QObject
{
    Q_OBJECT

private slots:
    void invalidNumberReturnsError();
    void validTemplatesHaveFourOptions();
    void markedOptionHasRequiredAccuracy();
    void optionsArePairwiseDistinct();
};

void Task3Test::invalidNumberReturnsError()
{
    QCOMPARE(task3_generate(0).correctAnswer, -1);
    QVERIFY(task3_generate(0).text.startsWith("error"));
    QCOMPARE(task3_generate(11).correctAnswer, -1);
    QVERIFY(task3_generate(11).text.startsWith("error"));
}

void Task3Test::validTemplatesHaveFourOptions()
{
    for (int num = 1; num <= 10; ++num) {
        Task3Problem problem = task3_generate(num);
        ParsedTask3 parsed;
        QVERIFY2(parseTask3(problem.text, &parsed), qPrintable(problem.text));
        QVERIFY(problem.correctAnswer >= 1 && problem.correctAnswer <= 4);
        QVERIFY(problem.text.contains("метод половинного деления", Qt::CaseInsensitive));
    }
}

void Task3Test::markedOptionHasRequiredAccuracy()
{
    const int runs = 300;
    for (int i = 0; i < runs; ++i) {
        int num = i % 10 + 1;
        Task3Problem problem = task3_generate(num);
        ParsedTask3 parsed;
        QVERIFY2(parseTask3(problem.text, &parsed), qPrintable(problem.text));

        auto f = parseFunction(parsed.functionText);
        QVERIFY2(bool(f), qPrintable("Unknown function: " + parsed.functionText));
        QVERIFY(f(parsed.a) * f(parsed.b) <= 0.0);

        double root = highPrecisionRoot(f, parsed.a, parsed.b);
        double marked = parsed.options.at(problem.correctAnswer - 1);
        QVERIFY2(std::fabs(marked - root) <= parsed.epsilon + 0.0002,
                 qPrintable(QString("template %1: marked=%2 root=%3 epsilon=%4")
                            .arg(num).arg(marked).arg(root).arg(parsed.epsilon)));
    }
}

void Task3Test::optionsArePairwiseDistinct()
{
    const int runs = 200;
    for (int i = 0; i < runs; ++i) {
        Task3Problem problem = task3_generate(i % 10 + 1);
        ParsedTask3 parsed;
        QVERIFY(parseTask3(problem.text, &parsed));
        for (int a = 0; a < parsed.options.size(); ++a) {
            for (int b = a + 1; b < parsed.options.size(); ++b) {
                QVERIFY(std::fabs(parsed.options[a] - parsed.options[b]) >= 0.009);
            }
        }
    }
}

QTEST_APPLESS_MAIN(Task3Test)

#include "tst_task3_test.moc"
