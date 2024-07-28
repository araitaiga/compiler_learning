#include <iostream>
#include <exception>
#include <stdexcept> // std::runtime_error

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    std::cerr << "The number of arguments is incorrect" << std::endl;
    throw std::runtime_error("error!");
    // return 1;
  }

  std::string p = argv[1];
  size_t idx = 0;

  std::cout << ".intel_syntax noprefix" << std::endl;
  std::cout << ".globl main" << std::endl;
  std::cout << "main:" << std::endl;
  std::cout << "  mov rax, " << std::stol(p, &idx, 10) << std::endl;

  while (idx < p.size())
  {
    if (p[idx] == '+')
    {
      idx++;
      std::cout << "  add rax, " << std::stol(&p[idx], &idx, 10) << std::endl;
      continue;
    }
    if (p[idx] == '-')
    {
      idx++;
      std::cout << "  sub rax, " << std::stol(&p[idx], &idx, 10) << std::endl;
      continue;
    }

    std::cerr << "The operator cannot be loaded: " << &p[idx] << std::endl;
    throw std::runtime_error("error!");
    // return 1;
  }

  std::cout << "  ret" << std::endl;
  return 0;
}
