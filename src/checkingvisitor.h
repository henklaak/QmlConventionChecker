#ifndef CHECKINGVISITOR_H
#define CHECKINGVISITOR_H

#include <QStack>

#include <5.6.0/QtQml/private/qqmljsast_p.h>          //QQmlJS::AST::Node
#include <5.6.0/QtQml/private/qqmljsastvisitor_p.h>   //QQmlJS::AST::Visitor



class AstContext
{
private:
    QStringList m_properties;
    QStringList m_functions;
    QStringList m_bindings;
    QStringList m_objects;

    friend class AstStack;
};



class AstStack : public QStack<AstContext>
{
public:
    QStringList &properties() { return top().m_properties; }
    QStringList &functions() { return top().m_functions; }
    QStringList &bindings() { return top().m_bindings; }
    QStringList &objects() { return top().m_objects; }
};



class CheckingVisitor : public QQmlJS::AST::Visitor
{
public:
    explicit CheckingVisitor();
    virtual ~CheckingVisitor();

    // QQmlJS::AST::Visitor implementation
    bool preVisit(QQmlJS::AST::Node *) Q_DECL_OVERRIDE;

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

    void verifyPropertyOrder(const QString &a_token, QQmlJS::AST::SourceLocation &a_location);
    void verifyFunctionOrder(const QString &a_token, QQmlJS::AST::SourceLocation &a_location);
    void verifyBindingOrder(const QString &a_token, QQmlJS::AST::SourceLocation &a_location);

    AstStack m_stack;

    QStringList m_warnings;
};

#endif // CHECKINGVISITOR_H
