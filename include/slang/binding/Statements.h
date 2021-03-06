//------------------------------------------------------------------------------
// Statements.h
// Statement creation and analysis.
//
// File is under the MIT license; see LICENSE for details.
//------------------------------------------------------------------------------
#pragma once

#include "slang/binding/EvalContext.h"
#include "slang/symbols/HierarchySymbols.h"
#include "slang/symbols/MemberSymbols.h"

namespace slang {

enum class StatementKind {
    Invalid,
    List,
    SequentialBlock,
    ExpressionStatement,
    VariableDeclaration,
    Return,
    Conditional,
    ForLoop
};

/// The base class for all statements in SystemVerilog.
class Statement {
public:
    /// The kind of statement; indicates the type of derived class.
    StatementKind kind;

    /// The syntax used to create this statement, if it was parsed from a source file.
    const StatementSyntax* syntax;

    /// Indicates whether the statement is invalid.
    bool bad() const { return kind == StatementKind::Invalid; }

    /// Evaluates the statement under the given evaluation context.
    bool eval(EvalContext& context) const;

    template<typename T>
    T& as() {
        ASSERT(T::isKind(kind));
        return *static_cast<T*>(this);
    }

    template<typename T>
    const T& as() const {
        ASSERT(T::isKind(kind));
        return *static_cast<const T*>(this);
    }

    template<typename TVisitor, typename... Args>
    decltype(auto) visit(TVisitor& visitor, Args&&... args) const;

protected:
    explicit Statement(StatementKind kind) : kind(kind), syntax(nullptr) {}
    Statement(StatementKind kind, const StatementSyntax& syntax) : kind(kind), syntax(&syntax) {}
};

/// Represents an invalid statement, which is usually generated and inserted
/// into a statement list due to violation of language semantics or type checking.
class InvalidStatement : public Statement {
public:
    /// A wrapped sub-statement that is considered invalid.
    const Statement* child;

    explicit InvalidStatement(const Statement* child) :
        Statement(StatementKind::Invalid), child(child) {}

    static bool isKind(StatementKind kind) { return kind == StatementKind::Invalid; }

    static const InvalidStatement Instance;
};

/// Represents a list of statements.
class StatementList : public Statement {
public:
    span<const Statement* const> list;

    StatementList(span<const Statement* const> list) : Statement(StatementKind::List), list(list) {}

    bool eval(EvalContext& context) const;

    static bool isKind(StatementKind kind) { return kind == StatementKind::List; }
    static const StatementList Empty;
};

/// Represents a sequential block statement.
class SequentialBlockStatement : public Statement {
public:
    const SequentialBlockSymbol& block;

    SequentialBlockStatement(const SequentialBlockSymbol& block) :
        Statement(StatementKind::SequentialBlock), block(block) {}

    bool eval(EvalContext& context) const;

    static bool isKind(StatementKind kind) { return kind == StatementKind::SequentialBlock; }
};

class ReturnStatement : public Statement {
public:
    const Expression* expr;

    ReturnStatement(const StatementSyntax& syntax, const Expression* expr) :
        Statement(StatementKind::Return, syntax), expr(expr) {}

    bool eval(EvalContext& context) const;

    static bool isKind(StatementKind kind) { return kind == StatementKind::Return; }
};

class VariableDeclStatement : public Statement {
public:
    const VariableSymbol& symbol;

    VariableDeclStatement(const VariableSymbol& symbol) :
        Statement(StatementKind::VariableDeclaration), symbol(symbol) {}

    bool eval(EvalContext& context) const;

    static bool isKind(StatementKind kind) { return kind == StatementKind::VariableDeclaration; }
};

class ConditionalStatement : public Statement {
public:
    const Expression& cond;
    const Statement& ifTrue;
    const Statement* ifFalse;

    ConditionalStatement(const StatementSyntax& syntax, const Expression& cond,
                         const Statement& ifTrue, const Statement* ifFalse) :
        Statement(StatementKind::Conditional, syntax),
        cond(cond), ifTrue(ifTrue), ifFalse(ifFalse) {}

    bool eval(EvalContext& context) const;

    static bool isKind(StatementKind kind) { return kind == StatementKind::Conditional; }
};

class ForLoopStatement : public Statement {
public:
    const StatementList& initializers;
    const Expression* stopExpr;
    span<const Expression* const> steps;
    const Statement& body;

    ForLoopStatement(const StatementSyntax& syntax, const StatementList& initializers,
                     const Expression* stopExpr, span<const Expression* const> steps,
                     const Statement& body) :
        Statement(StatementKind::ForLoop, syntax),
        initializers(initializers), stopExpr(stopExpr), steps(steps), body(body) {}

    bool eval(EvalContext& context) const;

    static bool isKind(StatementKind kind) { return kind == StatementKind::ForLoop; }
};

class ExpressionStatement : public Statement {
public:
    const Expression& expr;

    ExpressionStatement(const ExpressionStatementSyntax& syntax, const Expression& expr) :
        Statement(StatementKind::ExpressionStatement, syntax), expr(expr) {}

    bool eval(EvalContext& context) const;

    static bool isKind(StatementKind kind) { return kind == StatementKind::ExpressionStatement; }
};

} // namespace slang