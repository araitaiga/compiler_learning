#pragma once
#include <memory>

enum class NodeType
{
  ND_EQ,  // ==
  ND_NE,  // !=
  ND_LT,  // <
  ND_LE,  // <=
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM, // 整数
};

class OutputNodeType
{
public:
  static std::string toString(const NodeType &type)
  {
    switch (type)
    {
    case NodeType::ND_EQ:
      return "ND_EQ";
    case NodeType::ND_NE:
      return "ND_NE";
    case NodeType::ND_LT:
      return "ND_LT";
    case NodeType::ND_LE:
      return "ND_LE";
    case NodeType::ND_ADD:
      return "ND_ADD";
    case NodeType::ND_SUB:
      return "ND_SUB";
    case NodeType::ND_MUL:
      return "ND_MUL";
    case NodeType::ND_DIV:
      return "ND_DIV";
    case NodeType::ND_NUM:
      return "ND_NUM";
    }
  }
};

struct Node
{
  NodeType type;
  std::shared_ptr<Node> lhs;
  std::shared_ptr<Node> rhs;
  int val{0};
};
