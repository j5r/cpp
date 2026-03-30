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


## **Class Matrix\<type\>**

In what follows, when signed with `¹`, the input parameter is optional; `Axis` is one of `Axis::row` or `Axis::col`; 

### *Constructors*

 - `Matrix<int> A;` empty matrix (rows=0, cols=0). This is just a statement of the variable "A";
 - `Matrix<double> B(m,n);` full matrix with m rows and n columns;
 - `Matrix<float> C(m,n,11);` full matrix with m rows and n columns, whose all entries are `11`;
 - `Matrix<double> D = A;` casting from type `<int>` (matrix "A" - implicit casting), to type `<double>`;
 - `Matrix<double> E = (Matrix<double>) A;` casting from type `<int>` (matrix "A" - explicit casting), to type `<double>`;
 - `Matrix<float> F = {{1,2,3},{4,5,6}};` initializing the matrix with two rows and three columns;


### *Asking about structure: methods "is_xxxxx()" (returns true or false)*

 - `.is_empty();` returns true whenever the matrix has rows=0=columns;
 - `.is_square();` returns true whenever the matrix has rows=columns;
 - `.is_scalar();` returns true whenever the matrix has rows=1=columns;
 - `.is_rowvector();` returns true whenever row=1 and is not scalar;
 - `.is_colvector();` returns true whenever col=1 and is not scalar;
 - `.is_vector();` returns true whenever one of `is_rowvector()` or `is_colvector()` is true;
 - `.is_symmetric();` returns true whenever the matrix is symmetric.
 - `.is_lower_triangular(bool strictly_lower = false);` returns true if the matrix is lower triangular; When `strictly_lower=true` and there is a non-zero in the diagonal, it returns false.
 - `.is_upper_triangular(bool strictly_upper = false);` the analogous operation as above, but for upper triangular matrices.

### *Other getters*
 
 - `.rows();` returns the number of rows;
 - `.cols();` returns the number of columns;
 - `.ilrow();` returns the index of the last row (the same as `.rows()-1;`);
 - `.ilcol();` returns the index of the last column (the same as `.cols()-1;`);
 - `.numel();` returns the number of elements of the matrix (the same as `.rows()*.cols();`);
 - `.get_comment();` returns the comment of the matrix (a `std::string`);
 - `.size();` returns the size of the matrix as a `std::string` (example: it returns "3x15");


## **Methods**

### *I/O methods*
 - `.printsize();` it is the same as `std::cout << M.size();` for a matrix `M`;
 - `.msg(std::string);` it sets a comment into the matrix data, to be printed in the screen: as an example, suppose we have a matrix `M`.
 ```c++
// EXAMPLE 1
 Matrix<int> M = {{91, 79, -14}, {10, 11, 23}};
 M.msg("this is the comment, it might be an observation or the name of the matrix, for instance");
 std::cout << M;
 // Matrix (2 x 3) [ this is the comment, it might be an observation or the name of the matrix, for instance
 // +   0     1    2
 // 0|  91   79  -14
 // 1|  10   11   23
 //  ] appendix here
 ``` 
 - `.print(std::string appendix="");` it is the same as `std::cout << M;` but it adds an appendix at the end of the screen, see the Example 1;
 - `.from_promt();` it asks the user for the number of rows, columns and asks each of the entries of the matrix;
 - `.from_promt(size_t nrows,size_t ncols);` it asks the user for each of the entries of the matrix with **nrows** rows and **ncols** columns;
 - `.get(size_t index);` it returns the "index-th" entry of the matrix, following the order (0,0), (0,1), (0,2) ... (1,0), (1,1), ... (passing row by row). This method throws error if the index is out of the interval **[-numel() : numel()-1]**. To have higher speed performance, use the `M(index)` syntax (but it does not throws errors when accessing wrong indexes, so be careful). See the next example.
