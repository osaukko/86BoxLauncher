#include "formatter.h"

#include <QtTest/QTest>

class TestFormatter : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void invalidInput_data();
    void invalidInput();

    void validInput_data();
    void validInput();

private:
    QHash<QString, QString> variables;
};

void TestFormatter::initTestCase()
{
    variables["test"] = "hello";
    variables["foo"] = "bar";
}

void TestFormatter::invalidInput_data()
{
    QTest::addColumn<QString>("input");
    QTest::addRow("missing {") << "test}";
    QTest::addRow("missing { [alt]") << "{test}}";
    QTest::addRow("missing }") << "{test";
    QTest::addRow("missing } [alt]") << "{{test}";
    QTest::addRow("empty {}") << "{}";
    QTest::addRow("single {") << "{";
    QTest::addRow("single }") << "}";
}

void TestFormatter::invalidInput()
{
    QFETCH(QString, input);

    Formatter formatter;
    QVERIFY(!formatter.setInput(input));

    bool ok;
    auto result = Formatter::format(input, {}, &ok);
    QVERIFY(!ok);
    QCOMPARE(result, QString{});
}

void TestFormatter::validInput_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expected");
    QTest::addRow("empty is okay") << ""
                                   << "";
    QTest::addRow("nothing to format is okay") << "test"
                                               << "test";
    QTest::addRow("{{ and }} should be replaced with { }") << "{{test}}"
                                                           << "{test}";
    QTest::addRow("{test} = hello") << "{test}"
                                    << "hello";
    QTest::addRow("{{test}} = {test}") << "{{test}}"
                                       << "{test}";
    QTest::addRow("{{{test}}} = {hello}") << "{{{test}}}"
                                          << "{hello}";
    QTest::addRow("{{{{test}}}} = {{test}}") << "{{{{test}}}}"
                                             << "{{test}}";
    QTest::addRow("{{{{{test}}}}} = {{hello}}") << "{{{{{test}}}}}"
                                                << "{{hello}}";
    QTest::addRow("{test}{test} = hellohello") << "{test}{test}"
                                               << "hellohello";
    QTest::addRow("staring with literals") << "test {test}"
                                           << "test hello";
    QTest::addRow("ending with literals") << "{test}, world"
                                          << "hello, world";
    QTest::addRow("missing variable is empty") << "{bar}"
                                               << "";
    QTest::addRow("multiple literals and variables") << "test: {test}, foo: {foo}"
                                                     << "test: hello, foo: bar";
    QTest::addRow("multiple literals and variables [alt]") << "{{test}}: {test}, {{foo}}: {foo}"
                                                           << "{test}: hello, {foo}: bar";
}

void TestFormatter::validInput()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);

    Formatter formatter;
    QVERIFY(formatter.setInput(input));
    QCOMPARE(formatter.format(variables), expected);

    bool ok;
    auto result = Formatter::format(input, variables, &ok);
    QVERIFY(ok);
    QCOMPARE(result, expected);
}

QTEST_GUILESS_MAIN(TestFormatter)
#include "test_formatter.moc"
