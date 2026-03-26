
#pragma once
#include <fstream>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <iterator> // para std::distance
#include <stdexcept>
#include <iomanip>
#include <random>
#include <type_traits>
#include <typeinfo>
#include <initializer_list>
#include <utility>
#include <iostream>
#include <cstdint>
#include <sstream>
#define PI 3.14159265358979323846264338327950288419716939937510

template <typename T>
class Matrix;

class ostringstream_extension
{
public:
    std::ostringstream oss;

    std::string operator()()
    {
        std::string value = oss.str();
        oss.str(std::string());
        oss.clear();
        return value;
    }

    ostringstream_extension &operator<<(const std::string &txt)
    {
        oss << txt;
        return *this;
    }

    template <typename T>
    ostringstream_extension &operator<<(const T value)
    {
        oss << value;
        return *this;
    }

    std::string get() { return (*this)(); }

    template <typename T>
    Matrix<T> &operator()(Matrix<T> &m)
    {
        m.msg((*this)());
        return m;
    }
};

#include <iostream>
#include <cstdint>

struct j5r_bool
{
    uint8_t value;

    // Construtor padrão (inicia com 0)
    j5r_bool() : value(0) {}

    // Construtor Template: Aceita QUALQUER tipo numérico
    template <typename U>
    j5r_bool(U v) : value(v != static_cast<U>(0) ? 1 : 0) {}

    // Operador de Atribuição Template: Blinda QUALQUER tipo numérico
    template <typename U>
    j5r_bool &operator=(U v)
    {
        value = (v != static_cast<U>(0) ? 1 : 0);
        return *this;
    }

    // Conversões para quando a matriz precisar de ler o valor
    operator uint8_t() const { return value; }
    // O 'explicit' salva o dia! Evita que o C++ use o bool para contas matemáticas.
    explicit operator bool() const { return value != 0; }
};

inline std::ostream &operator<<(std::ostream &os, const j5r_bool &b)
{
    // O cast para int é OBRIGATÓRIO para imprimir como número (0 ou 1),
    // caso contrário o cout tentará imprimir como um caractere ASCII!
    os << static_cast<int>(b.value);
    return os;
}

inline j5r_bool operator-(const j5r_bool &b)
{
    return b;
}

// ==========================================
// INJEÇÃO NA BIBLIOTECA PADRÃO (Traits)
// ==========================================
// Ensinamos o C++ a tratar o nosso tipo customizado dentro das funções matemáticas do std
namespace std
{
    inline bool isnan(j5r_bool /*val*/)
    {
        return false; // Um bit nunca é NaN
    }

    inline bool isinf(j5r_bool /*val*/)
    {
        return false; // Um bit nunca é infinito
    }

    // 1. Quando misturar j5r_bool com j5r_bool, o resultado é j5r_bool
    template <>
    struct common_type<j5r_bool, j5r_bool>
    {
        using type = j5r_bool;
    };

    // 2. Quando misturar j5r_bool com outro tipo T (ex: int, double), trate o j5r_bool como se fosse um uint8_t
    template <typename T>
    struct common_type<j5r_bool, T>
    {
        using type = typename std::common_type<uint8_t, T>::type;
    };

    // 3. O inverso do caso 2 (ex: T misturado com j5r_bool)
    template <typename T>
    struct common_type<T, j5r_bool>
    {
        using type = typename std::common_type<T, uint8_t>::type;
    };

}

// Agora o seu alias usa o tipo blindado e a classe Matrix fica intocada!
// template <typename T> class Matrix { ... };
// using Matrixbool = Matrix<j5r_bool>;

inline ostringstream_extension strcc; // string concatenator like std::cout;
// example
//   strcc << "text " << 23 << " other text";
// to get the resulting string there are three options:
//   strcc.str();    //gets the string "text 23 other text"
//   strcc.get();    //gets the string "text 23 other text"
//   strcc();        //gets the string "text 23 other text"
//   strcc(M);       //gets the string and sets in .msg() of matrix M

inline int j5r(size_t seconds = 5)
{
    auto command = "cmd /c timeout " + std::to_string(seconds);
    system(command.c_str());
    return 0;
}

inline std::string _red = "\033[31m";     // Vermelho brilhante
inline std::string _blue = "\033[34m";    // Azul brilhante
inline std::string _green = "\033[32m";   // Verde brilhante
inline std::string _yellow = "\033[33m";  // Amarelo brilhante
inline std::string _cyan = "\033[36m";    // Ciano brilhante
inline std::string _magenta = "\033[35m"; // Magenta brilhante
inline std::string _reset = "\033[0m";    // Resetar cor

inline void not_implemented(const std::string &method_name)
{
    std::cout << _blue + "\nMethod " + method_name + " is not implemented yet.\n" + _reset;
}

inline void aqui(auto fname = __FILE__, auto _line = __LINE__)
{
    std::cout << _blue + "\nfile::" + std::string(fname) + "::row::" + std::to_string(_line) +
                     "\nReached the 'aqui(__FILE__, __LINE__)' breakpoint. Press Enter to continue...\n" + _reset;
    std::cin.get();
}

template <typename T>
inline T inf_() noexcept { return std::numeric_limits<T>::infinity(); }

template <typename T>
inline T nan_() noexcept { return std::numeric_limits<T>::quiet_NaN(); }

template <typename T>
inline bool isinf_(T value) noexcept { return std::isinf(value); }

template <typename T>
inline bool isnan_(T value) noexcept { return std::isnan(value); }

template <typename T>
class Matrix
{

    static_assert(!std::is_same_v<T, bool>,
                  "\033[5;32m\n[MATRIX ERROR]: Matrix<bool> is <<strictly prohibited>>\033[1m due to std::vector<bool> performance overhead. "
                  "\nPlease use the 'Matrixbool' alias instead! (Matrixbool is an alias for Matrix<j5r_bool>)\033[m\n");

private:
    size_t rows_;
    size_t cols_;
    std::vector<T> data_;
    std::string comment = "";

    inline static size_t print_precision_ = 6;
    inline static size_t print_width_ = 6;
    inline static size_t print_color_ = 3; // 1-red, 2-green, 3-yellow, 4-blue, 5-magenta, 6-cyan
    inline static bool print_debug_ = false;
    inline static size_t print_style_ = 1;

public:
    enum class Axis
    {
        row = 0,
        col = 1
    };
    friend class MatrixLinAlg;
    // Construtores
    Matrix() : rows_(0), cols_(0) {}
    Matrix(size_t rows, size_t cols) : rows_(rows), cols_(cols), data_(rows * cols, T()) {}
    Matrix(size_t rows, size_t cols, T initial_value) : rows_(rows), cols_(cols), data_(rows * cols, initial_value) {}

    // --- CONSTRUTOR PARA CONVERSAO 'CASTING'
    // EXEMPLO: Matrix<double> B = A
    template <typename U>
    Matrix(const Matrix<U> &other)
    {
        Matrix<T> result(other.rows(), other.cols());
        for (size_t i = 0; i < other.rows() * other.cols(); i++)
            result(i) = static_cast<T>(other(i));
        *this = result;
    }
    // --- CONVERSAO EXPLICITA DE CASTING
    // EXEMPLO: Matrix<double> B = (double) A;
    template <typename U>
    operator Matrix<U>() const
    {
        Matrix<U> result(rows_, cols_);
        for (int i = 0; i < rows_ * cols_; ++i)
        {
            // O static_cast garante a conversão segura entre tipos (ex: int -> double)
            result.data_[i] = static_cast<U>(this->data_[i]);
        }
        return result;
    }

    // --- CONSTRUTOR COM LISTA DE INICIALIZAÇÃO ---

