#include "checkingvisitor.h"//



/**************************************************************************************************/
CheckingVisitor::CheckingVisitor()
    : QQmlJS::AST::Visitor()
{
    // Start with root context
    m_stack.push(AstContext());
}

CheckingVisitor::~CheckingVisitor()
{
    // Remove root context
    m_stack.pop();
}

bool CheckingVisitor::preVisit(QQmlJS::AST::Node *a_arg)
{
    Q_UNUSED(a_arg);
    //qDebug() << a_arg->kind;
    return true;
}



/***************************************************************************************************
 * properties e.g. "property int a"
***************************************************************************************************/
bool CheckingVisitor::visit(QQmlJS::AST::UiPublicMember *a_arg)
{
    const QString token(a_arg->name.toString());
    //qDebug() << "UiPublicMember" << token;

    verifyPropertyOrder(token, a_arg->identifierToken);

    m_stack.properties().append(token);
    m_stack.push(AstContext());

    return true;
}

void CheckingVisitor::endVisit(QQmlJS::AST::UiPublicMember *)
{
    m_stack.pop();
}



/***************************************************************************************************
 * functions
***************************************************************************************************/
bool CheckingVisitor::visit(QQmlJS::AST::UiSourceElement *a_arg)
{
    QQmlJS::AST::FunctionDeclaration *funDecl =
            dynamic_cast<QQmlJS::AST::FunctionDeclaration *>(a_arg->sourceElement);
    Q_ASSERT(funDecl);

    const QString token(funDecl->name.toString());
    //qDebug() << "UiSourceElement" << token;

    verifyFunctionOrder(token, funDecl->identifierToken);

    m_stack.functions().append(token);
    m_stack.push(AstContext());

    return true;
}

void CheckingVisitor::endVisit(QQmlJS::AST::UiSourceElement *)
{
    m_stack.pop();
}



/***************************************************************************************************
 * assignments / bindings e.g. "height: 200"
***************************************************************************************************/
bool CheckingVisitor::visit(QQmlJS::AST::UiScriptBinding * a_arg)
{
    const QString token(getQualifiedId(a_arg->qualifiedId));
    //qDebug() << "UiScriptBinding" << token;

    verifyBindingOrder(token, a_arg->qualifiedId->identifierToken);

    m_stack.bindings().append(token);
    m_stack.push(AstContext());

    return true;
}

void CheckingVisitor::endVisit(QQmlJS::AST::UiScriptBinding *)
{
    m_stack.pop();
}



/***************************************************************************************************
 * object assignments / bindings
***************************************************************************************************/
bool CheckingVisitor::visit(QQmlJS::AST::UiObjectBinding * a_arg)
{
    const QString token(getQualifiedId(a_arg->qualifiedId));
    //qDebug() << "UiObjectBinding" << token;

    verifyBindingOrder(token, a_arg->qualifiedId->identifierToken);

    m_stack.bindings().append(token);
    m_stack.push(AstContext());

    return true;
}

void CheckingVisitor::endVisit(QQmlJS::AST::UiObjectBinding *)
{
    m_stack.pop();
}



/***************************************************************************************************
 * object assignments / bindings
***************************************************************************************************/
bool CheckingVisitor::visit(QQmlJS::AST::UiArrayBinding * a_arg)
{
    const QString token(getQualifiedId(a_arg->qualifiedId));
    //qDebug() << "UiArrayBinding" << token;

    verifyBindingOrder(token, a_arg->qualifiedId->identifierToken);

    m_stack.bindings().append(token);
    m_stack.push(AstContext());

    return true;
}

void CheckingVisitor::endVisit(QQmlJS::AST::UiArrayBinding *)
{
    m_stack.pop();
}



/***************************************************************************************************
 * objects e.g. "Item { }"
***************************************************************************************************/
bool CheckingVisitor::visit(QQmlJS::AST::UiObjectDefinition * a_arg)
{
    const QString token(getQualifiedId(a_arg->qualifiedTypeNameId));
    //qDebug() << "UiObjectDefinition" << token;

    Q_ASSERT(token.length() > 0);

    if (token.at(0).isUpper())
    {
        m_stack.objects().append(token);
    }

    m_stack.push(AstContext());

    return true;
}

void CheckingVisitor::endVisit(QQmlJS::AST::UiObjectDefinition *)
{
    m_stack.pop();
}



/**************************************************************************************************/
static QStringList filterAllowedBindings(const QStringList &a_input)
{
    static QStringList headers = {"id", "objectName"};

    QStringList output;

    foreach(const QString &input, a_input)
    {
        if (!headers.contains(input))
        {
            output.append(input);
        }
    }

    return output;
}



QString CheckingVisitor::getQualifiedId(QQmlJS::AST::UiQualifiedId *a_arg) const
{
    if (a_arg->next)
    {
        return a_arg->name.toString() + "." + getQualifiedId(a_arg->next);
    }
    return a_arg->name.toString();
}



void CheckingVisitor::verifyPropertyOrder(const QString &a_token,
                                          QQmlJS::AST::SourceLocation &a_location)
{
    const QStringList &functions = m_stack.functions();

    if(!functions.isEmpty())
    {
        QStringList a = QStringList() << "Functions ("
                                      << functions
                                      << ") before property"
                                      << a_token
                                      << "at line"
                                      << QString::number(a_location.startLine);
        m_warnings.append(a.join(" "));
    }

    const QStringList &bindings = filterAllowedBindings(m_stack.bindings());

    if(!bindings.isEmpty())
    {
        QStringList a = QStringList() << "Bindings ("
                                      << bindings
                                      << ") before property"
                                      << a_token
                                      << "at line"
                                      << QString::number(a_location.startLine);
        m_warnings.append(a.join(" "));
    }

    const QStringList &objects = m_stack.objects();

    if(!objects.isEmpty())
    {
        QStringList a = QStringList() << "Objects ("
                                      << objects
                                      << ") before property"
                                      << a_token
                                      << "at line"
                                      << QString::number(a_location.startLine);
        m_warnings.append(a.join(" "));
    }
}



void CheckingVisitor::verifyFunctionOrder(const QString &a_token,
                                          QQmlJS::AST::SourceLocation &a_location)
{
    // Discard the bindings that are 'allowed' before functions
    QStringList bindings = filterAllowedBindings(m_stack.bindings());

    if(!bindings.isEmpty())
    {
        QStringList a = QStringList() << "Bindings ("
                                      << bindings
                                      << ") before function"
                                      << a_token
                                      << "at line"
                                      << QString::number(a_location.startLine);
        m_warnings.append(a.join(" "));
    }

    const QStringList &objects = m_stack.objects();

    if(!objects.isEmpty())
    {
        QStringList a = QStringList() << "Objects ("
                                      << objects
                                      << ") before function"
                                      << a_token
                                      << "at line"
                                      << QString::number(a_location.startLine);
        m_warnings.append(a.join(" "));
    }
}



void CheckingVisitor::verifyBindingOrder(const QString &a_token,
                                         QQmlJS::AST::SourceLocation &a_location)
{
    static QStringList footers = {"states", "transitions"};

    const QStringList &objects = m_stack.objects();

    if(!objects.isEmpty() && !footers.contains(a_token))
    {
        QStringList a = QStringList() << "Objects ("
                                      << objects
                                      << ") before binding"
                                      << a_token
                                      << "at line"
                                      << QString::number(a_location.startLine);
        m_warnings.append(a.join(" "));
    }

}


