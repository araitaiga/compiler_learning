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

    std::regex re(R"(\s*(\d+|[-+])\s*)");
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
      token.str = match[0];
      if (std::isdigit(token.str[0]))
      {
        token.kind = TokenKind::TK_NUM;
        token.val = std::stoi(token.str);
      }
      else
      {
        // "+" or "-"の場合
        token.kind = TokenKind::TK_RESERVED;
        token.str = token.str[0];
      }
      token_list.push_back(token);
    }

    if (hasErrorToken(error_report))
    {
      std::cout << str << std::endl;
      std::cout << error_report << std::endl;
      throw std::runtime_error("error!");
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
