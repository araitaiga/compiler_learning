#pragma once

enum class TokenKind
{
  TK_RESERVED, // 記号
  TK_INDENT,   // 識別子
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
};

struct Token
{
  TokenKind kind;      // トークンの型
  int val{0};          // kindがTK_NUMの場合、その数値
  std::string str{""}; // トークン文字列
};

// ローカル変数の型
struct LVar
{
  // LVar *next;       // 次の変数かNULL
  std::string name; // 変数名
  int len;          // 変数名の長さ
  int offset;       // RBPからのオフセット
  bool operator<(const LVar &rhs) const
  {
    return name < rhs.name;
  }
};
