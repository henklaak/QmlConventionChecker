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

#include "checkingvisitor.h"          // CheckingVisitor
#include "checker.h"


/**************************************************************************************************/
bool checkQmlFile(const QString &a_filename, QStringList *a_warnings)
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

    CheckingVisitor checkingVisitor(a_filename);
    program->accept(&checkingVisitor);

    foreach (const QString &warning, checkingVisitor.getWarnings())
    {
        qInfo() << qPrintable(warning);
    }

    if (a_warnings) {
        *a_warnings = checkingVisitor.getWarnings();
    }

    return !checkingVisitor.hasWarnings();
}
