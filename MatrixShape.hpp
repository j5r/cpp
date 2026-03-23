#pragma once

#include "Matrix.hpp"
#include <vector>
#include <stdexcept>

class MatrixShape
{
public:
    // --- CONCATENAÇÃO HORIZONTAL ---
    template <typename T, typename... Args>
    static Matrix<T> hconcat(const Matrix<T> &first, const Args &...rest)
    {
        // [Desempacota os argumentos variádicos criando uma lista de ponteiros]
        std::vector<const Matrix<T> *> matrices = {&first, &rest...};

        size_t target_rows = first.rows();
        size_t total_cols = 0;

        // 1. Validação de dimensões e cálculo do tamanho final
        for (const Matrix<T> *m : matrices)
        {
            if (m->rows() != target_rows)
            {
                throw std::invalid_argument(_red + "\nMatrixShape::hconcat() Error: All matrices must have the same number of rows for horizontal concatenation.\n" + _reset);
            }
            total_cols += m->cols();
        }

        // 2. Alocação da matriz resultante
        Matrix<T> result(target_rows, total_cols);

        // 3. Cópia dos dados
        size_t current_col_offset = 0;
        for (const Matrix<T> *m : matrices)
        {
            for (size_t i = 0; i < target_rows; ++i)
            {
                for (size_t j = 0; j < m->cols(); ++j)
                {
                    // [Usa a sobrecarga operator() que criamos anteriormente]
                    result(i, current_col_offset + j) = (*m)(i, j);
                }
            }
            // [Avança o cursor de colunas para encaixar a próxima matriz ao lado]
            current_col_offset += m->cols();
        }

        return result;
    }

    // --- CONCATENAÇÃO VERTICAL ---
    template <typename T, typename... Args>
    static Matrix<T> vconcat(const Matrix<T> &first, const Args &...rest)
    {
        std::vector<const Matrix<T> *> matrices = {&first, &rest...};

        size_t target_cols = first.cols();
        size_t total_rows = 0;

        // 1. Validação
        for (const Matrix<T> *m : matrices)
        {
            if (m->cols() != target_cols)
            {
                throw std::invalid_argument(_red + "\nMatrixShape::vconcat() Error: All matrices must have the same number of columns for vertical concatenation.\n" + _reset);
            }
            total_rows += m->rows();
        }

        // 2. Alocação
        Matrix<T> result(total_rows, target_cols);

        // 3. Cópia dos dados
        size_t current_row_offset = 0;
        for (const Matrix<T> *m : matrices)
        {
            for (size_t i = 0; i < m->rows(); ++i)
            {
                for (size_t j = 0; j < target_cols; ++j)
                {
                    // [Desloca o índice da linha para empilhar as matrizes]
                    result(current_row_offset + i, j) = (*m)(i, j);
                }
            }
            current_row_offset += m->rows();
        }

        return result;
    }

    template <typename T, typename... Args>
    static Matrix<T> hexpand(const Matrix<T> &first, const Args &...rest)
    {
        // [Desempacota os argumentos variádicos criando uma lista de ponteiros]
        std::vector<const Matrix<T> *> matrices = {&first, &rest...};

        size_t total_rows = 0;
        size_t total_cols = 0;

        // 1. Validação de dimensões e cálculo do tamanho final
        for (const Matrix<T> *m : matrices)
        {
            total_cols += m->cols();
            total_rows = std::max(m->rows(), total_rows);
        }

        // 2. Alocação da matriz resultante
        Matrix<T> result(total_rows, total_cols);

        // 3. Cópia dos dados
        size_t current_col_offset = 0;
        for (const Matrix<T> *m : matrices)
        {
            for (size_t i = 0; i < m->rows(); ++i)
            {
                for (size_t j = 0; j < m->cols(); ++j)
                {
                    // [Usa a sobrecarga operator() que criamos anteriormente]
                    result(i, current_col_offset + j) = (*m)(i, j);
                }
            }
            // [Avança o cursor de colunas para encaixar a próxima matriz ao lado]
            current_col_offset += m->cols();
        }

        return result;
    }

