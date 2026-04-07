#include "Ast.h"
#include "AstVisitor.h"

// 수용부 구현체
void ColumnExpression::accept(AstVisitor& v) { v.visit(*this); }
void LiteralExpression::accept(AstVisitor& v) { v.visit(*this); }
void BinaryExpression::accept(AstVisitor& v) { v.visit(*this); }
void UnaryExpression::accept(AstVisitor& v) { v.visit(*this); }
void SelectStatement::accept(AstVisitor& v) { v.visit(*this); }
void CreateStatement::accept(AstVisitor& v) { v.visit(*this); }
void InsertStatement::accept(AstVisitor& v) { v.visit(*this); }