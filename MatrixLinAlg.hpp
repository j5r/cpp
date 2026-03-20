#pragma once
#include "Matrix.hpp"

class MatrixLinAlg
{
private:
  MatrixLinAlg(const MatrixLinAlg &) = delete;
  MatrixLinAlg &operator=(const MatrixLinAlg &) = delete;

public:
  MatrixLinAlg() = delete;
  ~MatrixLinAlg() = delete;

  // Add your static methods here

  template <typename T>
  [[nodiscard]] static Matrix<T> pivot(const Matrix<T> &m, size_t pivot_row, size_t pivot_col)
  {

    Matrix<T> result = m;
    if (m(pivot_row, pivot_col) == T())
    {
      std::cerr << _yellow + "\nMatrixLinAlg::pivot() Warning : pivot is zero, nothing has been done.\n" + _reset;
      return m;
    }

    // normaliza linha pivo
    T pivot_value = result(pivot_row, pivot_col);
    for (size_t j = 0; j < result.cols(); j++)
    {
      result(pivot_row, j) /= pivot_value;
    }

    // restante do pivoteamento
    for (size_t i = 0; i < result.rows(); i++)
    {
      if (i == pivot_row)
        continue;
      T factor = result(i, pivot_col);
      for (size_t j = 0; j < result.cols(); j++)
      {
        result(i, j) -= factor * result(pivot_row, j);
      }
    }
    return result;
  }


  template <typename T>
  static bool _pivot(Matrix<T> &m, size_t pivot_row, size_t pivot_col)
  {

    if (m(pivot_row, pivot_col) == T())
    {
      return false;
    }

    // normaliza linha pivo
    T pivot_value = m(pivot_row, pivot_col);
    for (size_t j = 0; j < m.cols(); j++)
    {
      m(pivot_row, j) /= pivot_value;
    }

    // restante do pivoteamento
    for (size_t i = 0; i < m.rows(); i++)
    {
      if (i == pivot_row)
        continue;
      T factor = m(i, pivot_col);
      for (size_t j = 0; j < m.cols(); j++)
      {
        m(i, j) -= factor * m(pivot_row, j);
      }
    }
    return true;
  }
  
};
