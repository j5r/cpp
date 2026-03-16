
#pragma once
#include <fstream>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <random>
#include <type_traits>
#include <typeinfo>
#include "j5rassert.cpp" // Para usar a macro de assert personalizada

int j5r(size_t seconds = 5)
{
    auto command = "cmd /c timeout " + std::to_string(seconds);
    system(command.c_str());
    return 0;
}



std::string _red = "\033[1;31m";     // Vermelho brilhante
std::string _blue = "\033[1;34m";    // Azul brilhante
std::string _green = "\033[1;32m";   // Verde brilhante
std::string _yellow = "\033[1;33m";  // Amarelo brilhante
std::string _cyan = "\033[1;36m";    // Ciano brilhante
std::string _magenta = "\033[1;35m"; // Magenta brilhante
std::string _reset = "\033[0m";      // Resetar cor

void not_implemented(const std::string &method_name)
{
    std::cout << _blue + "\nMethod " + method_name + " is not implemented yet.\n" + _reset;
}

void aqui()
{
    std::cout << _blue + "\nReached the 'aqui()' breakpoint. Press Enter to continue...\n" + _reset;
    std::cin.get();
}

template <typename T>
T inf_() { return std::numeric_limits<T>::infinity(); }

template <typename T>
T nan_() { return std::numeric_limits<T>::quiet_NaN(); }

template <typename T>
bool isinf_(T value) { return std::isinf(value); }

template <typename T>
bool isnan_(T value) { return std::isnan(value); }

template <typename T>
class Matrix
{
private:
    size_t rows_;
    size_t cols_;
    std::vector<T> data_;
    std::string comment = "";

    inline static size_t print_precision_ = 6;
    inline static size_t print_width_ = 6;
    inline static size_t print_color_ = 3; // 1-red, 2-green, 3-yellow, 4-blue, 5-magenta, 6-cyan

public:
    // Construtores
    Matrix() : rows_(0), cols_(0) {}
    Matrix(size_t rows, size_t cols) : rows_(rows), cols_(cols), data_(rows * cols, T()) {}
    Matrix(size_t rows, size_t cols, T initial_value) : rows_(rows), cols_(cols), data_(rows * cols, initial_value) {}