    Matrix(std::initializer_list<std::initializer_list<T>> list)
    {
        this->rows_ = list.size();

        // Se a lista estiver vazia (ex: Matrix m = {}), cria uma matriz 0x0
        if (this->rows_ == 0)
        {
            this->cols_ = 0;
            return;
        }

        // Descobre o número de colunas olhando para o tamanho da primeira linha
        this->cols_ = list.begin()->size();

        // Aloca a memória necessária de uma só vez
        data_.resize(this->rows_ * this->cols_);

        size_t i = 0;
        for (const auto &row_list : list)
        {
            // [Barreira de segurança: Garante que a matriz é perfeitamente retangular]
            if (row_list.size() != this->cols_)
            {
                throw std::invalid_argument("\nMatrix Constructor Error: All rows must have the same number of columns.\n");
            }

            size_t j = 0;
            for (const auto &val : row_list)
            {
                this->operator()(i, j) = val;
                j++;
            }
            i++;
        }
    }

    // Getters e Setters
    static void set_print_precision(size_t precision) noexcept { Matrix<T>::print_precision_ = precision; }
    static void set_print_width(size_t width) noexcept { Matrix<T>::print_width_ = width; }
    static void set_print_color(size_t color_int) noexcept { Matrix<T>::print_color_ = (color_int % 6) + 1; }
    static void set_print_font(size_t style_int) noexcept { Matrix<T>::print_style_ = (style_int % 9) + 1; }
    static void set_print_debug(bool debug) noexcept { print_debug_ = debug; }
    static void debug(bool d) { print_debug_ = d; };
    size_t rows() const noexcept { return rows_; }
    size_t cols() const noexcept { return cols_; }
    size_t ilrow() const noexcept { return rows_ - 1; }                                               // index of last row (ilrow)
    size_t ilcol() const noexcept { return cols_ - 1; }                                               // index of last column (ilcol)
    std::string size() const noexcept { return std::to_string(rows_) + "x" + std::to_string(cols_); } // [Conveniência para mensagens de erro e comentários]
    void printsize() const noexcept { std::cout << rows_ << "x" << cols_; }                           // [Método de conveniência para imprimir o tamanho da matriz em mensagens]
    bool is_empty() const noexcept { return rows_ == 0 || cols_ == 0; }
    bool is_square() const noexcept { return rows_ == cols_; }
    bool is_scalar() const noexcept { return rows_ == 1 && cols_ == 1; }
    bool is_rowvector() const noexcept { return rows_ == 1 && cols_ > 1; }
    bool is_colvector() const noexcept { return cols_ == 1 && rows_ > 1; }
    bool is_vector() const noexcept { return is_colvector() || is_rowvector(); }
    size_t numel() const noexcept { return rows_ * cols_; }
    bool is_symmetric() const noexcept
    {
        if (!is_square())
            return false;
        for (size_t i = 0; i < rows_; i++)
        {
            for (size_t j = i + 1; j < cols_; j++)
            {
                if (std::abs(this->operator()(i, j) - this->operator()(j, i)) > 1e-14)
                    return false;
            }
        }
        return true;
    }

    Matrix<T> &msg(const std::string &comment) noexcept
    {
        this->comment = comment;
        return *this;
    }

    void set(int64_t row, int64_t col, T value)
    {
        if (row < 0)
            row += rows_;
        if (col < 0)
            col += cols_;
        if (row < 0 || col < 0 || row >= rows_ || col >= cols_)
        {
            throw std::out_of_range(
                _red + "Matrix::set() index out of range.\nRange is from row[" +
                std::to_string(-rows() + 1) + "," + std::to_string(ilrow()) + "], col[" +
                std::to_string(-cols() + 1) + "," + std::to_string(ilcol()) + "], got [i,j]=[" +
                std::to_string(row) + "," + std::to_string(col) + "]." + _reset);
        }
        data_[row * cols_ + col] = value;
    }

    void set(int64_t index, T value)
    {
        auto index_in = index;
        if (index < 0)
            index += rows() * cols();
        if (index < 0 || index >= rows() * cols())
        {
            int from_index = -rows() * cols();
            throw std::out_of_range(
                _red + "Matrix::set() index out of range.\nRange is\n  index[" +
                std::to_string(from_index) + " (same as first item), " + std::to_string(rows() * cols() - 1) + " (same as last item)],\n  but got index=" +
                std::to_string(index_in) + "." + _reset);
        }
        data_[index] = value;
    }

    T get(int64_t row, int64_t col) const
    {
        auto row_in = row;
        auto col_in = col;
        if (row < 0)
            row += rows();
        if (col < 0)
            col += cols();
        if (row < 0 || col < 0 || row >= rows() || col >= cols())
        {
            int from_row = -this->rows();
            int from_col = -this->cols();
            throw std::out_of_range(
                _red + "Matrix::get() index out of range.\nRange is\n  row[" +
                std::to_string(from_row) + " (same as first row), " + std::to_string(ilrow()) + " (same as last row)],\n  col[" +
                std::to_string(from_col) + " (same as first col), " + std::to_string(ilcol()) + " (same as last col)],\n  but got [i,j]=[" +
                std::to_string(row_in) + "," + std::to_string(col_in) + "]." + _reset);
        }
        return data_[row * cols_ + col];
    }

    T get(int64_t index = 0)
    {
        auto index_in = index;
        if (index < 0)
            index += rows() * cols();
        if (index < 0 || index >= rows() * cols())
        {
            int from_index = -rows() * cols();
            throw std::out_of_range(
                _red + "Matrix::get() index out of range.\nRange is\n  index[" +
                std::to_string(from_index) + " (same as first item), " + std::to_string(rows() * cols() - 1) + " (same as last item)],\n  but got index=" +
                std::to_string(index_in) + "." + _reset);
        }
        return data_[index];
    }

    // Métodos (As implementações ficam aqui dentro mesmo)
    void from_prompt(size_t rows, size_t cols) noexcept
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

    void from_prompt() noexcept
    {
        size_t r, c;
        std::cout << "Matrix::from_prompt() Type the number of rows: ";
        std::cin >> r;
        std::cout << "Matrix::from_prompt() Type the number of columns: ";
        std::cin >> c;
        this->from_prompt(r, c);
    }

