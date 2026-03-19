#pragma once
#include <iostream>
#include <cstdlib>
#include <string> 
#include <stdexcept>

#define j5r_assert(cond, msg) \
    if (!(cond)) { \
      std::cerr << "\033[31mAssertion Error: file[" + std::string(__FILE__) + "].row[" + std::to_string(__LINE__) + "]:\t(j5rassert)\n\t" + std::string(msg) + "\033[m"; \
        system("pause"); \
        std::abort(); \
    }