    // Getters e Setters
    static void set_print_precision(size_t precision) { Matrix<T>::print_precision_ = precision; }
    static void set_print_width(size_t width) { Matrix<T>::print_width_ = width; }
    static void set_print_color(size_t color_int) { Matrix<T>::print_color_ = (color_int % 6) + 1; }
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }
    size_t ilrow() const { return rows_ - 1; }                                               // index of last row (ilrow)
    size_t ilcol() const { return cols_ - 1; }                                               // index of last column (ilcol)
    std::string size() const { return std::to_string(rows_) + "x" + std::to_string(cols_); } // [Conveniência para mensagens de erro e comentários]
    void printsize() const { std::cout << rows_ << "x" << cols_; }                           // [Método de conveniência para imprimir o tamanho da matriz em mensagens]
    bool is_empty() const { return rows_ == 0 || cols_ == 0; }
    bool is_square() const { return rows_ == cols_; }
    bool is_scalar() const { return rows_ == 1 && cols_ == 1; }
    bool is_rowvector() const { return rows_ == 1 && cols_ > 1; }
    bool is_colvector() const { return cols_ == 1 && rows_ > 1; }
    bool is_symmetric() const { return this->is_square() && (this->equals(this->t())); }

    Matrix<T> msg(const std::string &comment)
    {
        this->comment = comment;
        return *this;
    }
    void set(size_t row, size_t col, T value)
    {
        if (row >= rows_ || col >= cols_)
        {
            throw std::out_of_range("\nMatrix::set() Index out of range.\n");
        }
        data_[row * cols_ + col] = value;
    }
    T get(size_t row, size_t col) const
    {
        if (row >= rows_ || col >= cols_)
        {
            throw std::out_of_range("\nMatrix::get() Index out of range.\n");
        }
        return data_[row * cols_ + col];
    }

    // Métodos (As implementações ficam aqui dentro mesmo)
    void from_prompt(size_t rows, size_t cols)
    {
        this->rows_ = rows;
        this->cols_ = cols;
        data_.resize(rows * cols);
        std::cout << "\nMatrix::from_prompt() Type " << (rows * cols) << " values:\n";
        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                std::cout << "(" << i << ", " << j << "):\t";
                std::cin >> this->operator()(i, j);
            }
        }
    }

    void from_prompt()
    {
        size_t r, c;
        std::cout << "Matrix::from_prompt() Type the number of rows: ";
        std::cin >> r;
        std::cout << "Matrix::from_prompt() Type the number of columns: ";
        std::cin >> c;
        this->from_prompt(r, c);
    }

    void print() const
    {
        size_t width = print_width_;
        size_t color_int = print_color_;
        size_t precision = print_precision_;

        // --- AUTOCORREÇÃO DE GRID INTELIGENTE E OTIMIZADA ---
        bool skip_rows = rows_ > 10;
        bool skip_cols = cols_ > 8;

        int max_int_chars = 1;

        for (size_t i = 0; i < rows_; ++i)
        {
            // [Se a matriz for grande, salta o miolo das linhas]
            if (skip_rows && i == 3)
            {
                i = rows_ - 4;
                continue;
            }

            for (size_t j = 0; j < cols_; ++j)
            {
                // [Se a matriz for grande, salta o miolo das colunas]
                if (skip_cols && j == 3)
                {
                    j = cols_ - 4;
                    continue;
                }

                T val = this->get(i, j);

                if (!std::isnan(val) && !std::isinf(val))
                {
                    long long int_part = std::abs((long long)val);
                    int chars = std::to_string(int_part).length();

                    if (chars > max_int_chars)
                    {
                        max_int_chars = chars;
                    }
                }
            }
        }

        size_t min_safe_width = max_int_chars + 1 + precision + 2;

        if (width < min_safe_width)
        {
            width = min_safe_width;
        }
        // ----------------------------------------------------

        auto color_msg = color_int != 6 ? _cyan : _magenta;
        std::string color = "\033[1;3" + std::to_string(color_int) + "m"; // Azul brilhante
        j5r_assert(width >= 5,
                   "\nMatrix::print() Width must be at least 5 characters wide, you provided: " + std::to_string(width) + ".\n");
        std::cout << color + "Matrix: (" << rows_ << " x " << cols_ << ") [ " + color_msg + this->comment + color + "\n";

        // 1. Padronizamos TODOS os prefixos de linha para terem exatamente 4 caracteres.
        std::cout << "   + ";

        for (size_t j = 0; j < cols_; ++j)
        {
            if (skip_cols && j == 3)
            {
                // 2. Padronizamos a coluna: 2 espaços + (width - 2) caracteres de texto
                std::cout << "  " << std::setw(width - 2) << std::left << "...";
                j = cols_ - 4;
                continue;
            }
            std::cout << "  " << std::setw(width - 2) << std::left << j;
        }
        std::cout << "\n";

        for (size_t i = 0; i < rows_; ++i)
        {

            if (skip_rows && i == 3)
            {
                std::cout << color + "   :|" + _reset; // Prefixo de 4 caracteres
                for (size_t j = 0; j < cols_; ++j)
                {
                    if (skip_cols && j == 3)
                    {
                        std::cout << "  " << std::setw(width - 2) << std::left << "...";
                        j = cols_ - 4;
                        continue;
                    }
                    std::cout << "  " << std::setw(width - 2) << std::left << ":";
                }
                std::cout << "\n";
                i = rows_ - 4;
                continue;
            }
            // O prefixo de linha agora é padronizado para 4 caracteres, alinhado à direita, e colorido
            std::cout << color << std::setw(4) << std::right << i << "|" + _reset; // Prefixo de 4 caracteres

            for (size_t j = 0; j < cols_; ++j)
            {
                if (skip_cols && j == 3)
                {
                    std::cout << "  " << std::setw(width - 2) << std::left << "...";
                    j = cols_ - 4;
                    continue;
                }

                T val = this->get(i, j);

                // 3. Devolvemos o sinal negativo que se perdia com o abs_val
                if (val >= 0 || isnan_(val))
                {
                    std::cout << "  "; // 2 espaços alinhadores para positivos
                }
                else
                {
                    std::cout << " -"; // 1 espaço + o sinal que foi removido do abs_val
                }

                T abs_val = (val >= 0) ? val : -val;
                if (std::isnan(val))
                {
                    // 4. Se for NaN, imprimimos "NaN" centralizado no espaço disponível
                    std::cout << std::setw(width - 2) << std::left << "NaN";
                }
                else if (std::isinf(val))
                {
                    // 5. Se for Inf, imprimimos "Inf" centralizado no espaço disponível
                    std::cout << std::setw(width - 2) << std::left << "Inf";
                }
                else
                {
                    // 6. Para números normais, aplicamos a formatação com precisão adaptada ao width

                    // std::cout << std::setprecision(6) << std::setw(width - 2) << std::left << abs_val;
                    //  [O precision agora se adapta ao width. O -3 desconta o espaço do ponto decimal e margem de segurança]
                    std::cout << std::fixed << std::setprecision(precision) << std::setw(width - 2) << std::left << abs_val;
                }
            }
            std::cout << "\n";
        }
        std::cout << std::defaultfloat; // [Reseta para notação normal para evitar que o modo fixed afete outras partes do programa]
        std::cout << color + "] type fingerprint: [" + typeid(*this).name() +"]\n" + _reset;
    }

    // --- MÉTODOS DE ARQUIVO ---

    // [Adicionamos o parâmetro 'separator' com o valor padrão ';']
    void save_to_file(const std::string &filename, char separator = ';') const
    {
        std::ofstream file(filename);

        if (!file.is_open())
        {
            throw std::runtime_error("\nMatrix::save_to_file() Error: Could not open file for writing.\n");
        }
        // --- A MÁGICA DA PRECISÃO MÁXIMA ---
        // Diz ao arquivo para usar a precisão máxima matematicamente possível para o tipo T
        file << std::setprecision(std::numeric_limits<T>::max_digits10);
        // -----------------------------------

        // 1. Guarda o cabeçalho com as dimensões (também divididas pelo separador)
        file << rows_ << separator << cols_ << "\n";

        // 2. Guarda os dados
        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                file << this->operator()(i, j);
                // [Evita colocar o separador no final da linha, mantendo o CSV limpo]
                if (j < cols_ - 1)
                {
                    file << separator;
                }
            }
            file << "\n";
        }

        file.close();
    }

    void load_from_file(const std::string &filename, char separator = ';')
    {
        std::ifstream file(filename);

        if (!file.is_open())
        {
            throw std::runtime_error("\nMatrix::load_from_file() Error: Could not open file for reading.\n");
        }

        std::stringstream ss;
        std::string line;

        // Lê o ficheiro e troca todos os separadores por espaços simples
        while (std::getline(file, line))
        {
            if (separator != ' ')
            {
                for (char &c : line)
                {
                    if (c == separator)
                    {
                        c = ' ';
                    }
                }
            }
            ss << line << " ";
        }
        file.close();

        size_t new_rows, new_cols;

        // Tenta extrair as dimensões primeiro
        if (!(ss >> new_rows >> new_cols))
        {
            throw std::runtime_error(_red + "\nMatrix::load_from_file() Integrity error: File is empty or has invalid format.\n" + _reset);
        }

        size_t total_elements = new_rows * new_cols;

        // [Criamos um vetor temporário. Se houver erro, a matriz original intacta]
        std::vector<T> temp_data(total_elements);

        // 3. Verificação de integridade (Faltam dados?)
        for (size_t i = 0; i < total_elements; ++i)
        {
            if (!(ss >> temp_data[i]))
            {
                throw std::runtime_error(_red + "\nMatrix::load_from_file() Integrity error: Entries in the matrix are missing or are of incorrect data type.\n" + _reset);
            }
        }

        // 4. Verificação de integridade (Existem dados a mais?)
        T extra_data;
        if (ss >> extra_data)
        {
            throw std::runtime_error(_red + "\nMatrix::load_from_file() Integrity error: Too much entries in the matrix from the loaded file.\n" + _reset);
        }

        // Se o código chegou até aqui sem lançar erros, a validação foi um sucesso.
        // [Atualizamos o objeto atual com os dados validados]
        this->rows_ = new_rows;
        this->cols_ = new_cols;
        this->data_ = std::move(temp_data); // [std::move: transfere a memória rapidamente sem fazer cópias desnecessárias]
    }

    // --- MÉTODOS DE ALEATORIEDADE ---

    // --- MÉTODOS DE ÁLGEBRA LINEAR ---

    // --- MÉTODOS DE ACESSO ---

    // 1. Versão para leitura e escrita (retorna a referência da memória)
    T &operator()(size_t i, size_t j)
    {
        return data_[i * cols_ + j];
    }

    // 2. Versão apenas para leitura (usada quando a matriz é const)
    const T &operator()(size_t i, size_t j) const
    {
        return data_[i * cols_ + j];
    }

    // --- MÉTODOS DE ÁLGEBRA LINEAR ---

    // [Retorna uma nova Matrix e é assinalado como const para proteger a matriz original]
    Matrix diag() const
    {
        size_t limit = std::min(rows_, cols_);

        // [Cria um vetor coluna de tamanho 'limit x 1']
        Matrix result(limit, 1);

        for (size_t i = 0; i < limit; ++i)
        {
            // [Copia o valor da coordenada (i, i) da matriz atual para a linha 'i', coluna '0' do resultado]
            result(i, 0) = data_[i * cols_ + i];
        }

        return result;
    }

    Matrix<T> t() const
    {
        Matrix<T> result(cols_, rows_); // Inverte as dimensões

        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                result(j, i) = this->operator()(i, j); // Transpõe os elementos
            }
        }

        return result;
    }

    Matrix<T> sum(uint8_t axis = 0) const
    {
        if (axis == 0)
        {
            // Soma ao longo das colunas, resultando em um vetor linha
            Matrix<T> result(1, cols_);

            for (size_t j = 0; j < cols_; ++j)
            {
                T sum = T();
                for (size_t i = 0; i < rows_; ++i)
                {
                    sum += this->operator()(i, j);
                }
                result(0, j) = sum;
            }

            return result;
        }
        else if (axis == 1)
        {
            // Soma ao longo das linhas, resultando em um vetor coluna
            Matrix<T> result(rows_, 1);

            for (size_t i = 0; i < rows_; ++i)
            {
                T sum = T();
                for (size_t j = 0; j < cols_; ++j)
                {
                    sum += this->operator()(i, j);
                }
                result(i, 0) = sum;
            }

            return result;
        }
        else
        {
            throw std::invalid_argument(_red + "\nMatrix::sum() Error: Axis must be 0 (sum each column, return a row vector)\nor 1 (sum each row, return a column vector).\n" + _reset);
        }
    }

    Matrix<T> mean(uint8_t axis = 0) const
    {
        Matrix<T> sum_result = this->sum(axis);

        if (axis == 0)
        {
            // Divide cada elemento do vetor linha pela quantidade de linhas para obter a média
            for (size_t j = 0; j < sum_result.cols(); ++j)
            {
                sum_result(0, j) /= static_cast<T>(this->rows());
            }
        }
        else if (axis == 1)
        {
            // Divide cada elemento do vetor coluna pela quantidade de colunas para obter a média
            for (size_t i = 0; i < sum_result.rows(); ++i)
            {
                sum_result(i, 0) /= static_cast<T>(this->cols());
            }
        }

        return sum_result;
    }

    Matrix<T> var(uint8_t axis = 0) const
    {
        Matrix<T> mean_result = this->mean(axis);
        Matrix<T> var_result(mean_result.rows(), mean_result.cols());

        if (axis == 0)
        {
            for (size_t j = 0; j < this->cols(); ++j)
            {
                T sum_squared_diff = T();
                for (size_t i = 0; i < this->rows(); ++i)
                {
                    T diff = this->operator()(i, j) - mean_result(0, j);
                    sum_squared_diff += diff * diff;
                }
                var_result(0, j) = sum_squared_diff / static_cast<T>(this->rows());
            }
        }
        else if (axis == 1)
        {
            for (size_t i = 0; i < this->rows(); ++i)
            {
                T sum_squared_diff = T();
                for (size_t j = 0; j < this->cols(); ++j)
                {
                    T diff = this->operator()(i, j) - mean_result(i, 0);
                    sum_squared_diff += diff * diff;
                }
                var_result(i, 0) = sum_squared_diff / static_cast<T>(this->cols());
            }
        }

        return var_result;
    }

    Matrix<T> std(uint8_t axis = 0) const
    {
        Matrix<T> var_result = this->var(axis);
        Matrix<T> std_result(var_result.rows(), var_result.cols());

        for (size_t i = 0; i < var_result.rows(); ++i)
        {
            for (size_t j = 0; j < var_result.cols(); ++j)
            {
                std_result(i, j) = std::sqrt(var_result(i, j));
            }
        }

        return std_result;
    }

    Matrix<T> getrows(size_t start, size_t end) const
    {
        if (start >= rows_ || end > rows_ || start >= end)
        {
            throw std::out_of_range(_red + "\nMatrix::getrows() Error: Invalid row indices. Got start=" +
                                    std::to_string(start) + " and end=" + std::to_string(end) + "." + _reset);
        }

        Matrix<T> result(end - start, cols_);

        for (size_t i = start; i < end; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                result(i - start, j) = this->operator()(i, j);
            }
        }

        return result;
    }

    Matrix<T> getrow(size_t index) const
    {
        if (index > this->ilrow())
        {

            throw std::out_of_range(_red + "\nMatrix::getrow() Error: Invalid row index. Got index=" +
                                    std::to_string(index) + "." + _reset);
        }

        return this->getrows(index, index + 1);
    }

    Matrix<T> getcols(size_t start, size_t end) const
    {
        if (start >= cols_ || end > cols_ || start >= end)
        {
            throw std::out_of_range(_red + "\nMatrix::getcols() Error: Invalid column indices. Got start=" +
                                    std::to_string(start) + " and end=" + std::to_string(end) + "." + _reset);
        }

        Matrix<T> result(rows_, end - start);

        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = start; j < end; ++j)
            {
                result(i, j - start) = this->operator()(i, j);
            }
        }

        return result;
    }

    Matrix<T> getcol(size_t index) const
    {
        if (index > this->ilcol())
        {

            throw std::out_of_range(_red + "\nMatrix::getcol() Error: Invalid column index. Got index=" +
                                    std::to_string(index) + "." + _reset);
        }

        return this->getcols(index, index + 1);
    }

    Matrix<T> reshape(size_t new_rows, size_t new_cols) const
    {
        if (new_rows * new_cols != rows_ * cols_)
        {
            throw std::invalid_argument(_red + "\nMatrix::reshape() Error: New dimensions must contain the same total number of elements.\nGot " +
                                        std::to_string(new_rows) + "x" + std::to_string(new_cols) + " = " + std::to_string(new_rows * new_cols) + " but expected " +
                                        std::to_string(rows_ * cols_) + " elements. Current size is " + size() + "." + _reset);
        }

        Matrix<T> result(new_rows, new_cols);

        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                size_t index = i * cols_ + j;
                size_t new_i = index / new_cols;
                size_t new_j = index % new_cols;
                result(new_i, new_j) = this->operator()(i, j);
            }
        }

        return result;
    }

    Matrix<T> rmrows(size_t start, size_t end) const
    {
        if (start >= rows_ || end > rows_ || start >= end)
        {
            throw std::out_of_range(_red + "\nMatrix::rmrows() Error: Invalid row indices. Got start=" +
                                    std::to_string(start) + " and end=" + std::to_string(end) + "." + _reset);
        }

        Matrix<T> result(rows_ - (end - start), cols_);

        size_t new_i = 0;
        for (size_t i = 0; i < rows_; ++i)
        {
            if (i >= start && i < end)
            {
                continue; // Pula as linhas que devem ser removidas
            }
            for (size_t j = 0; j < cols_; ++j)
            {
                result(new_i, j) = this->operator()(i, j);
            }
            new_i++;
        }

        return result;
    }

    Matrix<T> rmrow(size_t index) const
    {
        if (index > this->ilrow())
        {

            throw std::out_of_range(_red + "\nMatrix::rmrow() Error: Invalid row index. Got index=" +
                                    std::to_string(index) + "." + _reset);
        }

        return this->rmrows(index, index + 1);
    }

    Matrix<T> rmcols(size_t start, size_t end) const
    {
        if (start >= cols_ || end > cols_ || start >= end)
        {
            throw std::out_of_range(_red + "\nMatrix::rmcols() Error: Invalid column indices. Got start=" +
                                    std::to_string(start) + " and end=" + std::to_string(end) + "." + _reset);
        }

        Matrix<T> result(rows_, cols_ - (end - start));

        for (size_t i = 0; i < rows_; ++i)
        {
            size_t new_j = 0;
            for (size_t j = 0; j < cols_; ++j)
            {
                if (j >= start && j < end)
                {
                    continue; // Pula as colunas que devem ser removidas
                }
                result(i, new_j) = this->operator()(i, j);
                new_j++;
            }
        }

        return result;
    }

    Matrix<T> rmcol(size_t index) const
    {
        if (index > this->ilcol())
        {

            throw std::out_of_range(_red + "\nMatrix::rmcol() Error: Invalid column index. Got index=" +
                                    std::to_string(index) + "." + _reset);
        }

        return this->rmcols(index, index + 1);
    }

    Matrix<T> flatten() const
    {
        Matrix<T> result(rows_ * cols_, 1);

        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                size_t index = i * cols_ + j;
                result(index, 0) = this->operator()(i, j);
            }
        }

        return result;
    }

    template <typename U>
    auto operator+(const Matrix<U> &other) const
    {
        if (rows_ != other.rows() || cols_ != other.cols())
        {
            throw std::invalid_argument(_red + "\nMatrix::operator+() Error: Matrices must have the same dimensions for addition.\nCurrent size is " + size() + " but got " + other.size() + "." + _reset);
        }
        using ResultType = std::common_type_t<T, U>;
        Matrix<ResultType> result(rows_, cols_);

        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                result(i, j) = this->operator()(i, j) + other(i, j);
            }
        }

        return result;
    }

    template <typename U>
    auto operator+(U scalar) const
    {
        using ResultType = std::common_type_t<T, U>;
        Matrix<ResultType> result(rows_, cols_);

        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                result(i, j) = this->operator()(i, j) + scalar;
            }
        }

        return result;
    }

    template <typename U>
    auto operator-(const Matrix<U> &other) const
    {
        if (rows_ != other.rows() || cols_ != other.cols())
        {
            throw std::invalid_argument(_red + "\nMatrix::operator-() Error: Matrices must have the same dimensions for subtraction.\nCurrent size is " + size() + " but got " + other.size() + "." + _reset);
        }
        using ResultType = std::common_type_t<T, U>;
        Matrix<ResultType> result(rows_, cols_);

        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                result(i, j) = this->operator()(i, j) - other(i, j);
            }
        }

        return result;
    }

    template <typename U>
    auto operator-(U scalar) const
    {
        using ResultType = std::common_type_t<T, U>;
        Matrix<ResultType> result(rows_, cols_);

        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                result(i, j) = this->operator()(i, j) - scalar;
            }
        }

        return result;
    }

    template <typename U>
    auto operator*(const Matrix<U> &other) const
    {
        if (cols_ != other.rows())
        {
            throw std::invalid_argument(_red + "\nMatrix::operator*() Error: Number of columns of the first matrix must equal the number of rows of the second matrix for multiplication.\nCurrent size is " + size() + " but got " + other.size() + "." + _reset);
        }

        using ResultType = std::common_type_t<T, U>;
        Matrix<ResultType> result(rows_, other.cols());

        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < other.cols(); ++j)
            {
                ResultType sum = ResultType();
                for (size_t k = 0; k < cols_; ++k)
                {
                    sum += this->operator()(i, k) * other(k, j);
                }
                result(i, j) = sum;
            }
        }

        return result;
    }

    template <typename U>
    auto operator*(U scalar) const
    {
        using ResultType = std::common_type_t<T, U>;
        Matrix<ResultType> result(rows_, cols_);

        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                result(i, j) = this->operator()(i, j) * scalar;
            }
        }

        return result;
    }

    template <typename U>
    auto operator/(U scalar) const
    {
        using ResultType = std::common_type_t<T, U>;
        Matrix<ResultType> result(rows_, cols_);
        ResultType inf = inf_<ResultType>();
        ResultType nan = nan_<ResultType>();

        if (scalar == U()) // den 0
        {

            for (size_t i = 0; i < rows_; ++i)
            {
                for (size_t j = 0; j < cols_; ++j)
                {
                    auto numerator = this->operator()(i, j);
                    if (numerator == T()) // num 0
                    {
                        result(i, j) = nan;
                    }
                    else if (isinf_(numerator)) // num inf
                    {
                        result(i, j) = nan;
                    }
                    else if (isnan_(numerator)) // num nan
                    {
                        result(i, j) = nan;
                    }
                    else
                    {
                        result(i, j) = numerator > 0 ? inf : -inf;
                    }
                }
            }
            return result;
        }

        if (isinf_(scalar)) // den inf
        {
            for (size_t i = 0; i < rows_; ++i)
            {
                for (size_t j = 0; j < cols_; ++j)
                {
                    auto numerator = this->operator()(i, j);
                    if (isinf_(numerator))
                    {
                        result(i, j) = nan; // Inf dividido por Inf é indeterminado (NaN)
                    }
                    else if (isnan_(numerator))
                    {
                        result(i, j) = nan; // NaN dividido por Inf é indeterminado (NaN)
                    }
                    else if (numerator == T())
                    {
                        result(i, j) = nan; // 0 dividido por Inf é indeterminado (NaN)
                    }
                    else
                    {
                        result(i, j) = 0; // Qualquer número finito dividido por infinito é zero
                    }
                }
            }
            return result;
        }

        if (isnan_(scalar)) // den NaN
        {
            for (size_t i = 0; i < rows_; ++i)
            {
                for (size_t j = 0; j < cols_; ++j)
                {
                    result(i, j) = nan; // Qualquer número dividido por NaN é NaN
                }
            }
            return result;
        }

        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                result(i, j) = this->operator()(i, j) / scalar;
            }
        }

        return result;
    }

    template <typename U>
    auto operator/(const Matrix<U> &other) const
    {
        if (cols_ != other.cols() || rows_ != other.rows())
        {
            throw std::invalid_argument(_red + "\nMatrix::operator/() Error: Incompatible matrix dimensions for element-wise division.\nCurrent size is " + size() + " but got " + other.size() + "." + _reset);
        }

        using ResultType = std::common_type_t<T, U>;
        Matrix<ResultType> result(rows_, cols_);

        ResultType inf = inf_<ResultType>();
        ResultType nan = nan_<ResultType>();

        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                U denominator = other(i, j);
                auto numerator = this->operator()(i, j);

                if (denominator == U()) // den 0
                {
                    if (numerator == T()) // num 0
                    {
                        result(i, j) = nan;
                    }
                    else if (isinf_(numerator)) // num inf
                    {
                        result(i, j) = nan;
                    }
                    else if (isnan_(numerator)) // num nan
                    {
                        result(i, j) = nan;
                    }
                    else
                    {
                        result(i, j) = (numerator / denominator) >= 0 ? inf : -inf;
                    }
                }
                else if (isinf_(denominator)) // den inf
                {
                    if (numerator == T()) // num 0
                    {
                        result(i, j) = nan;
                    }
                    else if (isinf_(numerator)) // num inf
                    {
                        result(i, j) = nan;
                    }
                    else if (isnan_(numerator)) // num nan
                    {
                        result(i, j) = nan;
                    }
                    else
                    {
                        result(i, j) = 0;
                    }
                }
                else if (isnan_(denominator)) // den nan
                {
                    result(i, j) = nan;
                }
                else
                {
                    result(i, j) = numerator / denominator;
                }
            }
        }
        return result;
    }

    // --- OPERAÇÕES COM ESCALAR À ESQUERDA ---

    // [friend: declara uma função não-membro que tem acesso direto aos dados da classe]
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator+(const U &scalar, const Matrix &m)
    {
        return m + scalar;
    }

    // [Opcional: Exemplo para subtração, onde a ordem importa muito]
    template <typename U>
    friend auto operator-(const U &scalar, const Matrix &m)
    {
        using ResultType = std::common_type_t<U, T>;
        Matrix<ResultType> result(m.rows(), m.cols());

        for (size_t i = 0; i < m.rows(); ++i)
        {
            for (size_t j = 0; j < m.cols(); ++j)
            {
                result(i, j) = scalar - m(i, j); // Aqui a ordem dita a matemática correta
            }
        }
        return result;
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator*(const U &scalar, const Matrix &m)
    {
        return m * scalar;
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator/(const U &scalar, const Matrix &m)
    {
        using ResultType = std::common_type_t<U, T>;
        Matrix<ResultType> result(m.rows(), m.cols());

        ResultType inf = inf_<ResultType>();
        ResultType nan = nan_<ResultType>();

        for (size_t i = 0; i < m.rows(); ++i)
        {
            for (size_t j = 0; j < m.cols(); ++j)
            {
                T denominator = m(i, j);
                if (scalar == U()) // num 0
                {
                    if (denominator == T())
                    {
                        result(i, j) = nan; // 0 dividido por 0 é indeterminado (NaN)
                    }
                    else if (isinf_(denominator))
                    {
                        result(i, j) = nan; // Inf dividido por 0 é indeterminado (NaN)
                    }
                    else if (isnan_(denominator))
                    {
                        result(i, j) = nan; // NaN dividido por 0 é indeterminado (NaN)
                    }
                    else
                    {
                        result(i, j) = 0; // Qualquer número finito dividido por zero é zero
                    }
                }
                else if (isinf_(scalar)) // num inf
                {
                    if (denominator == T())
                    {
                        result(i, j) = nan; // Inf dividido por 0 é indeterminado (NaN)
                    }
                    else if (isinf_(denominator))
                    {
                        result(i, j) = nan; // Inf dividido por Inf é indeterminado (NaN)
                    }
                    else if (isnan_(denominator))
                    {
                        result(i, j) = nan; // Inf dividido por NaN é indeterminado (NaN)
                    }
                    else
                    {
                        result(i, j) = (scalar / denominator) >= 0 ? inf : -inf; // Inf dividido por qualquer número finito é Inf ou -Inf dependendo do sinal
                    }
                }
                else if (isnan_(scalar)) // num nan
                {
                    result(i, j) = nan; // NaN dividido por qualquer número é NaN
                }
                else
                {
                    if (denominator == T()) // den 0
                    {
                        result(i, j) = (scalar / denominator) >= 0 ? inf : -inf; // Qualquer número finito dividido por zero é Inf ou -Inf dependendo do sinal
                    }
                    else if (isinf_(denominator)) // den inf
                    {
                        result(i, j) = 0; // Qualquer número finito dividido por infinito é zero
                    }
                    else if (isnan_(denominator)) // den nan
                    {
                        result(i, j) = nan; // Qualquer número dividido por NaN é NaN
                    }
                    else
                    {
                        result(i, j) = scalar / denominator; // Divisão normal para casos normais
                    }
                }
            }
        }
        return result;
    }

    bool equals(const Matrix<T> &other, T tolerance = static_cast<T>(1e-6)) const
    {
        if (rows_ != other.rows() || cols_ != other.cols())
        {
            return false;
        }

        T sum_ = T();
        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                T diff = std::abs(this->operator()(i, j) - other(i, j));
                sum_ += diff;
                if (sum_ > tolerance)
                {
                    return false;
                }
            }
        }

        return true;
    }
    Matrix<T> operator-() const { return 0 - (*this); }

    Matrix<T> ew_prod(const Matrix<T> &other) const
    {
        if (rows_ != other.rows() || cols_ != other.cols())
        {
            throw std::invalid_argument(_red + "\nMatrix::prod() Error: Matrices must have the same dimensions for element-wise multiplication.\nCurrent size is " + size() + " but got " + other.size() + "." + _reset);
        }

        Matrix<T> result(rows_, cols_);

        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                result(i, j) = this->operator()(i, j) * other(i, j);
            }
        }

        return result;
    }

    static void help()
    {
        std::cout << "\nMatrix Class Help:\n\n";
        std::cout << "1. Constructors:\n";
        std::cout << "   - Matrix(): Default constructor creates an empty matrix.\n";
        std::cout << "   - Matrix(size_t rows, size_t cols): Creates a matrix with specified dimensions initialized to zero.\n";
        std::cout << "   - Matrix(const std::vector<std::vector<T>>& values): Creates a matrix from a 2D vector.\n\n";

        std::cout << "2. Basic Properties:\n";
        std::cout << "   - rows(): Returns the number of rows.\n";
        std::cout << "   - cols(): Returns the number of columns.\n";
        std::cout << "   - size(): Returns a string representation of the matrix size (e.g., '3x4').\n";
        std::cout << "   - is_square(): Checks if the matrix is square.\n";
        std::cout << "   - is_rowvector(): Checks if the matrix is a row vector.\n";
        std::cout << "   - is_colvector(): Checks if the matrix is a column vector.\n";
        std::cout << "   - is_symmetric(): Checks if the matrix is symmetric.\n\n";

        std::cout << "3. Data Access:\n";
        std::cout << "   - set(size_t row, size_t col, T value): Sets the value at the specified position.\n";
        std::cout << "   - get(size_t row, size_t col): Gets the value at the specified position.\n";
        std::cout << "   - operator()(size_t i, size_t j): Provides access to elements for reading and writing.\n\n";

        std::cout << "4. Input/Output:\n";
        std::cout << "   - from_prompt(size_t rows, size_t cols): Fills the matrix with user input values.\n";
        std::cout << "   - from_prompt(): Prompts for dimensions and then fills the matrix with user input values.\n";
        std::cout << "   - print(size_t width = 9, size_t color_int = 3): Prints the matrix with formatting options.\n\n";

        std::cout << "5. File Operations:\n";
        std::cout << "   - save_to_file(const std::string &filename, char separator = ';'): Saves the matrix to a file with an optional separator.\n";
        std::cout << "   - load_from_file(const std::string &filename, char separator = ';'): Loads the matrix from a file with an optional separator.\n\n";

        std::cout << "6. Math operators:\n";
        std::cout << "   - operator+(const Matrix<T> &other): Matrix addition.\n";
        std::cout << "   - operator+(T scalar): Adds a scalar to each element of the matrix.\n";
        std::cout << "   - operator-(const Matrix<T> &other): Matrix subtraction.\n";
        std::cout << "   - operator-(T scalar): Subtracts a scalar from each element of the matrix.\n";
        std::cout << "   - operator*(const Matrix<T> &other): Matrix multiplication.\n";
        std::cout << "   - operator*(T scalar): Multiplies each element of the matrix by a scalar.\n";
        std::cout << "   - operator/(T scalar): Divides each element of the matrix by a scalar.\n";
        std::cout << "   - operator/(const Matrix<T> &other): Element-wise division of two matrices.\n";
        std::cout << "   - operator-() const: Negation of the matrix (multiplies all elements by -1).\n";
        std::cout << "   - ew_prod(const Matrix<T> &other): Element-wise product of two matrices.\n\n";
    }

    Matrix<T> copy() const
    {
        Matrix<T> result(rows_, cols_);

        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                result(i, j) = this->operator()(i, j);
            }
        }

        return result;
    }

    Matrix<T> pow(int exponent) const
    {
        if (exponent < 0)
        {
            throw std::invalid_argument(_red + "\nMatrix::pow() Error: Exponent must be a non-negative integer.\nGot exponent=" + std::to_string(exponent) + "." + _reset);
        }

        if (!is_square())
        {
            throw std::invalid_argument(_red + "\nMatrix::pow() Error: Only square matrices can be raised to a power.\nCurrent size is " + size() + "." + _reset);
        }

        Matrix<T> result(rows_, cols_);
        // Inicializa result como a matriz identidade
        for (size_t i = 0; i < rows_; ++i)
        {
            result(i, i) = T(1);
        }

        if (exponent == 0)
        {
            return result; // A^0 é a matriz identidade
        }

        Matrix<T> base = *this;

        while (exponent > 0)
        {
            if (exponent % 2 == 1)
            {
                result = result * base; // Multiplica pelo resultado se o expoente for ímpar
            }
            base = base * base; // Eleva a base ao quadrado
            exponent /= 2;      // Divide o expoente por 2
        }

        return result;
    }

    Matrix<T> pivot(size_t pivot_row, size_t pivot_col) const
    {
        if (pivot_row >= rows_ || pivot_col >= cols_)
        {
            throw std::out_of_range(_red + "\nMatrix::pivot() Error: Pivot row and column indices must be within matrix dimensions.\nGot pivot_row=" + std::to_string(pivot_row) + " and pivot_col=" + std::to_string(pivot_col) + "." + _reset);
        }

        Matrix<T> result(rows_, cols_);

        /*
        implementacao do pivotamento:
        */
        not_implemented("Matrix::pivot()");
        return result;
    }


    template <typename U>
    Matrix<bool> operator>(const U&scalar) const {
        Matrix<bool> result(rows_, cols_);
        for(size_t i=0; i < rows_;i++){
            for(size_t j=0; j<cols_; j++){
            result.set(i,j,this->get(i,j) > scalar);
            }
        }
        return result;
    }

    
    template <typename U>
    Matrix<bool> operator>=(const U&scalar) const {
        Matrix<bool> result(rows_, cols_);
        for(size_t i=0; i < rows_;i++){
            for(size_t j=0; j<cols_; j++){
            result.set(i,j,this->get(i,j) >= scalar);
            }
        }
        return result;
    }


    template <typename U>
    Matrix<bool> operator<(const U&scalar) const {
        Matrix<bool> result(rows_, cols_);
        for(size_t i=0; i < rows_;i++){
            for(size_t j=0; j<cols_; j++){
            result.set(i,j,this->get(i,j) < scalar);
            }
        }
        return result;
    }


    template <typename U>
    Matrix<bool> operator<=(const U&scalar) const {
        Matrix<bool> result(rows_, cols_);
        for(size_t i=0; i < rows_;i++){
            for(size_t j=0; j<cols_; j++){
            result.set(i,j,this->get(i,j) <= scalar);
            }
        }
        return result;
    }



    template <typename U>
    Matrix<bool> operator==(const U&scalar) const {
        Matrix<bool> result(rows_, cols_);
        for(size_t i=0; i < rows_;i++){
            for(size_t j=0; j<cols_; j++){
            result.set(i,j,this->get(i,j) == scalar);
            }
        }
        return result;
    }


    template <typename U>
    Matrix<bool> operator!=(const U&scalar) const {
        Matrix<bool> result(rows_, cols_);
        for(size_t i=0; i < rows_;i++){
            for(size_t j=0; j<cols_; j++){
            result.set(i,j,this->get(i,j) != scalar);
            }
        }
        return result;
    }



    template <typename U>
    Matrix<bool> operator>(const Matrix<U>&other) const {
        if(rows_!=other.rows() || cols_!=other.cols()){
            throw std::invalid_argument(_red+
            "\nMatrix::operator>() Matrices must have the same size. Got "+size()+" vs "+other.size()+".\n"
                +_reset);
        }
        Matrix<bool> result(rows_, cols_);
        for(size_t i=0; i < rows_;i++){
            for(size_t j=0; j<cols_; j++){
            result.set(i,j,this->get(i,j) > other.get(i,j));
            }
        }
        return result;
    }


    template <typename U>
    Matrix<bool> operator<(const Matrix<U>&other) const {
        if(rows_!=other.rows() || cols_!=other.cols()){
            throw std::invalid_argument(_red+
            "\nMatrix::operator>() Matrices must have the same size. Got "+size()+" vs "+other.size()+".\n"
                +_reset);
        }
        Matrix<bool> result(rows_, cols_);
        for(size_t i=0; i < rows_;i++){
            for(size_t j=0; j<cols_; j++){
            result.set(i,j,this->get(i,j) < other.get(i,j));
            }
        }
        return result;
    }


    template <typename U>
    Matrix<bool> operator>=(const Matrix<U>&other) const {
        if(rows_!=other.rows() || cols_!=other.cols()){
            throw std::invalid_argument(_red+
            "\nMatrix::operator>() Matrices must have the same size. Got "+size()+" vs "+other.size()+".\n"
                +_reset);
        }
        Matrix<bool> result(rows_, cols_);
        for(size_t i=0; i < rows_;i++){
            for(size_t j=0; j<cols_; j++){
            result.set(i,j,this->get(i,j) >= other.get(i,j));
            }
        }
        return result;
    }


    template <typename U>
    Matrix<bool> operator<=(const Matrix<U>&other) const {
        if(rows_!=other.rows() || cols_!=other.cols()){
            throw std::invalid_argument(_red+
            "\nMatrix::operator>() Matrices must have the same size. Got "+size()+" vs "+other.size()+".\n"
                +_reset);
        }
        Matrix<bool> result(rows_, cols_);
        for(size_t i=0; i < rows_;i++){
            for(size_t j=0; j<cols_; j++){
            result.set(i,j,this->get(i,j) <= other.get(i,j));
            }
        }
        return result;
    }


    template <typename U>
    Matrix<bool> operator==(const Matrix<U>&other) const {
        if(rows_!=other.rows() || cols_!=other.cols()){
            throw std::invalid_argument(_red+
            "\nMatrix::operator>() Matrices must have the same size. Got "+size()+" vs "+other.size()+".\n"
                +_reset);
        }
        Matrix<bool> result(rows_, cols_);
        for(size_t i=0; i < rows_;i++){
            for(size_t j=0; j<cols_; j++){
            result.set(i,j,this->get(i,j) == other.get(i,j));
            }
        }
        return result;
    }


    template <typename U>
    Matrix<bool> operator!=(const Matrix<U>&other) const {
        if(rows_!=other.rows() || cols_!=other.cols()){
            throw std::invalid_argument(_red+
            "\nMatrix::operator>() Matrices must have the same size. Got "+size()+" vs "+other.size()+".\n"
                +_reset);
        }
        Matrix<bool> result(rows_, cols_);
        for(size_t i=0; i < rows_;i++){
            for(size_t j=0; j<cols_; j++){
            result.set(i,j,this->get(i,j) != other.get(i,j));
            }
        }
        return result;
    }
};

template <typename T>
Matrix<T> diag(const Matrix<T> &m)
{
    if (m.is_empty())
    {
        throw std::invalid_argument(_red +
                                    "\nFunction:diag() Empty matrix does not have diagonal. Please give a non-empty matrix.\n" + _reset);
    }

    if (m.is_rowvector() || m.is_colvector())
    {
        auto dim = m.cols() >= m.rows() ? m.cols() : m.rows();
        Matrix<T> result(dim, dim);
        for (size_t i = 0; i < dim; i++)
        {
            result(i, i) = m.is_rowvector() ? m(0, i) : m(i, 0);
        }
        return result;
    }
    auto dim = m.cols() <= m.rows() ? m.cols() : m.rows();
    Matrix<T> result(dim, dim);
    for (size_t i = 0; i < dim; i++)
    {
        result(i, i) = m(i, i);
    }
    return result;
}