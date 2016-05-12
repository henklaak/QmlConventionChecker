#include "checkingvisitor.h"



/**************************************************************************************************/
CheckingVisitor::CheckingVisitor(const QString &a_filename)
    : QQmlJS::AST::Visitor()
    , m_filename(a_filename)
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
    return true;
}



/***************************************************************************************************
 * properties e.g. "property int a"
***************************************************************************************************/
bool CheckingVisitor::visit(QQmlJS::AST::UiPublicMember *a_arg)
{
    const QString token(a_arg->name.toString());

    if (!m_stack.top().functions.isEmpty())
    {
        QString warning = getLocationString(a_arg->identifierToken) +
                " error: property " + token +
                " after functions (" + m_stack.top().functions.join(" ") + ")";
        m_warnings.append(warning);
    }

    if (!m_stack.top().bindings.isEmpty())
    {
        QString warning = getLocationString(a_arg->identifierToken) +
                " error: property " + token +
                " after bindings (" + m_stack.top().bindings.join(" ") + ")";
        m_warnings.append(warning);
    }

    if (!m_stack.top().objects.isEmpty())
    {
        QString warning = getLocationString(a_arg->identifierToken) +
                " error: property " + token +
                " after objects (" + m_stack.top().objects.join(" ") + ")";
        m_warnings.append(warning);
    }

    if (m_stack.top().states)
    {
        QString warning = getLocationString(a_arg->identifierToken) +
                " error: property " + token +
                " after binding (states)";
        m_warnings.append(warning);
    }

    if (m_stack.top().transitions)
    {
        QString warning = getLocationString(a_arg->identifierToken) +
                " error: property " + token +
                " after binding (transitions)";
        m_warnings.append(warning);
    }

    m_stack.top().properties.append(token);
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

    if (!m_stack.top().bindings.isEmpty())
    {
        QString warning = getLocationString(funDecl->identifierToken) +
                " error: function " + token +
                " after bindings (" + m_stack.top().bindings.join(" ") + ")";
        m_warnings.append(warning);
    }

    if (!m_stack.top().objects.isEmpty())
    {
        QString warning = getLocationString(funDecl->identifierToken) +
                " error: function " + token +
                " after objects (" + m_stack.top().objects.join(" ") + ")";
        m_warnings.append(warning);
    }

    if (m_stack.top().states)
    {
        QString warning = getLocationString(funDecl->identifierToken) +
                " error: function " + token +
                " after binding (states)";
        m_warnings.append(warning);
    }

    if (m_stack.top().transitions)
    {
        QString warning = getLocationString(funDecl->identifierToken) +
                " error: function " + token +
                " after binding (transitions)";
        m_warnings.append(warning);
    }

    m_stack.top().functions.append(token);
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

    checkBinding(a_arg->qualifiedId->identifierToken, token);

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

    checkBinding(a_arg->qualifiedId->identifierToken, token);

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

    checkBinding(a_arg->qualifiedId->identifierToken, token);

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

    Q_ASSERT(token.length() > 0);

    if (token.at(0).isUpper())
    {
        if (m_stack.top().states)
        {
            QString warning = getLocationString(a_arg->qualifiedTypeNameId->identifierToken) +
                    " error: object " + token +
                    " after binding (states)";
            m_warnings.append(warning);
        }

        if (m_stack.top().transitions)
        {
            QString warning = getLocationString(a_arg->qualifiedTypeNameId->identifierToken) +
                    " error: object " + token +
                    " after binding (transitions)";
            m_warnings.append(warning);
        }

        m_stack.top().objects.append(token);
    }

    m_stack.push(AstContext());

    return true;
}

void CheckingVisitor::endVisit(QQmlJS::AST::UiObjectDefinition *)
{
    m_stack.pop();
}



/**************************************************************************************************/
QString CheckingVisitor::getQualifiedId(QQmlJS::AST::UiQualifiedId *a_arg) const
{
    if (a_arg->next)
    {
        return a_arg->name.toString() + "." + getQualifiedId(a_arg->next);
    }
    return a_arg->name.toString();
}



QString CheckingVisitor::getLocationString(const QQmlJS::AST::SourceLocation &a_location) const
{
    return m_filename +
            ":" + QString::number(a_location.startLine) +
            ":" + QString::number(a_location.startColumn);
}


void CheckingVisitor::checkBinding(const QQmlJS::AST::SourceLocation &a_arg, const QString &token)
{
    if (token == "id")
    {
        if (m_stack.top().objectName)
        {
            QString warning = getLocationString(a_arg) +
                    " error: binding " + token +
                    " after binding (objectName)";
            m_warnings.append(warning);
        }
    }
    if (token == "id" || token == "objectName")
    {
        if (!m_stack.top().properties.isEmpty())
        {
            QString warning = getLocationString(a_arg) +
                    " error: binding " + token +
                    " after properties (" + m_stack.top().properties.join(" ") + ")";
            m_warnings.append(warning);
        }

        if (!m_stack.top().functions.isEmpty())
        {
            QString warning = getLocationString(a_arg) +
                    " error: binding " + token +
                    " after functions (" + m_stack.top().functions.join(" ") + ")";
            m_warnings.append(warning);
        }

        if (!m_stack.top().bindings.isEmpty())
        {
            QString warning = getLocationString(a_arg) +
                    " error: binding " + token +
                    " after bindings (" + m_stack.top().bindings.join(" ") + ")";
            m_warnings.append(warning);
        }
    }

    if (token != "states" && token != "transitions")
    {
        if (!m_stack.top().objects.isEmpty())
        {
            QString warning = getLocationString(a_arg) +
                    " error: binding " + token +
                    " after objects (" + m_stack.top().objects.join(" ") + ")";
            m_warnings.append(warning);
        }

        if (m_stack.top().states)
        {
            QString warning = getLocationString(a_arg) +
                    " error: binding " + token +
                    " after binding (states)";
            m_warnings.append(warning);
        }

        if (m_stack.top().transitions)
        {
            QString warning = getLocationString(a_arg) +
                    " error: binding " + token +
                    " after binding (transitions)";
            m_warnings.append(warning);
        }

    }

    if (token == "states")
    {
        if (m_stack.top().transitions)
        {
            QString warning = getLocationString(a_arg) +
                    " error: binding " + token +
                    " after binding (transitions)";
            m_warnings.append(warning);
        }
    }
    if (token == "transitions")
    {
        // no extra checks
    }

    if (token == "id")
    {
        m_stack.top().id = true;
    }
    else  if (token == "objectName")
    {
        m_stack.top().objectName = true;
    }
    else  if (token == "states")
    {
        m_stack.top().states = true;
    }
    else  if (token == "transitions")
    {
        m_stack.top().transitions = true;
    }
    else
    {
        m_stack.top().bindings.append(token);
    }
}

