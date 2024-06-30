#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDirIterator>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QString>                    // QLatin1String

#include "qmlprivates.h"

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
    foreach (const QString &argument, parser.positionalArguments())
    {
        QFileInfo pathinfo(argument);
        if (pathinfo.isDir())
        {
            QDirIterator it(pathinfo.absoluteFilePath(),
                            QStringList() << "*.qml",
                            QDir::Files,
                            QDirIterator::Subdirectories);

            while (it.hasNext())
            {
                QFileInfo fileinfo(it.next());

                if (!fileinfo.baseName().startsWith("tst_"))
                {
                    success &= checkQmlFile(fileinfo.absoluteFilePath());
                }
            }
        }
        else if (pathinfo.isFile())
        {
            if (!pathinfo.baseName().startsWith("tst_"))
            {
                success &= checkQmlFile(pathinfo.absoluteFilePath());
            }
        }
    }

    return success ? 0 : -1;
}
