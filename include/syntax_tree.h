#pragma once

#include "token_pointer.h"
#include "node.h"
#include <memory>
#include <vector>
#include <optional>
#include <set>

class SyntaxTree
{

public:
  explicit SyntaxTree(std::vector<Token> &token_list) : token_pointer(token_list)
  {
  }

  std::vector<std::shared_ptr<Node>> program()
  {
    std::vector<std::shared_ptr<Node>> nodes;

    while (!token_pointer.atEOF())
    {
      nodes.push_back(stmt());
    }
    return nodes;
  }

private:
  TokenPointer token_pointer;
  std::set<LVar> lvars;

  std::shared_ptr<Node> newNode(NodeType type, std::shared_ptr<Node> lhs, std::shared_ptr<Node> rhs)
  {
    auto node = std::make_shared<Node>();
    node->type = type;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
  }
  std::shared_ptr<Node> newLValueNode(const LVar &lvar)
  {
    auto node = std::make_shared<Node>();
    node->type = NodeType::ND_LVAR;
    node->offset = lvar.offset;
    return node;
  }
  std::shared_ptr<Node> newNumberNode(int val)
  {
    auto node = std::make_shared<Node>();
    node->type = NodeType::ND_NUM;
    node->val = val;
    return node;
  }

  std::shared_ptr<Node> stmt()
  {
    auto node = expr();
    if (!token_pointer.consume(";"))
      throw std::runtime_error("error! ';' is not found");
    return node;
  }

  std::shared_ptr<Node> expr()
  {
    return assign();
  }

  std::shared_ptr<Node> assign()
  {
    auto node = equality();
    if (token_pointer.consume("="))
      node = newNode(NodeType::ND_ASSIGN, node, assign());
    return node;
  }

  std::shared_ptr<Node> equality()
  {
    auto node = relational();
    while (true)
    {
      if (token_pointer.consume("=="))
        node = newNode(NodeType::ND_EQ, node, relational());
      else if (token_pointer.consume("!="))
        node = newNode(NodeType::ND_NE, node, relational());
      else
        return node;
    }
  }
  std::shared_ptr<Node> relational()
  {
    auto node = add();
    while (true)
    {
      if (token_pointer.consume("<"))
        node = newNode(NodeType::ND_LT, node, add());
      else if (token_pointer.consume("<="))
        node = newNode(NodeType::ND_LE, node, add());
      else if (token_pointer.consume(">"))
        node = newNode(NodeType::ND_LT, add(), node);
      else if (token_pointer.consume(">="))
        node = newNode(NodeType::ND_LE, add(), node);
      else
        return node;
    }
  }

  std::shared_ptr<Node> add()
  {
    auto node = mul();
    while (true)
    {
      if (token_pointer.consume("+"))
        node = newNode(NodeType::ND_ADD, node, mul());
      else if (token_pointer.consume("-"))
        node = newNode(NodeType::ND_SUB, node, mul());
      else
        return node;
    }
  }

  std::shared_ptr<Node> mul()
  {
    auto node = unary();
    while (true)
    {
      if (token_pointer.consume("*"))
        node = newNode(NodeType::ND_MUL, node, unary());
      else if (token_pointer.consume("/"))
        node = newNode(NodeType::ND_DIV, node, unary());
      else
        return node;
    }
  }
  std::shared_ptr<Node> unary()
  {
    if (token_pointer.consume("+"))
      return primary();
    else if (token_pointer.consume("-"))
      // 単項"-"は0 - primary()として扱う
      return newNode(NodeType::ND_SUB, newNumberNode(0), primary());

    return primary();
  }
  std::shared_ptr<Node> primary()
  {
    if (token_pointer.consume("("))
    {
      auto node = expr();
      if (!token_pointer.consume(")"))
        throw std::runtime_error("error! ')' is not found");
      return node;
    }

    auto indent_token = token_pointer.consumeIndent();
    if (indent_token.has_value()) // 文字列の場合
    {
      const auto lvar = findLVar(indent_token.value().str);
      if (!lvar)
      {
        // 新たなoffset位置に変数を作成
        const LVar new_lvar = {indent_token.value().str, indent_token.value().str.size(), static_cast<int>(lvars.size() + 1) * 8};
        lvars.insert(new_lvar);
        return newLValueNode(new_lvar);
      }
      return newLValueNode(*lvar);
    }
    else // 数値の場合
    {
      return newNumberNode(token_pointer.consumeAndGetNumber());
    }
  }

  std::optional<LVar> findLVar(const std::string &name)
  {
    const LVar lvar_matching = {name, static_cast<int>(name.size()), 0};
    const auto lvar_found = lvars.find(lvar_matching);
    if (lvar_found == lvars.end())
      return std::nullopt;
    return *lvar_found;
  }
};
