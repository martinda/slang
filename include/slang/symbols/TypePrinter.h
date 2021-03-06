//------------------------------------------------------------------------------
// TypePrinter.h
// Type printing utilities.
//
// File is under the MIT license; see LICENSE for details.
//------------------------------------------------------------------------------
#pragma once

#include "slang/symbols/ASTVisitor.h"
#include "slang/symbols/TypeSymbols.h"
#include "slang/text/FormatBuffer.h"

namespace slang {

class TypePrinter : public ASTVisitor<TypePrinter> {
public:
    void append(const Type& type);

    std::string toString() const;

    void handle(const ScalarType& type);
    void handle(const PredefinedIntegerType& type);
    void handle(const FloatingType& type);
    void handle(const EnumType& type);
    void handle(const PackedArrayType& type);
    void handle(const PackedStructType& type);
    void handle(const UnpackedArrayType& type);
    void handle(const UnpackedStructType& type);
    void handle(const VoidType& type);
    void handle(const NullType& type);
    void handle(const CHandleType& type);
    void handle(const StringType& type);
    void handle(const EventType& type);
    void handle(const TypeAliasType& type);
    void handle(const ErrorType& type);

    // Catch-all method that should never be called.
    void handle(const Type&) { THROW_UNREACHABLE; }

private:
    void appendStructMembers(const Scope& scope);

    FormatBuffer buffer;
};

} // namespace slang