```c++
 // EXAMPLE 2
 Matrix<int> M = {{91, 79, -14}, {10, 11, 23}};
 //   index =       0   1    2     3   4   5    or
 //   index =      -6  -5   -4    -3  -2  -1
 std::cout << M(-1) << std::endl; // 23
 std::cout << M(-2) << std::endl; // 11
 std::cout << M(-4) << std::endl; // -14
 std::cout << M(2) << std::endl; // -14
 std::cout << M(0) << std::endl; // 91
 std::cout << M(3) << std::endl; // 10
 std::cout << M(4) << std::endl; // 11
 ```
 - `.get(size_t i,size_t j);` it returns the "(i,j)-th" entry (also throws errors.) To have higher performance, use `M(i,j)` syntax, but be careful since it does not check if you are acessing wrong indexes. The valid indexes for rows are the interval **[-rows() : rows()-1]** and for columns are **[-cols() : cols()-1]** and follows analogous logic as the previous `.get(index)` method. See the next example.
 ```c++
// EXAMPLE 3
//                                       //   row =    or   row
 Matrix<int> M = { {91,   79,   -14},    //       0          -2
                   {10,   11,    23} };  //       1          -1
 //         col =    0     1     2
 //    or   col =   -3    -2    -1
 std::cout << M(-1, -1) << std::endl; // 23
 std::cout << M(-2,  1) << std::endl; // 79
 std::cout << M( 0, -3) << std::endl; // 91
 std::cout << M( 1,  2) << std::endl; // 23
 ```
 - `.set(size_t index, Type value);` it is the counterpart of `.get(index)` to set a value into the "index" position: see `.get(index)`.
 - `.set(size_t i,size_t j, Type value);` it is the counterpart of `.get(i,j)` to set a value into the "(i,j)" position: see `.get(i,j)`.
 - `.save_to_file(std::string filename, char separator = ';');` it saves the matrix into a text file (like .csv files, values are separated by `separator`).
 - `.load_from_file(std::string filename, char separator = ';');` it loads the matrix from a text file (like .csv files, values are separated by `separator`).
 - `.save_to_binary(std::string filename);` it saves the matrix into a binary file;
 - `.load_from_binary(std::string filename);` it loads the the matrix from a binary file;
   - It is common to use **.dat** or **.bin** extension for a binary file name, like "matrixA.dat".

###  *Shape and structure methods*

 - `.diag();` It always returns a column vector. (1) [Square or rectangular matrix] It returns the diagonal of the matrix as a column vector. (2) [Row or column vector] It returns only the 1st entry of the vector, because it is the only entry that belongs to the diagonal.
 - `.square_diag();` It always returns a square diagonal matrix. (1) [Square or rectangular matrix] It takes the diagonal of the matrix and returns as a diagonal matrix; (2) [Row or column vector] It returns a matrix with the entries of the vector in its diagonal.
- `.reshape(size_t newrows, size_t newcols);` returns a matrix with the same entries, but with "newrows" number of rows and "newcols" number of columns. It works only if `rows()*cols()==newrows*newcols`. See Example 4
```c++
// EXAMPLE 4
Matrix<int> M = {{11, 22}, {33, 44}, {55, 66}}; // size is 3 x 2
M.reshape(2,3).print();  
// Matrix (2 x 3)[      // it takes each element of the 1st row, next 
//   +   0    1    2    // each element of the 2nd row, and so on, filling up
//  0|  11   22   33    // each row of the new matrix.
//  1|  44   55   66
// ]
```
 - `.flatten();` it has the same effect as `M.reshape(1, M.numel())`: returns a row vector.
 - `.copy();` it has the same effect as a simple assignment: creates a new matrix and copies all its entries.
    - **OBSERVATION** There are two ways of copying a matrix: by reference and by value. By reference, both variables point to the same object [See section "*Boolean related methods*" ahead, method "is()"], and by value, both variables has their own objects, even though their entries are the same.
