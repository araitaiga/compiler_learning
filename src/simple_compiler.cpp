#include <iostream>
#include <exception>
#include <stdexcept> // std::runtime_error

#include "syntax_tree.h"
#include "tokenizer.h"
#include "asembly_statement_generator.h"

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

  SyntaxTree syntax_tree(token_list);
  auto nodes = syntax_tree.program();

  AsemblyStatementGenerator asembly_statement_generator;
  asembly_statement_generator.generateAllStatements(nodes);

  return 0;
}
