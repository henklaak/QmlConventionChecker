#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDirIterator>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QString>                    // QLatin1String

#include <private/qqmljslexer_p.h>    // QQmlJS::Lexer
#include <private/qqmljsengine_p.h>   // QQmlJS::Engine QQmlJS::DiagnosticMessage
#include <private/qqmljsparser_p.h>   // QQmlJS::Parser

#include "checkingvisitor.h"          // CheckingVisitor


static bool checkQmlFile(const QString &a_filename)
{
    // Load the code
    QFile file(a_filename);
    if (!file.open(QFile::ReadOnly))
    {
        qWarning() << "Failed to open file" << a_filename << file.error();
        return false;
    }

    QString code = QString::fromUtf8(file.readAll());
    file.close();

    // Parse the code
    QQmlJS::Engine engine;
    QQmlJS::Lexer lexer(&engine);
    QQmlJS::Parser parser(&engine);

    QFileInfo info(a_filename);
    bool isJavaScript = info.suffix().toLower() == QLatin1String("js");
    lexer.setCode(code,  1, !isJavaScript);

    bool success = isJavaScript ? parser.parseProgram() : parser.parse();

    if (!success)
    {
        foreach (const QQmlJS::DiagnosticMessage &m, parser.diagnosticMessages())
        {
            qWarning("%s:%d : %s", qPrintable(a_filename), m.loc.startLine, qPrintable(m.message));
        }
    }

    // Check the code
    QQmlJS::AST::UiProgram *program = parser.ast();

    CheckingVisitor checkingVisitor(code);
    program->accept(&checkingVisitor);

    if (checkingVisitor.hasWarnings())
    {
        qInfo() << a_filename.toLatin1().data();

        foreach (const QString &warning, checkingVisitor.getWarnings())
        {
            qInfo() << "   " << warning.toLatin1().data();
        }
        qInfo() << "";
    }

    return !checkingVisitor.hasWarnings();
}



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
