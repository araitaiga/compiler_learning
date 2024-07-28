#include <iostream>
#include <exception>
#include <stdexcept> // std::runtime_error
#include <memory>
#include <vector>
#include <cctype>

#include <regex>
#include <string>

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

// class TokenManager
// {
// private:
//   std::vector<Token> token_list;

// public:
//   explicit TokenManager(std::vector<Token> &token_list) : token_list(token_list)
//   {
//     if (token_list.empty())
//       throw std::runtime_error("error! token_list is empty");

//     if (token_list.back().kind != TokenKind::TK_EOF)
//       throw std::runtime_error("error! last token is not EOF");

//     if (token_list[0].kind != TokenKind::TK_NUM)
//       throw std::runtime_error("error! first token is not number");
//   }
//   bool consume(char op)
//   {
//     if (token->kind != TokenKind::TK_RESERVED || token->str[0] != op)
//     {
//       return false;
//     }
//     token = token->next;
//     return true;
//   }
// };

class Tokenizer
{
public:
  Tokenizer() {}

  std::vector<Token> tokenize(const std::string &str)
  {
    std::vector<Token> token_list;

    std::regex re(R"(\s*(\d+|[-+])\s*)");
    std::smatch m;

    for (auto it = std::sregex_iterator(str.begin(), str.end(), re); it != std::sregex_iterator(); it++)
    {
      Token token;
      token.str = (*it)[0];
      if (std::isdigit(token.str[0]))
      {
        token.kind = TokenKind::TK_NUM;
        token.val = std::stoi(token.str);
      }
      else
      {
        token.kind = TokenKind::TK_RESERVED;
        token.str = token.str[0];
      }
      token_list.push_back(token);
    }
    return token_list;
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

  std::cout << ".intel_syntax noprefix" << std::endl;
  std::cout << ".globl main" << std::endl;
  std::cout << "main:" << std::endl;

  auto first_num = token_list[0];
  if (first_num.kind != TokenKind::TK_NUM)
    throw std::runtime_error("error! first token is not number");

  std::cout << "  mov rax, " << first_num.val << std::endl;

  for (auto it = token_list.begin() + 1; it != token_list.end(); it++)
  {
    if (it->kind == TokenKind::TK_RESERVED)
    {
      if (it->str == "+")
      {
        it++;
        std::cout << "  add rax, " << it->val << std::endl;
      }
      else if (it->str == "-")
      {
        it++;
        std::cout << "  sub rax, " << it->val << std::endl;
      }
      else
      {
        throw std::runtime_error("error! unknown operator");
      }
    }

    if (it->kind == TokenKind::TK_EOF)
      break;
  }

  std::cout << "  ret" << std::endl;
  return 0;
}