    template <typename T, typename... Args>
    static Matrix<T> vexpand(const Matrix<T> &first, const Args &...rest)
    {
        // [Desempacota os argumentos variádicos criando uma lista de ponteiros]
        std::vector<const Matrix<T> *> matrices = {&first, &rest...};

        size_t total_rows = 0;
        size_t total_cols = 0;

        // 1. Validação de dimensões e cálculo do tamanho final
        for (const Matrix<T> *m : matrices)
        {
            total_cols = std::max(m->cols(), total_cols);
            total_rows += m->rows();
        }

        // 2. Alocação da matriz resultante
        Matrix<T> result(total_rows, total_cols);

        // 3. Cópia dos dados
        size_t current_row_offset = 0;
        for (const Matrix<T> *m : matrices)
        {
            for (size_t i = 0; i < m->rows(); ++i)
            {
                for (size_t j = 0; j < m->cols(); ++j)
                {
                    // [Usa a sobrecarga operator() que criamos anteriormente]
                    result(current_row_offset + i, j) = (*m)(i, j);
                }
            }
            // [Avança o cursor de linhas para encaixar a próxima matriz abaixo]
            current_row_offset += m->rows();
        }

        return result;
    }

    template <typename T>
    static Matrix<T> addrows(const Matrix<T> &m, size_t n_rows = 1, T initial_value = T())
    {
        if (n_rows == 0)
        {
            return m; // Sem alterações
        }

        Matrix<T> result(m.rows() + n_rows, m.cols(), initial_value);

        // Copia os dados existentes
        for (size_t i = 0; i < m.rows(); ++i)
        {
            for (size_t j = 0; j < m.cols(); ++j)
            {
                result(i, j) = m(i, j);
            }
        }

        // As novas linhas já estão inicializadas com o valor padrão de T()
        return result;
    }

    template <typename T>
    static Matrix<T> addcols(const Matrix<T> &m, size_t n_cols = 1, T initial_value = T())
    {
        if (n_cols == 0)
        {
            return m; // Sem alterações
        }

        Matrix<T> result(m.rows(), m.cols() + n_cols, initial_value);

        // Copia os dados existentes
        for (size_t i = 0; i < m.rows(); ++i)
        {
            for (size_t j = 0; j < m.cols(); ++j)
            {
                result(i, j) = m(i, j);
            }
        }

        // As novas colunas já estão inicializadas com o valor padrão de T()
        return result;
    }

    template <typename T>
    static Matrix<T> rmlrows(const Matrix<T> &m, size_t n_rows = 1)
    { // remove last rows
        if (n_rows >= m.rows())
        {
            throw std::invalid_argument(_red + "\nMatrixShape::rmrows() Error: Cannot remove more rows than the matrix has.\n" + _reset);
        }

        Matrix<T> result(m.rows() - n_rows, m.cols());

        // Copia os dados restantes
        for (size_t i = 0; i < result.rows(); ++i)
        {
            for (size_t j = 0; j < m.cols(); ++j)
            {
                result(i, j) = m(i, j);
            }
        }

        return result;
    }

    template <typename T>
    static Matrix<T> rmlcols(const Matrix<T> &m, size_t n_cols = 1)
    { // remove last columns
        if (n_cols >= m.cols())
        {
            throw std::invalid_argument(_red + "\nMatrixShape::rmcols() Error: Cannot remove more columns than the matrix has.\n" + _reset);
        }

        Matrix<T> result(m.rows(), m.cols() - n_cols);

        // Copia os dados restantes
        for (size_t i = 0; i < m.rows(); ++i)
        {
            for (size_t j = 0; j < result.cols(); ++j)
            {
                result(i, j) = m(i, j);
            }
        }

        return result;
    }

    template <typename T>
    static Matrix<T> replacerows(const Matrix<T> &m, size_t index, const Matrix<T> &new_rows)
    {
        if (index + new_rows.rows() > m.rows())
        {
            throw std::invalid_argument(_red + "\nMatrixShape::replacerows() Error: Replacement rows exceed matrix dimensions.\n" + _reset);
        }

        Matrix<T> result = m;
        for (size_t i = 0; i < new_rows.rows(); ++i)
        {
            for (size_t j = 0; j < m.cols(); ++j)
            {
                result(index + i, j) = new_rows(i, j);
            }
        }
        return result;
    }

    template <typename T>
    static Matrix<T> replacecols(const Matrix<T> &m, size_t index, const Matrix<T> &new_cols)
    {
        if (index + new_cols.cols() > m.cols())
        {
            throw std::invalid_argument(_red + "\nMatrixShape::replacecols() Error: Replacement rows exceed matrix dimensions.\n" + _reset);
        }

        Matrix<T> result = m;
        for (size_t j = 0; j < new_cols.cols(); ++j)
        {
            for (size_t i = 0; i < m.rows(); ++i)
            {
                result(i, index + j) = new_cols(i, j);
            }
        }
        return result;
    }
};
