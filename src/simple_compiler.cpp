#include <iostream>
#include <exception>
#include <stdexcept> // std::runtime_error
#include <memory>
#include <vector>
#include <cctype>

#include <regex>
#include <string>
#include <list>
#include <iterator>

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

enum class TokenKind
{
  TK_RESERVED, // 記号
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
};

struct Token
{
  TokenKind kind;      // トークンの型
  int val{0};          // kindがTK_NUMの場合、その数値
  std::string str{""}; // トークン文字列
};

class TokenPointer
{
private:
  std::vector<Token> token_list;
  std::vector<Token>::iterator current_token;

public:
  explicit TokenPointer(std::vector<Token> &token_list) : token_list(token_list)
  {
    if (token_list.empty())
      throw std::runtime_error("error! token_list is empty");

    if (token_list.back().kind != TokenKind::TK_EOF)
      throw std::runtime_error("error! last token is not EOF");

    current_token = token_list.begin();
  }

  bool atEOF()
  {
    return current_token->kind == TokenKind::TK_EOF;
  }

  bool consume(const std::string &op)
  {
    if (current_token->kind != TokenKind::TK_RESERVED || current_token->str != op)
    {
      return false;
    }
    ++current_token;
    return true;
  }

  int getNumber()
  {
    if (current_token->kind != TokenKind::TK_NUM)
      throw std::runtime_error("error! not number");

    int number = current_token->val;
    ++current_token;
    return number;
  }
};

class SyntaxTree
{
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

  std::shared_ptr<Node> newNumberNode(int val)
  {
    auto node = std::make_shared<Node>();
    node->type = NodeType::ND_NUM;
    node->val = val;
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
    return newNumberNode(token_pointer.getNumber());
  }

public:
  explicit SyntaxTree(std::vector<Token> &token_list) : token_pointer(token_list)
  {
  }

  std::shared_ptr<Node> expr()
  {
    return equality();
  }
};

class Tokenizer
{
private:
  bool hasErrorToken(const std::string &error_report)
  {
    for (auto c : error_report)
    {
      if (c == '^')
        return true;
    }
    return false;
  }

public:
  Tokenizer() {}

  std::vector<Token> tokenize(const std::string &str)
  {
    std::vector<Token> token_list;

    // strの先頭から、以下にマッチするものを探す
    // [空白文字], (1桁以上の数字, >=, <=, ==, !=, +, -, *, /, (, ), <, >)のいずれか, [空白文字]
    std::regex re(R"(\s*(\d+|<=|>=|==|!=|[-+*/()<>])\s*)");

    std::sregex_iterator begin(str.begin(), str.end(), re);
    std::sregex_iterator end;

    size_t last_match_end = 0;
    std::string error_report(str.size(), ' ');

    for (auto it = begin; it != end; ++it)
    {
      std::smatch match = *it;
      size_t match_start = match.position();
      size_t match_end = match_start + match.length();
      if (last_match_end < match_start)
      {
        for (size_t i = last_match_end; i < match_start; ++i)
        {
          error_report[i] = '^';
        }
      }
      last_match_end = match_end;

      Token token;
      token.str = match[0].str();
      if (std::isdigit(token.str[0]))
      {
        token.kind = TokenKind::TK_NUM;
        token.val = std::stoi(token.str);
      }
      else
      {
        token.kind = TokenKind::TK_RESERVED;
      }
      token_list.push_back(token);
    }
    const Token eof_token{TokenKind::TK_EOF, 0, ""};
    token_list.push_back(eof_token);

    if (hasErrorToken(error_report))
    {
      std::cout << str << std::endl;
      std::cout << error_report << std::endl;
      throw std::runtime_error("error!");
    }

    return token_list;
  }
};

class AsemblyStatementGenerator
{
public:
  AsemblyStatementGenerator() {}

  void initialState()
  {
    std::cout << ".intel_syntax noprefix" << std::endl;
    std::cout << ".globl main" << std::endl;
    std::cout << "main:" << std::endl;
  }

  void generate(std::shared_ptr<Node> node)
  {
    if (node->type == NodeType::ND_NUM)
    {
      std::cout << "  push " << node->val << std::endl;
      return;
    }

    generate(node->lhs);
    generate(node->rhs);

    std::cout << "  pop rdi" << std::endl;
    std::cout << "  pop rax" << std::endl;
    switch (node->type)
    {
    case NodeType::ND_EQ:
      std::cout << "  cmp rax, rdi" << std::endl;
      std::cout << "  sete al" << std::endl;
      std::cout << "  movzb rax, al" << std::endl;
      break;
    case NodeType::ND_NE:
      std::cout << "  cmp rax, rdi" << std::endl;
      std::cout << "  setne al" << std::endl;
      std::cout << "  movzb rax, al" << std::endl;
      break;
    case NodeType::ND_LT:
      std::cout << "  cmp rax, rdi" << std::endl;
      std::cout << "  setl al" << std::endl;
      std::cout << "  movzb rax, al" << std::endl;
      break;
    case NodeType::ND_LE:
      std::cout << "  cmp rax, rdi" << std::endl;
      std::cout << "  setle al" << std::endl;
      std::cout << "  movzb rax, al" << std::endl;
      break;
    case NodeType::ND_ADD:
      std::cout << "  add rax, rdi" << std::endl;
      break;
    case NodeType::ND_SUB:
      std::cout << "  sub rax, rdi" << std::endl;
      break;
    case NodeType::ND_MUL:
      std::cout << "  imul rax, rdi" << std::endl;
      break;
    case NodeType::ND_DIV:
      std::cout << "  cqo" << std::endl;
      std::cout << "  idiv rdi" << std::endl;
      break;
    }

    std::cout << "  push rax" << std::endl;
  }
};

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    std::cerr << "The number of arguments is incorrect" << std::endl;
    throw std::runtime_error("error!");
  }

  std::string p = argv[1];

  Tokenizer tokenizer;
  auto token_list = tokenizer.tokenize(p);

  TokenPointer token_pointer(token_list);

  SyntaxTree syntax_tree(token_list);

  auto node = syntax_tree.expr();

  AsemblyStatementGenerator asembly_statement_generator;
  asembly_statement_generator.initialState();
  asembly_statement_generator.generate(node);

  std::cout << "  pop rax" << std::endl;
  std::cout << "  ret" << std::endl;

  return 0;
}
