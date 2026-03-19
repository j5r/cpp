#pragma once
#include "Matrix.hpp"


class MatrixLinAlg
{
private:
  MatrixLinAlg(const MatrixLinAlg&) = delete;
  MatrixLinAlg& operator=(const MatrixLinAlg&) = delete;

  
public:
  MatrixLinAlg() = delete;
  ~MatrixLinAlg() = delete;
  
  // Add your static methods here
  

 template <typename T>
  [[nodiscard]] static Matrix<T> pivot(const Matrix<T>&m, size_t pivot_row, size_t pivot_col) 
    {
        if (pivot_row >= m.rows_ || pivot_col >= m.cols_)
        {
            throw std::out_of_range(_red + "\nMatrixLinAlg::pivot() Error: Pivot row and column indices must be within matrix dimensions.\nGot pivot_row=" + std::to_string(pivot_row) + " and pivot_col=" + std::to_string(pivot_col) + "." + _reset);
        }

        Matrix<T> result(m.rows_, m.cols_);
        m.print();

        /*
        implementacao do pivotamento:
        */
        not_implemented("MatrixLinAlg::pivot()");
        return result;
    }
};

