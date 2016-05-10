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

    verifyNoFunctionsBeforeProperty(token, a_arg->identifierToken);

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

    verifyNoBindingsBeforeFunction(token, funDecl->identifierToken);

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

    verifyNoObjectsBeforeBinding(token, a_arg->qualifiedId->identifierToken);

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

    verifyNoObjectsBeforeBinding(token, a_arg->qualifiedId->identifierToken);

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

    verifyNoObjectsBeforeBinding(token, a_arg->qualifiedId->identifierToken);

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
QString CheckingVisitor::getQualifiedId(QQmlJS::AST::UiQualifiedId *a_arg)
{
    if (a_arg->next)
    {
        return a_arg->name.toString() + "." + getQualifiedId(a_arg->next);
    }
    return a_arg->name.toString();
}

void CheckingVisitor::verifyNoObjectsBeforeBinding(const QString &a_token,
                                                   QQmlJS::AST::SourceLocation &a_location)
{
    static QStringList footers = {"states","transitions"};

    QStringList &objects = m_stack.objects();

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



void CheckingVisitor::verifyNoFunctionsBeforeProperty(const QString &a_token,
                                                      QQmlJS::AST::SourceLocation &a_location)
{
    QStringList &functions = m_stack.functions();

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
}



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

void CheckingVisitor::verifyNoBindingsBeforeFunction(const QString &a_token,
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
}
