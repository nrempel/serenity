/*
 * Copyright (c) 2021, Itamar S. <itamar8910@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "AK/NonnullRefPtr.h"
#include "AST.h"
#include "Preprocessor.h"
#include <AK/Noncopyable.h>
#include <LibCpp/Lexer.h>

namespace Cpp {

class Parser final {
    AK_MAKE_NONCOPYABLE(Parser);

public:
    explicit Parser(const StringView& program, const String& filename, Preprocessor::Definitions&& = {});
    ~Parser() = default;

    NonnullRefPtr<TranslationUnit> parse();
    bool eof() const;

    RefPtr<ASTNode> eof_node() const;
    RefPtr<ASTNode> node_at(Position) const;
    Optional<size_t> index_of_node_at(Position) const;
    Optional<Token> token_at(Position) const;
    Optional<size_t> index_of_token_at(Position) const;
    RefPtr<const TranslationUnit> root_node() const { return m_root_node; }
    String text_of_node(const ASTNode&) const;
    StringView text_of_token(const Cpp::Token& token) const;
    void print_tokens() const;
    const Vector<String>& errors() const { return m_state.errors; }
    const Preprocessor::Definitions& definitions() const { return m_definitions; }

    struct TokenAndPreprocessorDefinition {
        Token token;
        Preprocessor::DefinedValue preprocessor_value;
    };
    const Vector<TokenAndPreprocessorDefinition>& replaced_preprocessor_tokens() const { return m_replaced_preprocessor_tokens; }

private:
    enum class DeclarationType {
        Function,
        Variable,
        Enum,
        Struct,
        Namespace,
        Class,
    };

    Optional<DeclarationType> match_declaration_in_translation_unit();
    bool match_function_declaration();
    bool match_comment();
    bool match_preprocessor();
    bool match_whitespace();
    bool match_variable_declaration();
    bool match_expression();
    bool match_secondary_expression();
    bool match_enum_declaration();
    bool match_class_declaration();
    bool match_struct_declaration();
    bool match_literal();
    bool match_unary_expression();
    bool match_boolean_literal();
    bool match_keyword(const String&);
    bool match_block_statement();
    bool match_namespace_declaration();
    bool match_template_arguments();
    bool match_name();
    bool match_cpp_cast_expression();
    bool match_c_style_cast_expression();
    bool match_sizeof_expression();
    bool match_braced_init_list();
    bool match_type();

    Optional<NonnullRefPtrVector<Parameter>> parse_parameter_list(ASTNode& parent);
    Optional<Token> consume_whitespace();
    void consume_preprocessor();

    NonnullRefPtr<Declaration> parse_declaration(ASTNode& parent, DeclarationType);
    NonnullRefPtr<FunctionDeclaration> parse_function_declaration(ASTNode& parent);
    NonnullRefPtr<FunctionDefinition> parse_function_definition(ASTNode& parent);
    NonnullRefPtr<Statement> parse_statement(ASTNode& parent);
    NonnullRefPtr<VariableDeclaration> parse_variable_declaration(ASTNode& parent, bool expect_semicolon = true);
    NonnullRefPtr<Expression> parse_expression(ASTNode& parent);
    NonnullRefPtr<Expression> parse_primary_expression(ASTNode& parent);
    NonnullRefPtr<Expression> parse_secondary_expression(ASTNode& parent, NonnullRefPtr<Expression> lhs);
    NonnullRefPtr<FunctionCall> parse_function_call(ASTNode& parent);
    NonnullRefPtr<StringLiteral> parse_string_literal(ASTNode& parent);
    NonnullRefPtr<ReturnStatement> parse_return_statement(ASTNode& parent);
    NonnullRefPtr<EnumDeclaration> parse_enum_declaration(ASTNode& parent);
    NonnullRefPtr<StructOrClassDeclaration> parse_struct_or_class_declaration(ASTNode& parent, StructOrClassDeclaration::Type);
    NonnullRefPtr<MemberDeclaration> parse_member_declaration(ASTNode& parent);
    NonnullRefPtr<Expression> parse_literal(ASTNode& parent);
    NonnullRefPtr<UnaryExpression> parse_unary_expression(ASTNode& parent);
    NonnullRefPtr<BooleanLiteral> parse_boolean_literal(ASTNode& parent);
    NonnullRefPtr<Type> parse_type(ASTNode& parent);
    NonnullRefPtr<BinaryExpression> parse_binary_expression(ASTNode& parent, NonnullRefPtr<Expression> lhs, BinaryOp);
    NonnullRefPtr<AssignmentExpression> parse_assignment_expression(ASTNode& parent, NonnullRefPtr<Expression> lhs, AssignmentOp);
    NonnullRefPtr<ForStatement> parse_for_statement(ASTNode& parent);
    NonnullRefPtr<BlockStatement> parse_block_statement(ASTNode& parent);
    NonnullRefPtr<Comment> parse_comment(ASTNode& parent);
    NonnullRefPtr<IfStatement> parse_if_statement(ASTNode& parent);
    NonnullRefPtr<NamespaceDeclaration> parse_namespace_declaration(ASTNode& parent, bool is_nested_namespace = false);
    NonnullRefPtrVector<Declaration> parse_declarations_in_translation_unit(ASTNode& parent);
    RefPtr<Declaration> parse_single_declaration_in_translation_unit(ASTNode& parent);
    NonnullRefPtrVector<Type> parse_template_arguments(ASTNode& parent);
    NonnullRefPtr<Name> parse_name(ASTNode& parent);
    NonnullRefPtr<CppCastExpression> parse_cpp_cast_expression(ASTNode& parent);
    NonnullRefPtr<SizeofExpression> parse_sizeof_expression(ASTNode& parent);
    NonnullRefPtr<BracedInitList> parse_braced_init_list(ASTNode& parent);
    NonnullRefPtr<CStyleCastExpression> parse_c_style_cast_expression(ASTNode& parent);

    bool match(Token::Type);
    Token consume(Token::Type);
    Token consume();
    Token consume_keyword(const String&);
    Token peek(size_t offset = 0) const;
    Optional<Token> peek(Token::Type) const;
    Position position() const;
    String text_in_range(Position start, Position end) const;

    void save_state();
    void load_state();

    struct State {
        size_t token_index { 0 };
        Vector<String> errors;
        NonnullRefPtrVector<ASTNode> nodes;
    };

    void error(StringView message = {});

    template<class T, class... Args>
    NonnullRefPtr<T>
    create_ast_node(ASTNode& parent, const Position& start, Optional<Position> end, Args&&... args)
    {
        auto node = adopt(*new T(&parent, start, end, m_filename, forward<Args>(args)...));
        if (!parent.is_dummy_node()) {
            m_state.nodes.append(node);
        }
        return node;
    }

    NonnullRefPtr<TranslationUnit>
    create_root_ast_node(const Position& start, Position end)
    {
        auto node = adopt(*new TranslationUnit(nullptr, start, end, m_filename));
        m_state.nodes.append(node);
        m_root_node = node;
        return node;
    }

    DummyAstNode& get_dummy_node()
    {
        static NonnullRefPtr<DummyAstNode> dummy = adopt(*new DummyAstNode(nullptr, {}, {}, {}));
        return dummy;
    }

    bool match_attribute_specification();
    void consume_attribute_specification();
    bool match_ellipsis();
    void initialize_program_tokens(const StringView& program);
    void add_tokens_for_preprocessor(Token& replaced_token, Preprocessor::DefinedValue&);
    Vector<StringView> parse_type_qualifiers();
    Vector<StringView> parse_function_qualifiers();

    Preprocessor::Definitions m_definitions;
    String m_filename;
    Vector<Token> m_tokens;
    State m_state;
    Vector<State> m_saved_states;
    RefPtr<TranslationUnit> m_root_node;

    Vector<TokenAndPreprocessorDefinition> m_replaced_preprocessor_tokens;
};

}
