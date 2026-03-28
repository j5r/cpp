# PROJECT cpp (C-plus-plus)

By Junior R. Ribeiro, [2026, March, 16] ...................................................... j5rodrib@gmail.com

This project is willing to implement a library for Computer Science and Mathematics purposes. It was born from the need I had to learn C++ language. I tried to learn it for around 4 or 5 times along some yearss, without succeed, because learning by my own with a poor understanding of English was a determinant barrier I was not prepared for. From time to time, I tried again, without much more advances since the previous one. Nowadays, with help of Natural Language Models such as ChatGPT or Gemini that I did not had in those other tries, and with a more accurate English skills, I am able to achieve new marks in my learning process.

The basics of the project are described below:
 - [x] **Matrix.hpp:** a library with the basis - implementing **only matrix operations** (arithmetic/boolean operations, some basic statistics such as min, max, mean, var, std, and others);
 - [x] **MatrixGen.hpp:** a library for **generating** matrices from parameters (identity, several random models);
 - [x] **MatrixShape.hpp:** a library for **modifying structure and shape** of the matrices (add/remove columns and rows);

These three are the most important imports you need to include in your main file, because they need each other in some amount and are the basics operations we usually need in Computer Science.

The other steps I am willing to introduce in this project are described below:
 - [x] **MatrixLinAlg:** a library for the algorithms of **Linear Algebra**;
 - [ ] **MatrixSimplex:** a library for the algorithms needed for **Simplex and Dual Simplex** implementations;
 - [ ] **MatrixOpsResearch:** a library for Operations Research, implementing algorithms such as **Branch-and-Bound** and others.
 - [ ] **MatrixMLPerceptron:** a library for **Multilayer Perceptron** (Machine Learning);



# **CLASS MATRIX (file Matrix.hpp)**


This is the primordial instrument you will use, and it is planned to be as performatic and secure as possible: nothing about pointers are in this project, because a little bug in there may cause  catastrophics errors in the future, so I decided to create everything over the `std::vector` layer (it is a standard C++ structure, optimized and secure). Not everything is "easy-peasy", so we need to add some extra cautions, bringing certain structures into this file that are not the "standard way to do it", always willing performance or security for supporting large models.

## **The Matrix not related structures**

### *String concatenator*

I implemented a class `ostringstream_extension` to make easier to get string concatenation, and if needed you can declare an object from it to be used anywhere you want:

```c++
// Example: code
ostringstream_extension str_concatenator;      // declaring the concatenator
str_concatenator << "text " << 23 << " other text";

// to get back the resulting string from above there are three options:
std::string text = str_concatenator.str(); //gets the string "text 23 other text"
std::string text = str_concatenator.get(); //gets the string "text 23 other text"
std::string text = str_concatenator();     //gets the string "text 23 other text"
// these 3 are the same: they concatenate the string (same as std::cout) and returns the string, cleaning the buffer. So, you feel free to choose the one you like the most.

// Now, for a Matrix<type> object, code
Matrix<int> M = bla bla bla;
str_concatenator(M); //use the () operator to uncharge and set into M the string
// It gets the text as above and inserts into the matrix "comment"
// Matrix comments are shown when printed right in front of it. Example:
// Matrix (3 x 5) [ this is a comment
//   +  0   1   2   3   4
//  0|  6   11 -3   9  NaN
//  1|  Inf 0   67 -7  NaN
```

### *New type to simulate "bool" type*

The boolean type is stored in batches inside a byte (byte is the actual unity of memory used in computations, even though a bit is the least amount of memory; a byte contains 8 bits). Example: to store `bool mybool_isok = true;` it takes only the last position of the byte `0000.0001`, and if I need an array with 3 booleans, the compiler will take the same byte, and occupies the last 3 positions:
`bool[3] array = {true,true,false}`  is stored as `0000.0110` to occupy the smallest memory possible. Even this is good for saving memory, this is terrible for large scale computations. In such case, we change the `bool` model to the `j5r_bool` model: this new is only a skin for an `uint8_t` type (`uint8_t` stores values from 0 to 255 instead of 0 to 1, the true `bool`). So, in our example, `j5r_bool mybool_isok = true;`  now is stored as `0000.0001` (same thing) but the array `j5r_bool[3] array = {true,true,false}` is stored as `0000.0001 0000.0001 0000.0000` (one byte for each `true/false`). The trade-off choice here is for computing speed, not for memory saving.

Thus, when you need a `Matrix<bool>` type, you must use `Matrix<j5r_bool>` or also the alias `Matrixbool`. Relational comparisons with matrices aways return a `Matrixbool` type (operations like `"A >= B"`, `"A != C"` etc.). `Matrix<bool>` objects are not allowed to be instantiated.

Due to this new type, some type traits are inserted into `std` namespace, even though this looks not be the recomended action to do, it is safe and correct.


### *Colors to print in the terminal*

Six colors are possible: before a `std::string` you can add one of these to send to `std::cout`:
 - `_red`
 - `_blue`
 - `_green`
 - `_yellow`
 - `_cyan`
 - `_magenta`
 - `_reset` cleans the format and brings back the terminal's standard color
  
Example: 

`std::cout << _cyan + "my message in cyan color" + _reset;`  (notice that we use "+" to concatenate these strings, but you may also use "<<" instead of "+", it is up to you)

