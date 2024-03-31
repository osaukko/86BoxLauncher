#include "data/machine.h"

#include <QtTest/QTest>

class TestMachine : public QObject
{
    Q_OBJECT
private slots:
    void save_and_restore();
    void extra_varibles_are_kept();
};

void TestMachine::save_and_restore()
{
    Machine a;
    a.setIcon(Machine::IconFromFile, TEST_ICON);
    QCOMPARE(a.iconType(), Machine::IconFromFile);
    a.setName("Test machine");
    a.setSummary("summary");
    a.setConfigFile("config-file");
    a.setSettingsCommand("settings-command");
    a.setStartCommand("start-command");

    Machine b;
    b.restore(a.save());

    Machine c(b.save());

    // All machines should be the same
    const auto testPairs = {std::make_pair(&a, &b), std::make_pair(&b, &c), std::make_pair(&c, &a)};
    for (const auto &testPair : testPairs) {
        auto *a = testPair.first;
        auto *b = testPair.second;
        Q_ASSERT(!a->icon().isNull());
        Q_ASSERT(!b->icon().isNull());
        QCOMPARE(a->iconName(), b->iconName());
        QCOMPARE(a->iconType(), b->iconType());
        QCOMPARE(a->name(), b->name());
        QCOMPARE(a->summary(), b->summary());
        QCOMPARE(a->configFile(), b->configFile());
        QCOMPARE(a->settingsCommand(), b->settingsCommand());
        QCOMPARE(a->startCommand(), b->startCommand());
    }
}

void TestMachine::extra_varibles_are_kept()
{
    QVariantMap customConfig = {{"configFile", "config-file"},
                                {"iconName", TEST_ICON},
                                {"iconType", Machine::IconFromFile},
                                {"name", "Test machine"},
                                {"settingsCommand", "settings-command"},
                                {"startCommand", "start-command"},
                                {"summary", "summary"},
                                {"extra", "should keep this"}};
    Machine machine(customConfig);
    QCOMPARE(customConfig, machine.save());
}

QTEST_GUILESS_MAIN(TestMachine)
#include "test_machine.moc"
