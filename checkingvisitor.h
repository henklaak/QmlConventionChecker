#ifndef CHECKINGVISITOR_H
#define CHECKINGVISITOR_H

#include <QStack>

#include <private/qqmljsast_p.h>          //QQmlJS::AST::Node
#include <private/qqmljsastvisitor_p.h>   //QQmlJS::AST::Visitor



class AstContext
{
public:
    QStringList m_properties;
    QStringList m_functions;
    QStringList m_bindings;
    QStringList m_objects;
};



class CheckingVisitor : public QQmlJS::AST::Visitor
{
public:
    explicit CheckingVisitor(const QString &code);
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

    bool visit(QQmlJS::AST::UiObjectDefinition * a_arg) Q_DECL_OVERRIDE;
    void endVisit(QQmlJS::AST::UiObjectDefinition *) Q_DECL_OVERRIDE;

    // CheckingVisitor implementation
    bool hasWarnings() const { return !m_warnings.isEmpty(); }
    QStringList getWarnings() const { return m_warnings; }

private:
    QString getQualifiedId(QQmlJS::AST::UiQualifiedId *a_arg);

    void verifyNoObjectsBeforeBinding(const QString &a_token);
    void verifyNoFunctionsBeforeProperty(const QString &a_token);
    void verifyNoBindingsBeforeFunction(const QString &a_token);

    QString m_code;
    QStack<AstContext> m_stack;
    QStringList m_warnings;
};

#endif // CHECKINGVISITOR_H
