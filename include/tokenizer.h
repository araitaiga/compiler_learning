#pragma once

#include "token.h"
#include <regex>
#include <string>
#include <vector>
#include <cctype>

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
    // [空白文字], (1桁以上の数字, >=, <=, ==, !=, +, -, *, /, (, ), <, >,;,'a'~'z')のいずれか, [空白文字]
    std::regex re(R"(\s*(\d+|<=|>=|==|!=|[-+*/()<>=;]|[a-z])\s*)");

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
      else if ('a' <= token.str[0] && token.str[0] <= 'z')
      {
        // 変数
        token.kind = TokenKind::TK_INDENT;
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
