//------------------------------------------------------------------------------
// Lexer.h
// Source file lexer.
//
// File is under the MIT license; see LICENSE for details.
//------------------------------------------------------------------------------
#pragma once

#include "slang/diagnostics/Diagnostics.h"
#include "slang/parsing/Token.h"
#include "slang/text/SourceLocation.h"
#include "slang/util/SmallVector.h"
#include "slang/util/Util.h"

namespace slang {

class VectorBuilder;
class BumpAllocator;
struct SourceBuffer;

/// Contains various options that can control lexing behavior.
struct LexerOptions {
    /// The maximum number of errors that can occur before the rest of the source
    /// buffer is skipped.
    uint32_t maxErrors = 16;
};

/// The Lexer is responsible for taking source text and chopping it up into tokens.
/// Tokens carry along leading "trivia", which is things like whitespace and comments,
/// so that we can programmatically piece back together what the original file looked like.
///
/// There are also helper methods on this class that handle token manipulation on the
/// character level.
class Lexer {
public:
    Lexer(SourceBuffer buffer, BumpAllocator& alloc, Diagnostics& diagnostics,
          LexerOptions options = LexerOptions{});

    // Not copyable
    Lexer(const Lexer&) = delete;
    Lexer& operator=(const Lexer&) = delete;

    /// Lexes the next token from the source code.
    /// This will never return a null pointer; at the end of the buffer,
    /// an infinite stream of EndOfFile tokens will be generated
    Token lex(KeywordVersion keywordVersion = getDefaultKeywordVersion());

    BufferID getBufferID() const;
    BumpAllocator& getAllocator() { return alloc; }
    Diagnostics& getDiagnostics() { return diagnostics; }

    /// Concatenates two tokens together; used for macro pasting.
    static Token concatenateTokens(BumpAllocator& alloc, Token left, Token right);

    /// Converts a range of tokens into a string literal; used for macro stringification.
    /// The @a location and @a trivia parameters are used in the newly created token.
    /// The range of tokens to stringify is given by @a begin and @a end.
    /// If @a noWhitespace is set to true, all whitespace in between tokens will be stripped.
    static Token stringify(BumpAllocator& alloc, SourceLocation location, span<Trivia const> trivia,
                           Token* begin, Token* end);

    /// Splits the given token at the specified offset into its raw source text. The trailing
    /// portion of the split is lexed into new tokens and appened to @a results
    static void splitTokens(BumpAllocator& alloc, Diagnostics& diagnostics,
                            const SourceManager& sourceManager, Token sourceToken, size_t offset,
                            KeywordVersion keywordVersion, SmallVector<Token>& results);

private:
    Lexer(BufferID bufferId, string_view source, const char* startPtr, BumpAllocator& alloc,
          Diagnostics& diagnostics, LexerOptions options);

    TokenKind lexToken(Token::Info* info, KeywordVersion keywordVersion);
    TokenKind lexNumericLiteral(Token::Info* info);
    TokenKind lexEscapeSequence(Token::Info* info);
    TokenKind lexDollarSign(Token::Info* info);
    TokenKind lexDirective(Token::Info* info);
    TokenKind lexApostrophe(Token::Info* info);

    void lexStringLiteral(Token::Info* info);
    bool lexIntegerBase(Token::Info* info, bool isSigned);
    bool lexTimeLiteral(Token::Info* info);

    void lexTrivia(SmallVector<Trivia>& triviaBuffer);

    void scanBlockComment(SmallVector<Trivia>& triviaBuffer);
    void scanLineComment(SmallVector<Trivia>& triviaBuffer);
    void scanWhitespace(SmallVector<Trivia>& triviaBuffer);
    void scanIdentifier();
    void scanUnsignedNumber(uint64_t& value, int& digits);
    bool scanExponent(uint64_t& value, bool& negative);

    void addTrivia(TriviaKind kind, SmallVector<Trivia>& triviaBuffer);
    void addDiag(DiagCode code, uint32_t offset);

    // source pointer manipulation
    void mark() { marker = sourceBuffer; }
    void advance() { sourceBuffer++; }
    void advance(int count) { sourceBuffer += count; }
    char peek() { return *sourceBuffer; }
    char peek(int offset) { return sourceBuffer[offset]; }
    uint32_t currentOffset();

    // in order to detect embedded nulls gracefully, we call this whenever we
    // encounter a null to check whether we really are at the end of the buffer
    bool reallyAtEnd() { return sourceBuffer >= sourceEnd - 1; }

    uint32_t lexemeLength() { return (uint32_t)(sourceBuffer - marker); }
    string_view lexeme() { return string_view(marker, lexemeLength()); }

    bool consume(char c) {
        if (peek() == c) {
            advance();
            return true;
        }
        return false;
    }

    BumpAllocator& alloc;
    Diagnostics& diagnostics;
    LexerOptions options;

    // the source text and start and end pointers within it
    BufferID bufferId;
    const char* originalBegin;
    const char* sourceBuffer;
    const char* sourceEnd;

    // save our place in the buffer to measure out the current lexeme
    const char* marker;

    // the number of errors that have occurred while lexing the current buffer
    uint32_t errorCount = 0;

    // Keeps track of whether we just entered a new line, to enforce tokens
    // that must start on their own line
    bool onNewLine = true;
};

} // namespace slang