```c++
// EXAMPLE 5
Matrix<int> M = {{11, 22}, {33, 44}, {55, 66}};

Matrix<int> N = M; // value copy: M and N are different objects 
std::cout << M.is(N) << std::endl; // 0 or false

Matrix<int>& P = M; // reference copy: M and P are the same object. Note "&" sign.
std::cout << M.is(N) << std::endl; // 1 or true
M(0,0) = -1; // M changed: N remains the same, but P also changed because it is M.

std::cout << "M(0,0)=" << M(0,0) << ", N(0,0)=" << N(0,0) << ", P(0,0)=" << P(0,0) << std::endl;
// M(0,0)=-1, N(0,0)=11, P(0,0)=-1
```

### *Axis related operations*

The methods that receives an axis as input argument works with the following logic:
- `Axis axis = Axis::row`: the method starts looking to the 1st element of the 1st row and operates from the top to the bottom of the matrix, taking the result and saving it into the 1st position of a row vector. Then it goes to the next column and repeats the process. The resulting matrix is a row vector.
```
  1      5      9     13
  2      6     10     14
  3      7     11     15
  4      8     12     16
  :      :      :      :
[save   save   save   save] row vector as a result
```
- `Axis axis = Axis::col`: the method starts looking to the 1st element of the 1st row and operates from the left to the right of the matrix, taking the result and saving it into the 1st position of a column vector. Then it goes to the next row and repeats the process. The resulting matrix is a column vector.
```
  1      2      3      4    ..  [save
  5      6      7      8    ..   save
  9     10     11     12    ..   save
 13     14     15     16    ..   save] column vector as a result                                  
```


### *Math related methods*
 - `.t();` returns the transpose of a matrix.
 - `.sum(Axis axis = Axis::row);` it sums all the rows.
 - `.sum();` it sums all entries of the matrix.
 - `.cumsum(Axis axis = Axis::row);` it returns a cumulative sum.
 - `.mean(Axis axis = Axis::row);` it returns the mean or average.
 - `.var(Axis axis = Axis::row);` it returns the variance.
 - `.std(Axis axis = Axis::row);` it returns the standard deviation.
 - `.min(Axis axis = Axis::row);` it returns the minimum value.
    - It returns a pair of matrices **[Matrix\<size_t\> indices, Matrix\<Type\> values]**. Example: code `auto [idxs, vals] = M.min();`. The matrix `idxs` is a row/col vector with the indices of the minimum values and `vals` is a row vector with the minimum values.
 - `.max(Axis axis = Axis::row);` it returns the maximum.
    - It returns a pair of matrices **[Matrix\<size_t\> indices, Matrix\<Type\> values]**. Example: code `auto [idxs, vals] = M.max(Axis::col);`. The matrix `idxs` is a row/col vector with the indices of the maximum values and `vals` is a column vector with the maximum values.
 - `.abs();` returns the absolute value matrix (all entries are made non-negative).
 - `.round();` it rounds all the entries of the matrix to the closer integer number.
 - `.ceil();` it rounds up all the entries of the matrix to the closer integer number.
 - `.floor();` it rounds down all the entries of the matrix to the closer integer number.
 - `.triu(bool strictly_above = false);` it returns the upper triangular matrix; if `strictly_above=true`, the diagonal is made zero.
 - `.tril(bool strictly_above = false);` it returns the lower triangular matrix; if `strictly_above=true`, the diagonal is made zero.
 - `.count();` **it works only for Matrixbool or Matrix<j5r_bool> matrices**: it has the same effect of `matrixbool.sum()`.
 - `.count(Axis axis = Axis::row);` **it works only for Matrixbool or Matrix<j5r_bool> matrices**: it has the same effect of `matrixbool.sum(axis)`.

