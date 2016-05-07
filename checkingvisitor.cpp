#include "checkingvisitor.h"



/**************************************************************************************************/
CheckingVisitor::CheckingVisitor(const QString &code)
    : QQmlJS::AST::Visitor()
    , m_code(code)
{
    // Start with root context
    m_stack.push(AstContext());
}

CheckingVisitor::~CheckingVisitor()
{
    // Remove root context
    m_stack.pop();
}



/***************************************************************************************************
 * properties e.g. "property int a"
***************************************************************************************************/
bool CheckingVisitor::visit(QQmlJS::AST::UiPublicMember *a_arg)
{
    const QString token(a_arg->name.toString());

    verifyNoFunctionsBeforeProperty(token);

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

    verifyNoBindingsBeforeFunction(token);

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

    verifyNoObjectsBeforeBinding(token);

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

    verifyNoObjectsBeforeBinding(token);

    m_stack.bindings().append(token);
    m_stack.push(AstContext());

    return true;
}

void CheckingVisitor::endVisit(QQmlJS::AST::UiObjectBinding *)
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

void CheckingVisitor::verifyNoObjectsBeforeBinding(const QString &a_token)
{
    static QStringList footers = {"states","assignments"};

    QStringList &objects = m_stack.objects();

    if(!objects.isEmpty() && !footers.contains(a_token))
    {
        QStringList a = QStringList() << "Objects ("
                                      << objects
                                      << ") before binding ("
                                      << a_token
                                      << ")";
        m_warnings.append(a.join(" "));
    }
}



void CheckingVisitor::verifyNoFunctionsBeforeProperty(const QString &a_token)
{
    QStringList &functions = m_stack.functions();

    if(!functions.isEmpty())
    {
        QStringList a = QStringList() << "Functions ("
                                      << functions
                                      << ") before property ("
                                      << a_token
                                      << ")";
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

void CheckingVisitor::verifyNoBindingsBeforeFunction(const QString &a_token)
{
    // Discard the bindings that are 'allowed' before functions
    QStringList bindings = filterAllowedBindings(m_stack.bindings());

    if(!bindings.isEmpty())
    {
        QStringList a = QStringList() << "Bindings ("
                                      << bindings
                                      << ") before function ("
                                      << a_token
                                      << ")";
        m_warnings.append(a.join(" "));
    }
}
