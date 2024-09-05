#pragma once

#include "token_pointer.h"
#include "node.h"
#include <memory>
#include <vector>

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
    nodes.push_back(nullptr);
    return nodes;
  }

private:
  TokenPointer token_pointer;

  std::shared_ptr<Node> newNode(NodeType type, std::shared_ptr<Node> lhs, std::shared_ptr<Node> rhs)
  {
    auto node = std::make_shared<Node>();
    node->type = type;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
  }
  std::shared_ptr<Node> newLValueNode(char variable)
  {
    auto node = std::make_shared<Node>();
    node->type = NodeType::ND_LVAR;
    node->offset = (variable - 'a' + 1) * 8;
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

    for (char c = 'a'; c <= 'z'; c++)
    {
      if (token_pointer.consumeIndent(c))
        return newLValueNode(c);
    }

    return newNumberNode(token_pointer.consumeAndGetNumber());
  }
};
