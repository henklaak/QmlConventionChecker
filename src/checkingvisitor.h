#ifndef CHECKINGVISITOR_H
#define CHECKINGVISITOR_H

#include <QStack>

#include <qmlprivates.h>


class AstContext
{
public:
    bool id;
    bool objectName;
    QStringList properties;
    QStringList functions;
    QStringList bindings;
    QStringList objects;
    bool states;
    bool transitions;
};



class AstStack : public QStack<AstContext>
{
};



class CheckingVisitor : public QQmlJS::AST::Visitor
{
public:
    explicit CheckingVisitor(const QString &a_filename);
    virtual ~CheckingVisitor();

    // QQmlJS::AST::Visitor implementation
    bool visit(QQmlJS::AST::UiPublicMember * a_arg) Q_DECL_OVERRIDE;
    void endVisit(QQmlJS::AST::UiPublicMember * a_arg) Q_DECL_OVERRIDE;

    bool visit(QQmlJS::AST::UiSourceElement * a_arg) Q_DECL_OVERRIDE;
    void endVisit(QQmlJS::AST::UiSourceElement * a_arg) Q_DECL_OVERRIDE;

    bool visit(QQmlJS::AST::UiScriptBinding * a_arg) Q_DECL_OVERRIDE;
    void endVisit(QQmlJS::AST::UiScriptBinding * a_arg) Q_DECL_OVERRIDE;

    bool visit(QQmlJS::AST::UiObjectBinding * a_arg) Q_DECL_OVERRIDE;
    void endVisit(QQmlJS::AST::UiObjectBinding * a_arg) Q_DECL_OVERRIDE;

    bool visit(QQmlJS::AST::UiArrayBinding * a_arg) Q_DECL_OVERRIDE;
    void endVisit(QQmlJS::AST::UiArrayBinding * a_arg) Q_DECL_OVERRIDE;

    bool visit(QQmlJS::AST::UiObjectDefinition * a_arg) Q_DECL_OVERRIDE;
    void endVisit(QQmlJS::AST::UiObjectDefinition *) Q_DECL_OVERRIDE;

    // CheckingVisitor implementation
    bool hasWarnings() const { return !m_warnings.isEmpty(); }
    QStringList getWarnings() const { return m_warnings; }

private:
    QString getQualifiedId(QQmlJS::AST::UiQualifiedId *a_arg) const;
    QString getLocationString(const QQmlJS::AST::SourceLocation &a_location) const;

    void checkBinding(const QQmlJS::AST::SourceLocation &a_arg, const QString &token);

    AstStack m_stack;

    QString m_filename;
    QStringList m_warnings;
};

#endif // CHECKINGVISITOR_H
