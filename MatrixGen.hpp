#pragma once

#include "Matrix.hpp"
#include <random>

class MatrixGen
{
private:
  MatrixGen() = delete;
  inline static int seed = -1;

  static std::mt19937 create_engine()
  {
    // 1. Instanciamos o motor vazio
    std::mt19937 gen;

    // 2. Decidimos qual semente usar
    if (seed == -1)
    {
      std::random_device rd;
      gen.seed(rd()); // Semente caótica (comportamento padrão)
    }
    else
    {
      // [A MÁGICA AQUI: O motor usa o número fixo que você passou]
      gen.seed(static_cast<unsigned int>(seed));
    }
    return gen;
  } 

public:
  static void set_random_seed(int s) { seed = s; }
  static void unfreeze_seed() { seed = -1; }
  template <typename T>
  static Matrix<T> rand(size_t nrows, size_t ncols = 1, T min_value = T(), T max_value = T(1))
  {
    // [Trava a compilação com uma mensagem amigável se T não for float ou double]
    static_assert(std::is_floating_point<T>::value, _red + "\nMatrix::rand() Method requires float or double types of matrix.\n" + _reset);

    Matrix<T> result(nrows, ncols);

    auto gen = create_engine();

    // [Garante uma distribuição plana e igualitária entre min_value e max_value]
    std::uniform_real_distribution<T> dis(min_value, max_value);

    for (size_t i = 0; i < result.rows(); ++i)
    {
      for (size_t j = 0; j < result.cols(); ++j)
      {
        result(i, j) = dis(gen);
      }
    }
    return result;
  }

  template <typename T>
  static Matrix<T> exprand(T lambda, size_t nrows, size_t ncols = 1)
  {
    static_assert(std::is_floating_point<T>::value, _red + "\nMatrix::exprand() Method requires float or double types of matrix.\n" + _reset);

    Matrix<T> result(nrows, ncols);

    auto gen = create_engine();

    // [C++ já aplica nativamente a fórmula que você citou: -ln(1-u)/lambda]
    std::exponential_distribution<T> dis(lambda);

    for (size_t i = 0; i < result.rows(); ++i)
    {
      for (size_t j = 0; j < result.cols(); ++j)
      {
        result(i, j) = dis(gen);
      }
    }
    return result;
  }

  template <typename T>
  static Matrix<T> randn(size_t nrows, size_t ncols = 1)
  {
    static_assert(std::is_floating_point<T>::value, _red + "\nMatrix::randn() Method requires float or double types of matrix.\n" + _reset);

    Matrix<T> result(nrows, ncols);

    auto gen = create_engine();

    // [Gera a famosa Curva de Sino (Gaussiana) com média 0.0 e desvio padrão 1.0]
    std::normal_distribution<T> dis(0.0, 1.0);

    for (size_t i = 0; i < result.rows(); ++i)
    {
      for (size_t j = 0; j < result.cols(); ++j)
      {
        result(i, j) = dis(gen);
      }
    }
    return result;
  }

  template <typename T>
  static Matrix<T> eye(size_t nrows, size_t ncols = 1, int shift = 0)
  {
    Matrix<T> result(nrows, ncols);

    // [Descobre qual é a menor dimensão para não tentar escrever fora da matriz]
    size_t limit = std::min(nrows, ncols);

    // [Percorre apenas a diagonal principal e preenche com 1, fazendo o cast para o tipo T]
    for (size_t i = 0; i < limit; ++i)
    {
      auto col_index = i + shift;
      if (col_index < ncols && col_index >= 0) // [Garante que o índice da coluna não
        result(i, col_index) = T(1);
    }
    return result;
  }

  template <typename T>
  static Matrix<T> randi(size_t nrows, size_t ncols = 1, int min_value = 0, int max_value = 1)
  {
    static_assert(std::is_integral<T>::value, _red + "\nMatrix::randint() Method requires integer types of matrix.\n" + _reset);

    Matrix<T> result(nrows, ncols);

    auto gen = create_engine();

    std::uniform_int_distribution<int> dis(min_value, max_value);

    for (size_t i = 0; i < result.rows(); ++i)
    {
      for (size_t j = 0; j < result.cols(); ++j)
      {
        result(i, j) = static_cast<T>(dis(gen));
      }
    }
    return result;
  }

  template <typename T>
  static Matrix<size_t> randd(const Matrix<T> &distrib, size_t nrows, size_t ncols = 1)
  {
    auto sum_up_to_one = distrib.sum().sum(1).operator()(0, 0);

    Matrix<size_t> result(nrows, ncols);
  }
};