### *Boolean related methods*
 - `.is(Matrix<Type> other);` it compares the memory address of the matrices and returns true if these addresses are equal: Example: `A.is(B)`.
 - `.all();` it returns true if the matrix has no **zero** or **nan** entry.
 - `.all(Axis axis = Axis::row);` same as `.all()`, but specified by axis.
 - `.any();` it returns true if the matrix have some **non-zero** or **non-NaN** entry.
 - `.any(Axis axis = Axis::row);` same as `.any()`, but specified by axis.
 - `M(Matrixbool m)` or `M(Matrix<j5r_bool> m)` **USING BOOLEAN MASK:** `m` is a matrix of type `Matrixbool` or `Matrix<j5r_bool>`. These matrices are result of relational or boolean operations, see sections "*Relational Operators*" and "*Boolean operators*" ahead. These boolean masks inserted into the `()` operator returns a row vector with the entries that corresponds to the boolean "true" in `m`.
 ```c++
 // EXAMPLE 6
 Matrix<int> M = {{11, 22}, {33, 44}, {55, 66}};
 Matrixbool mask = M > 40;
 Matrix<j5r_bool> result = M(mask); // result == {{44, 55, 66}}   row vector
 ```



## *Arithmetic operators*
 - `M.ew_prod(N)` it multiplies both matrices in element-wise sense.
 - `M + N` it sums both matrices: if `M.is_scalar()` or `N.is_scalar()`, the operation is done as Matrix-scalar operation.
 - `M - N` it subtracts both matrices: if `M.is_scalar()` or `N.is_scalar()`, the operation is done as Matrix-scalar operation.
 - `M * N` it multiplies both matrices in algebraic sense (not element-wise sense): if `M.is_scalar()` or `N.is_scalar()`, the operation is done as Matrix-scalar operation.
 - `M / N` it divides both matrices in element-wise sense: if `M.is_scalar()` or `N.is_scalar()`, the operation is done as Matrix-scalar operation.
 - `- M` (unary minus sign) it changes the sign of all the entries of the matrix.

### *Matrix-scalar operations* (same operation in all entries)
 - `M + s` and `s + M` sum of scalar and matrix.
 - `M - s` and `s - M` subtraction of scalar and matrix.
 - `M * s` and `s * M` multiplication of scalar and matrix.
 - `M / s` and `s / M` division of scalar and matrix.
 - `M.pow(s);` it computes the "s-th" power of `M` in algebraic sense (not element-wise sense).
 - `M ^ s` the same as `M.pow(s)`.

## *Relational operators*
 - `.equals(Matrix<Type> N, double tolerance=1e-14);` if type is integer, it compares `M` and `N` in element-wise sense, if floating point, compares in norm sense.
 - `M > N` returns a Matrixbool comparing element-wise; if `M.is_scalar()` or `N.is_scalar()`, the comparison is done as Matrix-scalar operation.
 - `M >= N`  returns a Matrixbool comparing element-wise; if `M.is_scalar()` or `N.is_scalar()`, the comparison is done as Matrix-scalar operation.
 - `M < N`  returns a Matrixbool comparing element-wise; if `M.is_scalar()` or `N.is_scalar()`, the comparison is done as Matrix-scalar operation.
 - `M <= N` returns a Matrixbool comparing element-wise; if `M.is_scalar()` or `N.is_scalar()`, the comparison is done as Matrix-scalar operation.
 - `M == N` returns a Matrixbool comparing element-wise; if `M.is_scalar()` or `N.is_scalar()`, the comparison is done as Matrix-scalar operation.
 - `M != N` returns a Matrixbool comparing element-wise; if `M.is_scalar()` or `N.is_scalar()`, the comparison is done as Matrix-scalar operation.
  
### *Matrix-scalar operations* (same operation in all entries)
 - `M > s` and `s > M`  returns a Matrixbool. 
 - `M >= s` and `s >= M` returns a Matrixbool.
 - `M < s`  and `s < M` returns a Matrixbool.
 - `M <= s` and `s <= M` returns a Matrixbool.
 - `M == s` and `s == M` returns a Matrixbool.
 - `M != s` and `s == M` returns a Matrixbool.

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






  