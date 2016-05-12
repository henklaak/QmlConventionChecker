#ifndef CHECKER_H
#define CHECKER_H

#include <QString>

bool checkQmlFile(const QString &a_filename, QStringList *a_warnings = nullptr);

#endif
