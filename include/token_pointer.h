#pragma once
#include "token.h"
#include <vector>

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
