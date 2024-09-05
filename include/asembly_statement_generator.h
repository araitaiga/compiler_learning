#pragma once

#include "node.h"
#include <iostream>
#include <memory>

class AsemblyStatementGenerator
{
public:
  AsemblyStatementGenerator() {}

  void generateAllStatements(std::vector<std::shared_ptr<Node>> nodes)
  {
    initialState();
    prologue();

    for (auto node : nodes)
    {
      generate(node);
      std::cout << "  pop rax" << std::endl;
    }
    epilogue();
    endState();
  }

private:
  void initialState()
  {
    std::cout << ".intel_syntax noprefix" << std::endl;
    std::cout << ".globl main" << std::endl;
    std::cout << "main:" << std::endl;
  }

  void prologue()
  {
    // 変数26個分の領域を確保する
    std::cout << "  push rbp" << std::endl;
    std::cout << "  mov rbp, rsp" << std::endl;
    std::cout << "  sub rsp, 208" << std::endl;
  }

  void epilogue()
  {
    std::cout << "  mov rsp, rbp" << std::endl;
    std::cout << "  pop rbp" << std::endl;
  }

  void endState()
  {
    // std::cout << "  pop rax" << std::endl;
    std::cout << "  ret" << std::endl;
  }

  void generateLValue(std::shared_ptr<Node> node)
  {
    if (node->type != NodeType::ND_LVAR)
      throw std::runtime_error("error! not lvalue");

    std::cout << "  mov rax, rbp" << std::endl;
    std::cout << "  sub rax, " << node->offset << std::endl;
    std::cout << "  push rax" << std::endl;
  }

  void generate(std::shared_ptr<Node> node)
  {
    if (node->type == NodeType::ND_NUM)
    {
      std::cout << "  push " << node->val << std::endl;
      return;
    }

    switch (node->type)
    {
    case NodeType::ND_NUM:
      std::cout << "  push " << node->val << std::endl;
      return;
    case NodeType::ND_LVAR:
      generateLValue(node);
      std::cout << "  pop rax" << std::endl;
      std::cout << "  mov rax, [rax]" << std::endl;
      std::cout << "  push rax" << std::endl;
      return;
    case NodeType::ND_ASSIGN:
      generateLValue(node->lhs);
      generate(node->rhs);

      std::cout << "  pop rdi" << std::endl;
      std::cout << "  pop rax" << std::endl;
      std::cout << "  mov [rax], rdi" << std::endl;
      std::cout << "  push rdi" << std::endl;
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