    friend std::ostream &operator<<(std::ostream &os, const Matrix<T> &m) noexcept
    {

        size_t width = print_width_;
        size_t color_int = print_color_;
        size_t precision = print_precision_;
        std::string bold = std::to_string(print_style_);

        // --- AUTOCORREÇÃO DE GRID INTELIGENTE E OTIMIZADA ---
        bool skip_rows = m.rows_ > 10;
        bool skip_cols = m.cols_ > 8;

        int max_int_chars = 1;

        for (size_t i = 0; i < m.rows_; ++i)
        {
            // [Se a matriz for grande, salta o miolo das linhas]
            if (skip_rows && i == 3)
            {
                i = m.rows_ - 4;
                continue;
            }

            for (size_t j = 0; j < m.cols_; ++j)
            {
                // [Se a matriz for grande, salta o miolo das colunas]
                if (skip_cols && j == 3)
                {
                    j = m.cols_ - 4;
                    continue;
                }

                T val = m(i, j);

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
        std::string color = "\033[" + bold + ";3" + std::to_string(color_int) + "m"; // Azul brilhante

        os << color + "Matrix: (" << m.rows_ << " x " << m.cols_ << ") ";
        if (print_debug_)
        {
            os << " [[Debug Mode]] ";
            os << (m.is_empty() ? "[empty matrix] " : "");
            os << (m.is_colvector() ? "[column vector] " : "");
            os << (m.is_rowvector() ? "[row vector] " : "");
            os << (m.is_scalar() ? "[scalar matrix] " : "");
            os << (m.is_square() ? "[square matrix] " : "");
            os << (m.is_symmetric() ? "[symmetric matrix] " : "");
        }
        os << "[ " + color_msg + m.comment + _reset + color + "\n";

        // 1. Padronizamos TODOS os prefixos de linha para terem exatamente 4 caracteres.
        os << "   + ";

        for (size_t j = 0; j < m.cols_; ++j)
        {
            if (skip_cols && j == 3)
            {
                // 2. Padronizamos a coluna: 2 espaços + (width - 2) caracteres de texto
                os << "  " << std::setw(width - 2) << std::left << "...";
                j = m.cols_ - 4;
                continue;
            }
            os << "  " << std::setw(width - 2) << std::left << j;
        }
        os << "\n";

        for (size_t i = 0; i < m.rows_; ++i)
        {

            if (skip_rows && i == 3)
            {
                os << color + "   :|" + _reset; // Prefixo de 4 caracteres
                for (size_t j = 0; j < m.cols_; ++j)
                {
                    if (skip_cols && j == 3)
                    {
                        os << "  " << std::setw(width - 2) << std::left << "...";
                        j = m.cols_ - 4;
                        continue;
                    }
                    os << "  " << std::setw(width - 2) << std::left << ":";
                }
                os << "\n";
                i = m.rows_ - 4;
                continue;
            }
            // O prefixo de linha agora é padronizado para 4 caracteres, alinhado à direita, e colorido
            os << color << std::setw(4) << std::right << i << "|" + _reset; // Prefixo de 4 caracteres

            for (size_t j = 0; j < m.cols_; ++j)
            {
                if (skip_cols && j == 3)
                {
                    os << "  " << std::setw(width - 2) << std::left << "...";
                    j = m.cols_ - 4;
                    continue;
                }

                T val = m(i, j);

                // 3. Devolvemos o sinal negativo que se perdia com o abs_val
                if (val >= 0 || isnan_(val))
                {
                    os << "  "; // 2 espaços alinhadores para positivos
                }
                else
                {
                    os << " -"; // 1 espaço + o sinal que foi removido do abs_val
                }

                T abs_val = (val >= 0) ? val : -val;
                if (std::isnan(val))
                {
                    // 4. Se for NaN, imprimimos "NaN" centralizado no espaço disponível
                    os << std::setw(width - 2) << std::left << "NaN";
                }
                else if (std::isinf(val))
                {
                    // 5. Se for Inf, imprimimos "Inf" centralizado no espaço disponível
                    os << std::setw(width - 2) << std::left << "Inf";
                }
                else
                {
                    // 6. Para números normais, aplicamos a formatação com precisão adaptada ao width

                    // std::cout << std::setprecision(6) << std::setw(width - 2) << std::left << abs_val;
                    //  [O precision agora se adapta ao width. O -3 desconta o espaço do ponto decimal e margem de segurança]
                    os << std::fixed << std::setprecision(precision) << std::setw(width - 2) << std::left << +abs_val;
                }
            }
            os << "\n";
        }
        os << std::defaultfloat; // [Reseta para notação normal para evitar que o modo fixed afete outras partes do programa]
        os << color + "]";
        if (print_debug_)
        {
            os << " [[Debug Mode]] Type: [" << typeid(m).name() << "] | Address: ["
               << &m << "]";
            size_t total_bytes = m.rows() * m.cols() * sizeof(T);
            double size_formatted = total_bytes;
            std::string suffix = " B";

            if (total_bytes >= 1024ULL * 1024 * 1024)
            { // Gigabytes
                size_formatted = total_bytes / (1024.0 * 1024.0 * 1024.0);
                suffix = " GB";
            }
            else if (total_bytes >= 1024 * 1024)
            { // Megabytes
                size_formatted = total_bytes / (1024.0 * 1024.0);
                suffix = " MB";
            }
            else if (total_bytes >= 1024)
            { // Kilobytes
                size_formatted = total_bytes / 1024.0;
                suffix = " KB";
            }
            os << " | Size: [" << std::fixed << std::setprecision(2) << size_formatted
               << suffix << "]";
        }
        os << _reset;
        return os;
    }

    void print(const std::string &appendix = "") const noexcept
    {
        auto color_msg = print_color_ != 6 ? _cyan : _magenta;
        std::string color = "\033[1;3" + std::to_string(print_color_) + "m"; // Azul brilhante
        std::cout << *this << " " << color << appendix + _reset + "\n";
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

    // --- MÉTODOS DE ACESSO ---

    // 1. Versão para leitura e escrita (retorna a referência da memória)
    T &operator()(size_t row, size_t col) noexcept
    {

        return data_[row * cols_ + col];
    }

    // 2. Versão apenas para leitura (usada quando a matriz é const)
    const T &operator()(size_t row, size_t col) const noexcept
    {

        return data_[row * cols_ + col];
    }

    T &operator()(size_t index) noexcept
    {
        return data_[index];
    }

    const T &operator()(size_t index) const noexcept
    {
        return data_[index];
    }

    // --- MÉTODOS DE ÁLGEBRA LINEAR ---

    // [Retorna uma nova Matrix e é assinalado como const para proteger a matriz original]
    [[nodiscard]] Matrix<T> diag() const noexcept
    {
        size_t limit = std::min(rows_, cols_);

        // [Cria um vetor coluna de tamanho 'limit x 1']
        Matrix<T> result(limit, 1);

        for (size_t i = 0; i < limit; ++i)
        {
            // [Copia o valor da coordenada (i, i) da matriz atual para a linha 'i', coluna '0' do resultado]
            result(i, 0) = data_[i * cols_ + i];
        }

        return result;
    }

    [[nodiscard]] Matrix<T> t() const noexcept
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

    [[nodiscard]] Matrix<T> sum(Axis axis = Axis::row) const
    {
        if (axis == Axis::row)
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
        else if (axis == Axis::col)
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

    [[nodiscard]] T sumsum() noexcept
    {
        Matrix<T> temp = sum();
        temp = temp.sum(Axis::col);
        return temp(0, 0);
    }

    [[nodiscard]] Matrix<T> cumsum(Axis axis = Axis::row) const
    {
        if (axis != 0 && axis != 1)
        {
            throw std::invalid_argument(
                _red + "Matrix::cumsum() Error: Axis must be 0 or 1." + _reset);
        }

        Matrix<T> result = *this;
        result >> "cumsum()";

        if (axis == 0)
        {
            for (size_t i = 0; i < rows(); i++)
            {
                if (rows() > 1 && i == 0)
                    continue;
                for (size_t j = 0; j < cols(); j++)
                {
                    result(i, j) += result(i - 1, j);
                }
            }
            return result;
        }
        else
        {
            for (size_t j = 0; j < cols(); j++)
            {
                if (cols() > 1 && j == 0)
                    continue;
                for (size_t i = 0; i < rows(); i++)
                {
                    result(i, j) += result(i, j - 1);
                }
            }
            return result;
        }
    }

    [[nodiscard]] Matrix<T> mean(Axis axis = Axis::row) const noexcept
    {
        Matrix<T> sum_result = this->sum(axis);

        if (axis == Axis::row)
        {
            // Divide cada elemento do vetor linha pela quantidade de linhas para obter a média
            for (size_t j = 0; j < sum_result.cols(); ++j)
            {
                sum_result(0, j) /= static_cast<T>(this->rows());
            }
        }
        else if (axis == Axis::col)
        {
            // Divide cada elemento do vetor coluna pela quantidade de colunas para obter a média
            for (size_t i = 0; i < sum_result.rows(); ++i)
            {
                sum_result(i, 0) /= static_cast<T>(this->cols());
            }
        }

        return sum_result;
    }

    [[nodiscard]] Matrix<T> var(Axis axis = Axis::row) const noexcept
    {
        Matrix<T> mean_result = this->mean(axis);
        Matrix<T> var_result(mean_result.rows(), mean_result.cols());

        if (axis == Axis::row)
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
        else if (axis == Axis::col)
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

    [[nodiscard]] Matrix<T> std(Axis axis = Axis::row) const noexcept
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

    [[nodiscard]] Matrix<T> getrows(size_t start, size_t end) const
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

    [[nodiscard]] Matrix<T> getrow(size_t index) const
    {
        if (index > this->ilrow())
        {

            throw std::out_of_range(_red + "\nMatrix::getrow() Error: Invalid row index. Got index=" +
                                    std::to_string(index) + "." + _reset);
        }

        return this->getrows(index, index + 1);
    }

    [[nodiscard]] Matrix<T> getcols(size_t start, size_t end) const
    {
        if (start >= cols_ || end > cols_ || start >= end)
        {
            throw std::out_of_range(_red + "\nMatrix::getcols() Error: Invalid column indices. The size is " + size() + ".\nGot start=" +
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

    [[nodiscard]] Matrix<T> getcol(size_t index) const
    {
        if (index > this->ilcol())
        {

            throw std::out_of_range(_red + "\nMatrix::getcol() Error: Invalid column index. Got index=" +
                                    std::to_string(index) + "." + _reset);
        }

        return this->getcols(index, index + 1);
    }

    [[nodiscard]] Matrix<T> reshape(size_t new_rows, size_t new_cols) const
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

    [[nodiscard]] Matrix<T> rmrows(size_t start, size_t end) const
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

    [[nodiscard]] Matrix<T> rmrow(size_t index) const
    {
        if (index > this->ilrow())
        {

            throw std::out_of_range(_red + "\nMatrix::rmrow() Error: Invalid row index. Got index=" +
                                    std::to_string(index) + "." + _reset);
        }

        return this->rmrows(index, index + 1);
    }

    [[nodiscard]] Matrix<T> rmcols(size_t start, size_t end) const
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

    [[nodiscard]] Matrix<T> rmcol(size_t index) const
    {
        if (index > this->ilcol())
        {

            throw std::out_of_range(_red + "\nMatrix::rmcol() Error: Invalid column index. Got index=" +
                                    std::to_string(index) + "." + _reset);
        }

        return this->rmcols(index, index + 1);
    }

    [[nodiscard]] Matrix<T> flatten() const
    {
        Matrix<T> result = *this;
        result.rows_ = 1;
        result.cols_ = rows() * cols();
        return result;
    }

    template <typename U>
    [[nodiscard]] auto operator+(const Matrix<U> &other) const
    {
        if (other.is_scalar())
        {
            return operator+(other(0, 0));
        }
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
    [[nodiscard]] auto operator+(U scalar) const
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
    [[nodiscard]] auto operator-(const Matrix<U> &other) const
    {
        if (other.is_scalar())
        {
            return operator-(other(0, 0));
        }
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
    [[nodiscard]] auto operator-(U scalar) const
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
    [[nodiscard]] auto operator*(const Matrix<U> &other) const
    {
        if (other.is_scalar())
        {
            return operator*(other(0, 0));
        }
        if (cols_ != other.rows())
        {
            throw std::invalid_argument(_red + "\nMatrix::operator*() Error: Number of columns of the first matrix must equal the number of rows of the second matrix for multiplication.\nCurrent size is " + size() + " but got " + other.size() + "." + _reset);
        }

        using ResultType = std::common_type_t<T, U>;
        Matrix<ResultType> result(rows_, other.cols());

        for (size_t i = 0; i < rows_; i++)
        {
            for (size_t k = 0; k < cols_; k++)
            {
                ResultType temp = this->operator()(i, k);
                for (size_t j = 0; j < other.cols(); j++)
                {
                    result(i, j) += temp * other(k, j);
                }
            }
        }

        return result;
    }

    template <typename U>
    [[nodiscard]] auto operator*(U scalar) const
    {
        using ResultType = std::common_type_t<T, U>;
        Matrix<ResultType> result(rows_, cols_);

        for (size_t i = 0; i < rows_ * cols_; ++i)
        {
            result(i) = (*this)(i)*scalar;
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
        if (other.is_scalar())
        {
            return operator/(other(0, 0));
        }
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
    friend auto operator+(const U &scalar, const Matrix<T> &m) -> Matrix<std::common_type_t<U, T>>
    {
        return m + scalar;
    }

    // [Opcional: Exemplo para subtração, onde a ordem importa muito]
    template <typename U>
    friend auto operator-(const U &scalar, const Matrix<T> &m) -> Matrix<std::common_type_t<U, T>>
    {
        using ResultType = std::common_type_t<U, T>;
        Matrix<ResultType> result(m.rows(), m.cols());

        for (size_t i = 0; i < m.rows() * m.cols(); ++i)
        {
            result(i) = scalar - m(i); // Aqui a ordem dita a matemática correta
        }
        return result;
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator*(const U &scalar, const Matrix<T> &m) -> Matrix<std::common_type_t<U, T>>
    {
        return m * scalar;
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator/(const U &scalar, const Matrix<T> &m) -> Matrix<std::common_type_t<U, T>>
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

    [[nodiscard]] bool equals(const Matrix<T> &other, T tolerance = static_cast<T>(1e-8)) const
    {
        if (rows_ != other.rows() || cols_ != other.cols())
        {
            return false;
        }

        if constexpr (std::is_integral_v<T>)
        {
            for (size_t i = 0; i < rows(); i++)
            {
                for (size_t j = 0; j < cols(); j++)
                {
                    if (this->operator()(i, j) != other(i, j))
                        return false;
                }
            }
            return true;
        }

        T sum_ = T();
        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {

                T diff = this->operator()(i, j) - other(i, j);
                diff = (diff >= 0) ? diff : -diff;
                sum_ += diff;
                if (sum_ > tolerance)
                {
                    return false;
                }
            }
        }

        return true;
    }
    [[nodiscard]] Matrix<T> operator-() const
    {
        Matrix<T> result(rows_, cols_);
        for (size_t i = 0; i < data_.size(); i++)
        {
            result.data_[i] = -data_[i];
        }
        return result;
    }

    [[nodiscard]] Matrix<T> ew_prod(const Matrix<T> &other) const
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

    // --- INTERACTIVE DOCUMENTATION ---
    static void help()
    {
        /*

        set_print_precision(size_t precision) noexcept { Matrix<T>::print_precision_ = precision; }
        static void set_print_width(size_t width) noexcept { Matrix<T>::print_width_ = width; }
        static void set_print_color(size_t color_int) noexcept { Matrix<T>::print_color_ = (color_int % 6) + 1; }
        static void set_print_font(size_t style_int) noexcept { Matrix<T>::print_style_ = (style_int % 9) + 1; }
        static void set_print_debug(bool debug) noexcept { print_debug_ = debug; }
        */

        std::cout << _cyan << "\n====================================================================\n"
                  << "                 J5R MATRIX LIBRARY - DOCUMENTATION                \n"
                  << "====================================================================\n"
                  << _reset

                  << _green << "\nIn the following, ``!`` marks operations that modify the Matrix object, so be carefull.\n"
                  << "The operations without ``!`` work with a copy of the Matrix.\nType ENTER to go to the next part of the help.\n"
                  << _reset

                  << _yellow << "\n[Constructors]\n"
                  << _reset
                  << "   Matrix<T> m;                      : Empty matrix.\n"
                  << "   Matrix<T> m(nrows,ncols);         : Zero matrix with [nrows] rows and [ncols] columns.\n"
                  << "   Matrix<T> m(nrows,ncols,value);   : Same as above, but with [value] in all entries.\n"
                  << "   Matrix<T> m = {{1,2,3},{4,5,6}}   : A set of values are hardcoded into the matrix.\n"
                  << "   Matrix<T> m;\n"
                  << "   |_    m.from_prompt(nrows,ncols); : Prompts for entries from the user.\n"
                  << "   Matrix<T> m;\n"
                  << "   |_    m.from_prompt();            : Prompts the dimensions and entries from the user.\n";
        std::cin.get();

        std::cout << _yellow << "\n[Basic Properties]\n"
                  << _reset
                  << "  .rows()             : Returns the number of rows (size_t).\n"
                  << "  .cols()             : Returns the number of columns (size_t).\n"
                  << "  .ilrow()            : Returns the index of the last row (size_t) - similar to rows()-1.\n"
                  << "  .ilcol()            : Returns the index of the last column (size_t) - similar to cols()-1.\n"
                  << "  .size()             : Returns the dimensions as a string (e.g., \"3x4\").\n"
                  << "  .printsize()        : Prints the dimensions (e.g std::cout << \"3x4\").\n"
                  << "  .is_empty()         : Checks if the matrix is empty.\n"
                  << "  .is_square()        : Checks if the matrix is square (rows == columns).\n"
                  << "  .is_scalar()        : Checks if the matrix is scalar (rows==1==columns).\n"
                  << "  .is_rowvector()     : Checks if the matrix is a row vector (1xN).\n"
                  << "  .is_colvector()     : Checks if the matrix is a column vector (Nx1).\n"
                  << "  .is_symmetric()     : Checks if the matrix is symmetric.\n";
        std::cin.get();

        std::cout << _yellow << "\n[Access and Modification]\n"
                  << _reset
                  << "  !M(i, j)            : Direct and ultra-fast access (no bounds checking).\n"
                  << "  !M(index)           : Ultra-fast 1D linear row-wise access (no bounds checking).\n"
                  << "  .get(i, j)          : Returns the element with bounds checking (safe/debug).\n"
                  << "  .get(idx)           : 1D linear row-wise access - returns the element with bounds checking (safe/debug).\n"
                  << "  !.set(i, j, val)    : Modifies the element with bounds checking (safe/debug).\n"
                  << "  !.set(idx, val)     : 1D linear row-wise access - modifies the element with bounds checking (safe/debug).\n"
                  << "->obs: these get/set methods accepts negative indexes as in python language.\n"
                  << "  .getrow(i)          : Returns row 'i' as a copied new matrix (1xN).\n"
                  << "  .getrows(start,end) : Returns a copy of rows from (start) to (end-1).\n"
                  << "  .getcol(j)          : Returns column 'j' as a copied new matrix (Nx1).\n"
                  << "  .getcols(start,end) : Returns a copy of columns from (start) to (end-1).\n"
                  << "  .rmrow(i)           : Returns a copy removing the i-th row.\n"
                  << "  .rmrows(start,end)  : Returns a copy removing rows from (start) to (end-1).\n"
                  << "  .rmcol(j)           : Returns a copy removing the j-th column.\n"
                  << "  .rmcols(start,end)  : Returns a copy removing rows from (start) to (end-1).\n"
                  << "  .flatten()          : Returns a row vector with all the entries taken in row-wise way.\n"
                  << "  .reshape()          : Returns a new shaped matrix with all the entries taken in row-wise way.\n";

        std::cin.get();

        std::cout << _yellow << "\n[Mathematical and Statistical Operations]\n"
                  << _reset
                  << "  .t()                : Returns the transposed matrix.\n"
                  << "  .diag()             : Returns a column with the diagonal elements.\n"
                  << "  .abs()              : Returns a new matrix with absolute values.\n"
                  << "  .min(axis=0)        : Returns std::pair {indices, minimum_values}; axis: (0=Cols, 1=Rows).\n"
                  << "  |_      auto [a,b] = M.min();\n"
                  << "  .max(axis=0)        : Returns std::pair {indices, maximum_values}; axis: (0=Cols, 1=Rows).\n"
                  << "  |_      auto [a,b] = M.max();\n"
                  << "  .sum(axis=0)        : Returns the sum of elements; axis: (0=Cols, 1=Rows).\n"
                  << "  .sumsum()           : Returns the sum of all entries of the matrix.\n"
                  << "  .mean(axis=0)       : Returns the mean of elements; axis: (0=Cols, 1=Rows).\n"
                  << "  .var(axis=0)        : Returns the variance of elements; axis: (0=Cols, 1=Rows).\n"
                  << "  .std(axis=0)        : Returns the standard deviation; axis: (0=Cols, 1=Rows).\n";
        std::cin.get();

        std::cout << _yellow << "\n[Input, Output and Debug]\n"
                  << _reset
                  << "  std::cout << M                : Prints the formatted matrix to the terminal.\n"
                  << "  !.msg(\"text\")                 : Sets a title to be displayed in the next .print() or std::cout.\n"
                  << "  !M << text << number << text  : The same as .msg() but with << operator\n"
                  << "  !M >> mytext (or number)      : The same as .msg() but clears previous title replacing by mytext\n"
                  << "  .print(appendix=\"\")           : Prints the matrix with an appendix string at the end.\n"
                  << "  .save_to_file(fname,sep=\";\")  : Saves the matrix in text format (CSV).\n"
                  << "  .load_from_file(fname,sep=\";\") : Loads the matrix from a text file (CSV).\n"
                  << "  .save_to_binary(fname)        : Saves the matrix in ultra-fast binary format (.dat).\n"
                  << "  .load_from_binary(fname)      : Loads the matrix from a binary file (.dat).\n";
        std::cin.get();

        std::cout << _yellow << "\n[Relational and Logical Operators]\n"
                  << _reset
                  << "  M1 == M2            : Returns a Matrixbool (uint8_t) with 1 where elements are equal.\n"
                  << "  M1 > M2             : Returns a Matrixbool (uint8_t) with 1 where M1 > M2.\n"
                  << "  M1 < M2             : Returns a Matrixbool (uint8_t) with 1 where M1 < M2.\n"
                  << "  M1 >= M2            : Returns a Matrixbool (uint8_t) with 1 where M1 >= M2.\n"
                  << "  M1 <= M2            : Returns a Matrixbool (uint8_t) with 1 where M1 <= M2.\n"
                  << "  M1 != M2            : Returns a Matrixbool (uint8_t) with 1 where elements are different.\n"
                  << "  M1 == scalar        : Returns a Matrixbool (uint8_t) with 1 where elements are equal.\n"
                  << "  M1 > scalar         : Returns a Matrixbool (uint8_t) with 1 where M1 > scalar.\n"
                  << "  M1 < scalar         : Returns a Matrixbool (uint8_t) with 1 where M1 < scalar.\n"
                  << "  M1 >= scalar        : Returns a Matrixbool (uint8_t) with 1 where M1 >= scalar.\n"
                  << "  M1 <= scalar        : Returns a Matrixbool (uint8_t) with 1 where M1 <= scalar.\n"
                  << "  M1 != scalar        : Returns a Matrixbool (uint8_t) with 1 where M1 != M2.\n"
                  << "  .is(M2)             : Checks if two matrices point to the same memory address.\n";
        std::cin.get();

        std::cout << _yellow << "\n[Arithmetic Operators]\n"
                  << _reset
                  << "  M1 + M2            : Returns the sum of the matrices.\n"
                  << "  M1 - M2            : Returns the subtraction of the matrices.\n"
                  << "  - M                : Returns the negative of the matrix.\n"
                  << "  M1 * M2            : Returns the product of the matrices.\n"
                  << "  M1 / M2            : Returns an element-wise division of the matrices.\n"
                  << "  M.pow(power)       : Returns M raised to a positive power (identity if power is zero).\n"
                  << "  M1.ew_prod(M2)     : Returns an element-wise product of the matrices.\n"
                  << "  M + scalar         : Returns the sum with the scalar.\n"
                  << "  M - scalar         : Returns the subtraction with the scalar.\n"
                  << "  M * scalar         : Returns the product with the scalar.\n"
                  << "  scalar / M         : Returns the division with the scalar.\n"
                  << "  scalar + M         : Returns the sum with the scalar.\n"
                  << "  scalar - M         : Returns the subtraction with the scalar.\n"
                  << "  scalar * M         : Returns the product with the scalar.\n"
                  << "  scalar / M         : Returns the division with the scalar.\n";
        std::cin.get();

        std::cout << _yellow << "\n[Auxiliary Global Functions]\n"
                  << _reset
                  << "  diag(M)             : Returns a square matrix with the diagonal elements of M\n"
                  << "  |_        (or takes the elements of M into a diagonal, if M is a row or column vector).\n"
                  << "  strcc [object]      : Streaming for concatenating like std::cout.\n"
                  << "  |   strcc << \"text \" << 23 << \" other text\";\n"
                  << "  |_  strcc.str();    //gets the string \"text 23 other text\"\n";
        std::cin.get();

        std::cout << _yellow << "\n[Global Settings]\n"
                  << _reset
                  << "   Matrix<T>::set_print_precision(precision)  : Sets the precision of the numbers printed (int).\n"
                  << "   Matrix<T>::set_print_width(width)          : Sets the width for the numbers printed (int).\n"
                  << "   Matrix<T>::set_print_color(color_int)      : Sets the color for printing the matrix (int from 1 to 6).\n"
                  << "   Matrix<T>::set_print_font(style_int)       : Sets the font style for printing the matrix (int from 1 to 9).\n"
                  << "   Matrix<T>::set_print_debug(debug)          : Sets whether in debug mode or not (bool).\n\n"
                  << _cyan << "====================================================================\n"
                  << _reset;
    }

    [[nodiscard]] Matrix<T> copy() const
    {
        Matrix<T> result = *this;
        return result;
    }

    [[nodiscard]] Matrix<T> pow(int exponent) const
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

    template <typename U>
    Matrix<j5r_bool> operator>(const U &scalar) const
    {
        Matrix<j5r_bool> result(rows_, cols_);
        for (size_t i = 0; i < rows_; i++)
        {
            for (size_t j = 0; j < cols_; j++)
            {
                result.set(i, j, this->operator()(i, j) > scalar);
            }
        }
        return result;
    }

    template <typename U>
    Matrix<j5r_bool> operator>=(const U &scalar) const
    {
        Matrix<j5r_bool> result(rows_, cols_);
        for (size_t i = 0; i < rows_; i++)
        {
            for (size_t j = 0; j < cols_; j++)
            {
                result.set(i, j, this->operator()(i, j) >= scalar);
            }
        }
        return result;
    }

    template <typename U>
    Matrix<j5r_bool> operator<(const U &scalar) const
    {
        Matrix<j5r_bool> result(rows_, cols_);
        for (size_t i = 0; i < rows_; i++)
        {
            for (size_t j = 0; j < cols_; j++)
            {
                result.set(i, j, this->operator()(i, j) < scalar);
            }
        }
        return result;
    }

    template <typename U>
    Matrix<j5r_bool> operator<=(const U &scalar) const
    {
        Matrix<j5r_bool> result(rows_, cols_);
        for (size_t i = 0; i < rows_; i++)
        {
            for (size_t j = 0; j < cols_; j++)
            {
                result.set(i, j, this->operator()(i, j) <= scalar);
            }
        }
        return result;
    }

    template <typename U>
    Matrix<j5r_bool> operator==(const U &scalar) const
    {
        Matrix<j5r_bool> result(rows_, cols_);
        for (size_t i = 0; i < rows_; i++)
        {
            for (size_t j = 0; j < cols_; j++)
            {
                result.set(i, j, this->operator()(i, j) == scalar);
            }
        }
        return result;
    }

    template <typename U>
    Matrix<j5r_bool> operator!=(const U &scalar) const
    {
        Matrix<j5r_bool> result(rows_, cols_);
        for (size_t i = 0; i < rows_; i++)
        {
            for (size_t j = 0; j < cols_; j++)
            {
                result.set(i, j, this->operator()(i, j) != scalar);
            }
        }
        return result;
    }

    template <typename U>
    Matrix<j5r_bool> operator>(const Matrix<U> &other) const
    {
        if (other.is_scalar())
        {
            return operator>(other(0, 0));
        }
        if (rows_ != other.rows() || cols_ != other.cols())
        {
            throw std::invalid_argument(_red +
                                        "\nMatrix::operator>() Matrices must have the same size. Got " + size() + " vs " + other.size() + ".\n" + _reset);
        }
        Matrix<j5r_bool> result(rows_, cols_);
        for (size_t i = 0; i < rows_; i++)
        {
            for (size_t j = 0; j < cols_; j++)
            {
                result.set(i, j, this->operator()(i, j) > other(i, j));
            }
        }
        return result;
    }

    template <typename U>
    Matrix<j5r_bool> operator<(const Matrix<U> &other) const
    {
        if (other.is_scalar())
        {
            return operator<(other(0, 0));
        }
        if (rows_ != other.rows() || cols_ != other.cols())
        {
            throw std::invalid_argument(_red +
                                        "\nMatrix::operator>() Matrices must have the same size. Got " + size() + " vs " + other.size() + ".\n" + _reset);
        }
        Matrix<j5r_bool> result(rows_, cols_);
        for (size_t i = 0; i < rows_; i++)
        {
            for (size_t j = 0; j < cols_; j++)
            {
                result.set(i, j, this->operator()(i, j) < other(i, j));
            }
        }
        return result;
    }

    template <typename U>
    Matrix<j5r_bool> operator>=(const Matrix<U> &other) const
    {
        if (other.is_scalar())
        {
            return operator>=(other(0, 0));
        }
        if (rows_ != other.rows() || cols_ != other.cols())
        {
            throw std::invalid_argument(_red +
                                        "\nMatrix::operator>() Matrices must have the same size. Got " + size() + " vs " + other.size() + ".\n" + _reset);
        }
        Matrix<j5r_bool> result(rows_, cols_);
        for (size_t i = 0; i < rows_; i++)
        {
            for (size_t j = 0; j < cols_; j++)
            {
                result.set(i, j, this->operator()(i, j) >= other(i, j));
            }
        }
        return result;
    }

    template <typename U>
    Matrix<j5r_bool> operator<=(const Matrix<U> &other) const
    {
        if (other.is_scalar())
        {
            return operator<=(other(0, 0));
        }
        if (rows_ != other.rows() || cols_ != other.cols())
        {
            throw std::invalid_argument(_red +
                                        "\nMatrix::operator>() Matrices must have the same size. Got " + size() + " vs " + other.size() + ".\n" + _reset);
        }
        Matrix<j5r_bool> result(rows_, cols_);
        for (size_t i = 0; i < rows_; i++)
        {
            for (size_t j = 0; j < cols_; j++)
            {
                result.set(i, j, this->operator()(i, j) <= other(i, j));
            }
        }
        return result;
    }

    template <typename U>
    Matrix<j5r_bool> operator==(const Matrix<U> &other) const
    {
        if (other.is_scalar())
        {
            return operator==(other(0, 0));
        }
        if (rows_ != other.rows() || cols_ != other.cols())
        {
            throw std::invalid_argument(_red +
                                        "\nMatrix::operator>() Matrices must have the same size. Got " + size() + " vs " + other.size() + ".\n" + _reset);
        }
        Matrix<j5r_bool> result(rows_, cols_);
        for (size_t i = 0; i < rows_; i++)
        {
            for (size_t j = 0; j < cols_; j++)
            {
                result.set(i, j, this->operator()(i, j) == other(i, j));
            }
        }
        return result;
    }

    template <typename U>
    Matrix<j5r_bool> operator!=(const Matrix<U> &other) const
    {
        if (other.is_scalar())
        {
            return operator!=(other(0, 0));
        }
        if (rows_ != other.rows() || cols_ != other.cols())
        {
            throw std::invalid_argument(_red +
                                        "\nMatrix::operator>() Matrices must have the same size. Got " + size() + " vs " + other.size() + ".\n" + _reset);
        }
        Matrix<j5r_bool> result(rows_, cols_);
        for (size_t i = 0; i < rows_; i++)
        {
            for (size_t j = 0; j < cols_; j++)
            {
                result.set(i, j, this->operator()(i, j) != other(i, j));
            }
        }
        return result;
    }

    template <typename U>
    bool is(const Matrix<U> &other) const
    {
        return this == &other;
    }

    // --- GRAVAÇÃO EM BINÁRIO ---
    void save_to_binary(const std::string &filename) const
    {
        // A flag std::ios::binary é o que desliga a formatação de texto
        std::ofstream file(filename, std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("\nMatrix::save_to_binary() Error: Could not open file.\n");
        }

        // 1. Grava as dimensões (cabeçalho)
        // reinterpret_cast converte o endereço da variável num ponteiro de bytes puros
        file.write(reinterpret_cast<const char *>(&rows_), sizeof(size_t));
        file.write(reinterpret_cast<const char *>(&cols_), sizeof(size_t));

        // 2. Grava TODOS os dados da matriz de uma só vez (A MÁGICA DA VELOCIDADE!)
        file.write(reinterpret_cast<const char *>(data_.data()), data_.size() * sizeof(T));

        file.close();
    }

    // --- LEITURA EM BINÁRIO ---
    void load_from_binary(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("\nMatrix::load_from_binary() Error: Could not open file.\n");
        }

        size_t new_rows, new_cols;

        // 1. Lê o cabeçalho (dimensões)
        file.read(reinterpret_cast<char *>(&new_rows), sizeof(size_t));
        file.read(reinterpret_cast<char *>(&new_cols), sizeof(size_t));

        // 2. Prepara a matriz para receber os dados
        this->rows_ = new_rows;
        this->cols_ = new_cols;
        this->data_.resize(new_rows * new_cols);

        // 3. Despeja os dados do ficheiro direto para a memória RAM num único comando
        file.read(reinterpret_cast<char *>(data_.data()), data_.size() * sizeof(T));

        file.close();
    }

    [[nodiscard]] Matrix<T> operator()(const Matrix<uint8_t> &boolMatrix) const
    {
        if (boolMatrix.rows() != rows_ || boolMatrix.cols() != cols_)
        {
            throw std::out_of_range(
                _red +
                "Matrix::operator() boolMatrix has not a compatible size. It has " + boolMatrix.size() + " vs matrix size " + size() + ".");
        }

        Matrix<T> result;

        for (size_t i = 0; i < rows_; i++)
        {
            for (size_t j = 0; j < cols_; j++)
            {
                if (boolMatrix(i, j))
                {
                    result.data_.push_back(this->operator()(i, j));
                    result.cols_ += 1;
                    result.rows_ = 1;
                }
            }
        }
        return result;
    }

    [[nodiscard]] std::pair<Matrix<size_t>, Matrix<T>> min(uint8_t axis = 0) const
    {
        if (axis == 0) // Mínimo de cada COLUNA (Resulta em 1xCols)
        {
            Matrix<size_t> result_idx(1, cols());
            Matrix<T> result_val(1, cols());

            for (size_t j = 0; j < cols(); j++)
            {
                size_t best_idx = 0;
                T min_val = this->operator()(0, j); // Assume que o primeiro é o menor

                for (size_t i = 1; i < rows(); i++)
                {
                    if (this->operator()(i, j) < min_val)
                    {
                        min_val = this->operator()(i, j);
                        best_idx = i;
                    }
                }
                result_idx(0, j) = best_idx;
                result_val(0, j) = min_val;
            }

            result_idx >> "min idx | axis = " << (bool)axis;
            result_val >> "min val | axis = " << (bool)axis;
            return {result_idx, result_val};
        }
        else if (axis == 1) // Mínimo de cada LINHA (Resulta em Rowsx1)
        {
            // CORREÇÃO 1: Mudado de Matrix<int> para Matrix<size_t>
            Matrix<size_t> result_idx(rows(), 1);
            Matrix<T> result_val(rows(), 1);

            for (size_t i = 0; i < rows(); i++)
            {
                auto row_start = data_.begin() + (i * cols());
                auto row_end = row_start + cols();

                auto it_min = std::min_element(row_start, row_end);
                size_t best_idx = std::distance(row_start, it_min);

                result_idx(i, 0) = best_idx;
                // CORREÇÃO 2: Adicionado o asterisco (*) para pegar o valor do iterador
                result_val(i, 0) = *it_min;
            }

            result_idx >> "min idx | axis = " << (bool)axis;
            result_val >> "min val | axis = " << (bool)axis;
            return {result_idx, result_val};
        }
        else
        {
            throw std::invalid_argument(_red + "\nMatrix::min() Axis must be 0 or 1.\n" + _reset);
        }
    }

    [[nodiscard]] std::pair<Matrix<size_t>, Matrix<T>> max(uint8_t axis = 0) const
    {
        if (axis == 0) // Máximo de cada COLUNA (Resulta em 1xCols)
        {
            Matrix<size_t> result_idx(1, cols());
            Matrix<T> result_val(1, cols());

            for (size_t j = 0; j < cols(); j++)
            {
                size_t best_idx = 0;
                T max_val = this->operator()(0, j); // Assume que o primeiro é o maior

                for (size_t i = 1; i < rows(); i++)
                {
                    // A única diferença para o min() é este sinal de MAIOR (>)
                    if (this->operator()(i, j) > max_val)
                    {
                        max_val = this->operator()(i, j);
                        best_idx = i;
                    }
                }
                result_idx(0, j) = best_idx;
                result_val(0, j) = max_val;
            }

            result_idx >> "max idx | axis = " << (bool)axis;
            result_val >> "max val | axis = " << (bool)axis;
            return {result_idx, result_val};
        }
        else if (axis == 1) // Máximo de cada LINHA (Resulta em Rowsx1)
        {
            Matrix<size_t> result_idx(rows(), 1);
            Matrix<T> result_val(rows(), 1);

            for (size_t i = 0; i < rows(); i++)
            {
                // Como as linhas estão juntas na memória, usamos iteradores!
                auto row_start = data_.begin() + (i * cols());
                auto row_end = row_start + cols();

                // Usamos std::max_element da biblioteca <algorithm>
                auto it_max = std::max_element(row_start, row_end);
                size_t best_idx = std::distance(row_start, it_max);

                result_idx(i, 0) = best_idx;
                result_val(i, 0) = *it_max; // Desreferencia o iterador para obter o valor
            }

            result_idx >> "max idx | axis = " << (bool)axis;
            result_val >> "max val | axis = " << (bool)axis;
            return {result_idx, result_val};
        }
        else
        {
            throw std::invalid_argument(_red + "\nMatrix::max() Axis must be 0 or 1.\n" + _reset);
        }
    }

    Matrix<T> abs(void) const
    {
        if constexpr (std::is_unsigned_v<T>)
        {
            return *this;
        }
        Matrix<T> result = *this;
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < cols(); j++)
            {
                result(i, j) = (result(i, j) >= 0) ? result(i, j) : -result(i, j);
            }
        }
        return result;
    }

    Matrix<T> &operator+=(const Matrix<T> &other)
    {
        if (other.is_scalar())
        {
            this->operator+=(other(0));
            return *this;
        }

        if (is_scalar() && (!other.is_scalar()))
        {
            T scalar = this->operator()(0);
            *this = other;
            this->operator+=(scalar);
            return *this;
        }
        if (rows_ != other.rows() || cols_ != other.cols())
        {
            throw std::invalid_argument(_red +
                                        "Matrix::operator+=() Matrices must have the same dimensions for adding, or be a scalar matrix." +
                                        "\nGot " + size() + " vs " + other.size() + "." + _reset);
        }

        Matrix<T> result = ((*this) + other);
        *this = result;
        return *this;
    }

    Matrix<T> &operator-=(const Matrix<T> &other)
    {
        if (other.is_scalar())
        {
            this->operator-=(other(0));
            return *this;
        }

        if (is_scalar() && (!other.is_scalar()))
        {
            T scalar = this->operator()(0);
            *this = -other;
            this->operator+=(scalar);
            return *this;
        }

        if (rows_ != other.rows() || cols_ != other.cols())
        {
            throw std::invalid_argument(_red +
                                        "Matrix::operator-=() Matrices must have the same dimensions for subtracting, or be a scalar matrix." +
                                        "\nGot " + size() + " vs " + other.size() + "." + _reset);
        }

        Matrix<T> result = operator-(other);
        *this = result;
        return *this;
    }

    Matrix<T> &operator*=(const Matrix<T> &other)
    {
        if (other.is_scalar())
        {
            this->operator*=(other(0));
            return *this;
        }

        if (is_scalar() && (!other.is_scalar()))
        {
            T scalar = this->operator()(0);
            *this = other;
            this->operator*=(scalar);
            return *this;
        }

        if (cols_ != other.rows())
        {
            throw std::invalid_argument(_red +
                                        "Matrix::operator*=() Matrices must have compatible dimensions for multiplying, or be a scalar matrix." +
                                        "\nGot " + size() + " vs " + other.size() + "." + _reset);
        }

        Matrix<T> result = ((*this) * other);
        *this = result;
        return *this;
    }

    Matrix<T> &operator/=(const Matrix<T> &other)
    {
        if (other.is_scalar())
        {
            this->operator/=(other(0));
            return *this;
        }

        if (is_scalar() && (!other.is_scalar()))
        {
            Matrix<T> result = ((*this) / other);
            *this = result;
            return *this;
        }

        if (rows_ != other.rows() || cols_ != other.cols())
        {
            throw std::invalid_argument(_red +
                                        "Matrix::operator/=() Matrices must have the same dimensions for element-wise division, or be a scalar matrix." +
                                        "\nGot " + size() + " vs " + other.size() + "." + _reset);
        }

        Matrix<T> result = ((*this) / other);
        *this = result;
        return *this;
    }

    Matrix<T> &operator+=(T scalar) noexcept
    {
        for (size_t i = 0; i < rows() * cols(); i++)
        {
            this->data_[i] += scalar;
        }
        return *this;
    }

    Matrix<T> &operator-=(T scalar) noexcept
    {
        for (size_t i = 0; i < rows() * cols(); i++)
        {
            this->data_[i] -= scalar;
        }
        return *this;
    }

    Matrix<T> &operator*=(T scalar) noexcept
    {
        for (size_t i = 0; i < rows() * cols(); i++)
        {
            this->data_[i] *= scalar;
        }
        return *this;
    }

    Matrix<T> &operator/=(T scalar) noexcept
    {
        if constexpr (std::is_integral_v<T>)
        {
            throw std::invalid_argument(_red + "Matrix::operator/=() Integral types are not allowed to divide.\nTry Matrix<floating point> instead." + _reset);
        }
        else
        {
            for (size_t i = 0; i < rows() * cols(); i++)
            {
                this->data_[i] /= scalar;
            }
        }
        return *this;
    }

    Matrix<T> round() const
    {
        if constexpr (std::is_integral_v<T>)
        {
            throw std::exception(_red + "Matrix::round() Integral types are not allowed to invoke .round() method." + _reset);
        }
        Matrix<T> result = *this;
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < cols(); j++)
            {
                result(i, j) = std::round(result(i, j));
            }
        }
        return result;
    }

    Matrix<T> ceil() const
    {
        if constexpr (std::is_integral_v<T>)
        {
            throw std::exception(_red + "Matrix::ceil() Integral types are not allowed to invoke .ceil() method." + _reset);
        }
        Matrix<T> result = *this;
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < cols(); j++)
            {
                result(i, j) = std::ceil(result(i, j));
            }
        }
        return result;
    }

    Matrix<T> floor() const
    {
        if constexpr (std::is_integral_v<T>)
        {
            throw std::exception(_red + "Matrix::floor() Integral types are not allowed to invoke .floor() method." + _reset);
        }
        Matrix<T> result = *this;
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < cols(); j++)
            {
                result(i, j) = std::floor(result(i, j));
            }
        }
        return result;
    }

    bool is_lower_triangular(bool strictly_lower = false) const noexcept
    {
        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                if (strictly_lower)
                {
                    // diagonal e acima devem ser zero
                    if (j >= i && (*this)(i, j) != T())
                        return false;
                }
                else
                {
                    // apenas acima da diagonal deve ser zero
                    if (j > i && (*this)(i, j) != T())
                        return false;
                }
            }
        }
        return true;
    }

    bool is_upper_triangular(bool strictly_lower = false) const noexcept
    {
        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                if (strictly_lower)
                {
                    // diagonal e acima devem ser zero
                    if (i >= j && (*this)(i, j) != T())
                        return false;
                }
                else
                {
                    // apenas acima da diagonal deve ser zero
                    if (i > j && (*this)(i, j) != T())
                        return false;
                }
            }
        }
        return true;
    }

