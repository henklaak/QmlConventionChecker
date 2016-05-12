#include <QString>
#include <QTest>

#include "checker.h"

/**************************************************************************************************/
class QmlAstUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void tst_file_data();
    void tst_file();
};

/**************************************************************************************************/
void QmlAstUnitTest::tst_file_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<bool>("result");
    QTest::addColumn<int>("nrwarnings");

    QTest::newRow("valid") << "valid.qml" << true << 0;
    QTest::newRow("invalid1") << "invalid1.qml" << false << 1;
    QTest::newRow("invalid2") << "invalid2.qml" << false << 1;
    QTest::newRow("invalid3") << "invalid3.qml" << false << 1;
    QTest::newRow("invalid4") << "invalid4.qml" << false << 1;
    QTest::newRow("invalid5") << "invalid5.qml" << false << 1;
    QTest::newRow("invalid6") << "invalid6.qml" << false << 1;
    QTest::newRow("invalid7") << "invalid7.qml" << false << 34; // 1+2+3...+7 + 3 + 3
}

/**************************************************************************************************/
void QmlAstUnitTest::tst_file()
{
    QFETCH(QString, filename);
    QFETCH(bool, result);
    QFETCH(int, nrwarnings);

    QStringList warnings;
    QCOMPARE(checkQmlFile(filename, &warnings), result);
    QCOMPARE(warnings.count(), nrwarnings);
}

/**************************************************************************************************/
QTEST_MAIN( QmlAstUnitTest )
#include "QmlAstUnitTest.moc"
