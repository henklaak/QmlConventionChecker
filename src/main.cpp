#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDirIterator>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QString>                    // QLatin1String

#include <5.6.0/QtQml/private/qqmljslexer_p.h>    // QQmlJS::Lexer
#include <5.6.0/QtQml/private/qqmljsengine_p.h>   // QQmlJS::Engine QQmlJS::DiagnosticMessage
#include <5.6.0/QtQml/private/qqmljsparser_p.h>   // QQmlJS::Parser

#include "checker.h"                  // checkQmlFile

/**************************************************************************************************/
int main(int a_argv, char *a_argc[])
{
    QCoreApplication app(a_argv, a_argc);
    QCoreApplication::setApplicationName("QmlAst");
    QCoreApplication::setApplicationVersion("1.0");

    // Process cmdline
    QCommandLineParser parser;
    parser.setApplicationDescription(QLatin1String("QML syntax verifier"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QLatin1String("path"), QLatin1String("list of qml or js files to verify"));
    parser.process(app);

    if (parser.positionalArguments().isEmpty())
    {
        parser.showHelp(-1);
    }

    // Do work
    bool success = true;
    foreach (const QString &dirname, parser.positionalArguments())
    {
        QDirIterator it(dirname, QStringList() << "*.qml", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            QString filename = it.next();
            QFileInfo info(filename);
            if (!info.baseName().startsWith("tst_"))
            {
                success &= checkQmlFile(filename);
            }
        }
    }

    return success ? 0 : -1;
}