    Matrix<T> operator^(size_t exponent)
    {
        return pow(exponent);
    }

    Matrix<T> triu(bool strictly_above = false) const noexcept
    {
        if (is_vector() || is_scalar())
        {
            return Matrix<T>();
        }
        Matrix<T> result(rows_, cols_, T(0));
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < cols(); j++)
            {
                if (strictly_above)
                {
                    if (j <= i)
                        continue;
                }
                else
                {
                    if (j < i)
                        continue;
                }
                result(i, j) = (*this)(i, j);
            }
        }
        result >> "Matrix::triu() - triangular upper. Strictly above diagonal? -> " << strictly_above;
        return result;
    }

    Matrix<T> tril(bool strictly_above = false) const noexcept
    {
        if (is_vector() || is_scalar())
        {
            return Matrix<T>();
        }
        Matrix<T> result(rows_, cols_, T(0));
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < cols(); j++)
            {
                if (strictly_above)
                {
                    if (i <= j)
                        continue;
                }
                else
                {
                    if (i < j)
                        continue;
                }
                result(i, j) = (*this)(i, j);
            }
        }
        result >> "Matrix::tril() - triangular lower. Strictly below diagonal? -> " << strictly_above;
        return result;
    }

    Matrix<T> &operator<<(const std::string &txt)
    {
        strcc << comment << txt;
        strcc(*this);
        return *this;
    }

    Matrix<T> &operator>>(const std::string &txt)
    {
        strcc << txt;
        strcc(*this);
        return *this;
    }

    template <typename U>
    Matrix<T> &operator<<(U number)
    {
        strcc << comment << number;
        strcc(*this);
        return *this;
    }

    template <typename U>
    Matrix<T> &operator>>(U number)
    {
        strcc << number;
        strcc(*this);
        return *this;
    }

    /* remova esses operadores abaixo */
    void operator++(int)
    { // objeto++
        (*this) += T(1);
    }
    void operator--(int)
    { // objeto++
        (*this) -= T(1);
    }
    void operator++() // o que fazer com esse ?
    {                 // ++objeto

        print();
    }

    Matrix<uint8_t> operator||(const Matrix<T> &other) const
    {
        if (rows_ != other.rows() || cols_ != other.cols())
        {
            throw std::invalid_argument(_red +
                                        "Matrix::operator||() Matrices must have the same dimensions." + _reset);
        }
        Matrix<uint8_t> result(rows_, cols_);
        bool mine, yours;
        for (size_t i = 0; i < rows_ * cols_; i++)
        {
            mine = static_cast<bool>((*this)(i));
            yours = static_cast<bool>(other(i));
            result(i) = (mine || yours) ? 1 : 0;
        }
        return result;
    }

    Matrix<uint8_t> operator&&(const Matrix<T> &other) const
    {
        if (rows_ != other.rows() || cols_ != other.cols())
        {
            throw std::invalid_argument(_red +
                                        "Matrix::operator||() Matrices must have the same dimensions." + _reset);
        }
        Matrix<uint8_t> result(rows_, cols_);
        bool mine, yours;
        for (size_t i = 0; i < rows_ * cols_; i++)
        {
            mine = static_cast<bool>((*this)(i));
            yours = static_cast<bool>(other(i));
            result(i) = (mine && yours) ? 1 : 0;
        }
        return result;
    }

    Matrix<uint8_t> operator!() const noexcept
    {

        Matrix<uint8_t> result(rows_, cols_);
        bool value;
        for (size_t i = 0; i < rows_ * cols_; i++)
        {
            value = static_cast<bool>((*this)(i));
            result(i) = value ? 0 : 1;
        }
        return result;
    }

    void operator||(size_t style)
    {
        set_print_font(style);
    }

    void operator|(int v)
    {
        set_print_width(v);
        set_print_precision(v);
    }
    /* remova esses operadores acima */
};

template <typename T>
[[nodiscard]] inline Matrix<T> diag(const Matrix<T> &m)
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

using Matrixbool = Matrix<j5r_bool>;
