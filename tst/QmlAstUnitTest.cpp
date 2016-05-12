#include <QLoggingCategory>
#include <QSignalSpy>
#include <QTest>

#include "checker.h"

/**************************************************************************************************/
class QmlAstUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void tst_valid();
    void tst_invalid1();
    void tst_invalid2();
};

/**************************************************************************************************/
void QmlAstUnitTest::init()
{
}

/**************************************************************************************************/
void QmlAstUnitTest::cleanup()
{
}

/**************************************************************************************************/
void QmlAstUnitTest::tst_valid()
{
    QStringList warnings;
    QVERIFY(checkQmlFile("valid.qml", &warnings));
    QCOMPARE(warnings.count(), 0);
}

/**************************************************************************************************/
void QmlAstUnitTest::tst_invalid1()
{
    QStringList warnings;
    QVERIFY(!checkQmlFile("invalid1.qml", &warnings));
    QCOMPARE(warnings.count(), 1);
}

/**************************************************************************************************/
void QmlAstUnitTest::tst_invalid2()
{
    QStringList warnings;
    QVERIFY(!checkQmlFile("invalid2.qml", &warnings));
    QCOMPARE(warnings.count(), 1);
}

/**************************************************************************************************/
QTEST_MAIN( QmlAstUnitTest )
#include "QmlAstUnitTest.moc"