Colors inside a `static_assert()` are treated differently: we use directives for the compiler. Only the **red** color is defined and a **reset**. Use the constants `STATIC_ASSERT_RED` and `STATIC_ASSERT_RESET` if needed.

Example:

`static_assert(test_is_twelve==12, STATIC_ASSERT_RED "assert test failed since the value is not twelve" STATIC_ASSERT_RESET);` (notice that we do not use the "+" sign here)


### *Infs and NaNs*

You can use `mytype inf = inf_<mytype>();` to get an instance of infinite value of the type `mytype`. Analogously `mytype nan = nan_<mytype>();` gets an instance of Not-a-Number (nan) value of type `mytype`. You can evaluate if a `value` is inf or nan with `std::isinf(value)` and `std::isnan(value)` or simply `isinf_(value)` and `isnan_(value)`.

Example
```c++
double inf = inf_<double>(); // creates an "inf" object of type double
Matrix<double> A(2,2);       // states the matrix "A"
A(0,1) = inf;                // injects the object "inf" into the matrix 
// A is now
// [0 inf
//  0  0]
```

### *Axis = row or column*

Along your program, you may need to inform the sense of a summation, for instance. Would you like to sum row-wise or column-wise? You need to inform via an Enumeration (enum for short) called `Axis`. It is simple and direct: to inform "row" you need to inform `Axis::row`, for "column", `Axis::col`. **If the sense is not informed, it sums everything.** The same logic also applies for some other operations.



.

.

.

.

.

.

.

.

.

.

.

.

.

.

.

.

.

.

.

.

--------------------

**FROM NOW ON, THIS IS ONLY A TRASH:** NOTES TO MYSELF TO HELP ME, **JUST IGNORE IT**, I WILL REMOVE IT IN THE FUTURE

-------------------


Operadores que PODEM ser sobrecarregados
A maioria dos operadores aritméticos, lógicos e de acesso pode ser redefinida: 

Aritméticos: +, -, *, /, %, ++, --.
Relacionais: ==, !=, <, >, <=, >=.
Atribuição: =, +=, -=, *=, /=, etc.
Lógicos e Bitwise: &&, ||, !, &, |, ^, <<, >>.


Especiais:
-> (Acesso a membro por ponteiro).
[] (Subscrito/Array).
() (Chamada de função/Functor).
new, delete (Gerenciamento de memória).
* (Desreferenciação unária).
, (Vírgula). 















```c++
#include <type_traits>


template <typename U>
auto operator++++(Matrix<U>&other){
  using ResultType = std::common_type_t<T, U>;
  Matrix<ResultType> result(rows_, cols_);
}
```

---

```c++
// --- OPERAÇÕES COM ESCALAR À ESQUERDA (TIPOS MISTOS) ---

    // [O template U representa o tipo do escalar que está a chegar]
    template <typename U>
    friend auto operator+(const U& scalar, const Matrix& m) {
        
        // [Garante que o tipo resultante comporte tanto o escalar U quanto a matriz T]
        using ResultType = std::common_type_t<U, T>;
        
        Matrix<ResultType> result(m.rows(), m.cols());
        
        for (size_t i = 0; i < m.rows(); ++i) {
            for (size_t j = 0; j < m.cols(); ++j) {
                // [A operação ocorre normalmente e o resultado é guardado no novo tipo promovido]
                result(i, j) = scalar + m(i, j); 
            }
        }
        
        return result;
    }

    // [Exemplo análogo para a multiplicação]
    template <typename U>
    friend auto operator*(const U& scalar, const Matrix& m) {
        using ResultType = std::common_type_t<U, T>;
        Matrix<ResultType> result(m.rows(), m.cols());
        
        for (size_t i = 0; i < m.rows(); ++i) {
            for (size_t j = 0; j < m.cols(); ++j) {
                result(i, j) = scalar * m(i, j);
            }
        }
        
        return result;
    }
```

# Matrix

 - [ ] operadores aritmeticos
   - [ ] operadores entre matrizes + - / elementwise e * multiplicacao matricial .prod() multiplicacao elementwise
   - [ ] escalar vs matriz
 
 - [ ] operadores booleanos
   - [ ] operadores entre matrizes > < >= <= == !=
   - [ ] escalar vs matriz

- [x] ler/escrever em arquivo
- [X] imprimir em tela
- [ ] diag, trace, triu, tril, transpose
- [ ] sum, prod, min, max, mean, var, std, cumsum

- [ ] static: eye

# MatrixRandom
 - [ ] rand(dim1,dim2)
 - [ ] randi(min,max,dim1,dim2)
 - [ ] randn(dim1,dim2)
 - [ ] randexp(lambda,dim1,dim2)
 - [ ] randp(distribution,dim1,dim2)

# MatrixShape
- [x] hconcat(A,B) 
- [x] vconcat(A,B)
- [ ] blkdiag(A,B)
- [x] hexpand(A,ncols)
- [x] vexpand(A,nrows)
- [ ] expand(A,ncols,nrows)

# AlgLin
- [ ] struct-options: algorithm, tolerance, x0, max_iters,
- [ ] kron(A,B)
- [ ] sumkron(A,B)
- [ ] eig(A) `eigenvalues and eigenvectors`
- [ ] svd(A) `singular values and singular vectors`
- [ ] solve(A,b,options)






  