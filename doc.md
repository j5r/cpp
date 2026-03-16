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






  