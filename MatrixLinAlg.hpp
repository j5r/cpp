#pragma once
#include "Matrix.hpp"
#include "MatrixShape.hpp"
#include <tuple>
#include <cmath>
#include <algorithm>

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

  template <typename T>
  static Matrix<T> gauss_el(const Matrix<T> &m, const Matrix<T> &b)
  {
    Matrix<T> augmented_m = MatrixShape::hconcat(m, b);
    bool not_singular = true;
    for (size_t i = 0; i < augmented_m.rows(); i++)
    {
      not_singular = MatrixLinAlg::_pivot(augmented_m, i, i);
    }
    if (!not_singular)
    {
      std::cout << _magenta + "\nMatrixLinAlg::gauss_el() Warning: singular matrix detected. Result may be not accurate.\n" +
                       _reset;
    }
    return augmented_m.getcols(m.cols(), augmented_m.cols());
  }

  // ==========================================
  // MÉTODOS ITERATIVOS
  // ==========================================
  template <typename T>
  static Matrix<T> conjugate_gradient(const Matrix<T> &A, const Matrix<T> &b, T tol = 1e-6, size_t max_iter = 1000)
  {
    // [Garantia de Dimensões]
    if (!A.is_square() || A.rows() != b.rows())
    {
      throw std::invalid_argument("\nMatrixAlgLin::conjugate_gradient() Dimension mismatch.\n");
    }

    Matrix<T> x(b.rows(), 1);  // Chute inicial x0 = vetor de zeros
    Matrix<T> r = b - (A * x); // Resíduo inicial r0 = b - Ax0
    Matrix<T> p = r;           // Direção de busca inicial p0 = r0

    // [Produto escalar r^T * r]. Como retorna uma matriz 1x1, extraímos o escalar com (0,0)
    T rs_old = (r.t() * r)(0, 0);

    for (size_t i = 0; i < max_iter; i++)
    {
      Matrix<T> Ap = A * p;
      T pAp = (p.t() * Ap)(0, 0); // Direção conjugada p^T * A * p

      T alpha = rs_old / pAp; // Tamanho do passo

      x += (p * alpha);  // Atualiza a solução
      r -= (Ap * alpha); // Atualiza o resíduo

      T rs_new = (r.t() * r)(0, 0);

      // [Critério de Parada] Se a norma do resíduo (raiz do produto escalar) for menor que a tolerância
      if (std::sqrt(rs_new) < tol)
      {
        // Opcional: imprimir o número de iterações (ex: std::cout << "Convergiu em " << i << " passos.\n";)
        break;
      }

      T beta = rs_new / rs_old; // Fator de correção da direção
      p = r + (p * beta);       // Nova direção de busca

      rs_old = rs_new;
    }

    return x;
  }

  template <typename T>
  static Matrix<T> bicgstab(const Matrix<T> &A, const Matrix<T> &b, T tol = 1e-6, size_t max_iter = 1000)
  {
    // [Garantia de Dimensões]
    if (!A.is_square() || A.rows() != b.rows())
    {
      throw std::invalid_argument("\nMatrixAlgLin::bicgstab() Dimension mismatch.\n");
    }

    Matrix<T> x(b.rows(), 1);  // Chute inicial x0 = vetor de zeros
    Matrix<T> r = b - (A * x); // Resíduo inicial r0 = b - Ax0
    Matrix<T> r_hat = r;       // r_hat arbitrário (geralmente r0)

    // Vetores de suporte zerados
    Matrix<T> v(b.rows(), 1);
    Matrix<T> p(b.rows(), 1);

    T rho_prev = 1.0;
    T alpha = 1.0;
    T omega = 1.0;

    // Norma inicial para o critério de parada relativo (mais robusto!)
    T norm_b = std::sqrt((b.t() * b)(0, 0));
    if (norm_b == 0.0)
      return x; // Se b for zero, x é zero

    for (size_t i = 0; i < max_iter; i++)
    {
      T rho = (r_hat.t() * r)(0, 0);

      // Proteção contra divisão por zero (breakdown do método)
      if (std::abs(rho) < 1e-14)
      {
        std::cout << _magenta << "\nBiCGSTAB Warning: Breakdown detected (rho ~ 0).\n"
                  << _reset;
        break;
      }

      T beta = (rho / rho_prev) * (alpha / omega);

      // Nova direção de busca: p = r + beta * (p - omega * v)
      p = r + (p - (v * omega)) * beta;

      // Multiplicação matriz-vetor (a parte mais custosa)
      v = A * p;

      T r_hat_v = (r_hat.t() * v)(0, 0);
      alpha = rho / r_hat_v;

      // Passo intermediário (Half-step)
      Matrix<T> s = r - (v * alpha);

      // Verifica convergência antecipada
      T norm_s = std::sqrt((s.t() * s)(0, 0));
      if (norm_s / norm_b < tol)
      {
        x = x + (p * alpha);
        break;
      }

      // Passo de estabilização
      Matrix<T> t = A * s;
      T t_s = (t.t() * s)(0, 0);
      T t_t = (t.t() * t)(0, 0);
      omega = t_s / t_t;

      // Atualiza a solução e o resíduo final da iteração
      x = x + (p * alpha) + (s * omega);
      r = s - (t * omega);

      // Verifica convergência final
      T norm_r = std::sqrt((r.t() * r)(0, 0));
      if (norm_r / norm_b < tol)
      {
        // std::cout << "BiCGSTAB Convergiu em " << i + 1 << " iteracoes.\n";
        break;
      }

      rho_prev = rho;
    }

    return x;
  }

  // ==========================================
  // MÉTODO GMRES (Generalized Minimal Residual)
  // ==========================================
  template <typename T>
  static Matrix<T> gmres(const Matrix<T> &A, const Matrix<T> &b, size_t m = 50, T tol = 1e-6)
  {
    if (!A.is_square() || A.rows() != b.rows())
    {
      throw std::invalid_argument("\nMatrixAlgLin::gmres() Dimension mismatch.\n");
    }

    size_t n = b.rows();
    Matrix<T> x(n, 1); // Chute inicial x0 = vetor de zeros
    Matrix<T> r = b - (A * x);

    T b_norm = std::sqrt((b.t() * b)(0, 0));
    if (b_norm == 0.0)
      return x; // O sistema já está resolvido

    T r_norm = std::sqrt((r.t() * r)(0, 0));

    // Matriz V (armazena a base ortonormal do subespaço de Krylov)
    Matrix<T> V(n, m + 1);

    // Matriz de Hessenberg H
    Matrix<T> H(m + 1, m);

    // Vetor g (o lado direito do minúsculo sistema de mínimos quadrados)
    Matrix<T> g(m + 1, 1);
    g(0, 0) = r_norm;

    // Arrays para guardar os senos e cossenos das Rotações de Givens
    std::vector<T> cs(m, 0.0);
    std::vector<T> sn(m, 0.0);

    // O primeiro vetor da base ortogonal (v1 = r0 / ||r0||)
    for (size_t i = 0; i < n; i++)
    {
      V(i, 0) = r(i, 0) / r_norm;
    }

    size_t k = 0;
    for (; k < m; k++)
    {
      // --- 1. PROCESSO DE ARNOLDI (Ortogonalização de Gram-Schmidt) ---
      Matrix<T> w = A * V.getcol(k); // Pega a coluna k e multiplica por A

      for (size_t i = 0; i <= k; i++)
      {
        Matrix<T> vi = V.getcol(i);
        H(i, k) = (vi.t() * w)(0, 0); // Produto escalar
        w = w - (vi * H(i, k));       // Subtração de projeções
      }

      H(k + 1, k) = std::sqrt((w.t() * w)(0, 0)); // Norma do vetor ortogonalizado

      // Se a norma não for zero (evita divisão por zero)
      if (H(k + 1, k) > 1e-14)
      {
        for (size_t i = 0; i < n; i++)
        {
          V(i, k + 1) = w(i, 0) / H(k + 1, k);
        }
      }

      // --- 2. ROTAÇÕES DE GIVENS (Transforma H numa matriz triangular) ---
      // Aplica as rotações antigas à nova coluna gerada
      for (size_t i = 0; i < k; i++)
      {
        T temp = cs[i] * H(i, k) + sn[i] * H(i + 1, k);
        H(i + 1, k) = -sn[i] * H(i, k) + cs[i] * H(i + 1, k);
        H(i, k) = temp;
      }

      // Calcula o novo ângulo (seno e cosseno) para zerar o elemento H(k+1, k)
      T beta = std::sqrt(H(k, k) * H(k, k) + H(k + 1, k) * H(k + 1, k));
      if (beta == 0.0)
      {
        cs[k] = 1.0;
        sn[k] = 0.0;
      }
      else
      {
        cs[k] = H(k, k) / beta;
        sn[k] = H(k + 1, k) / beta;
      }

      // Aplica a nova rotação em H
      H(k, k) = cs[k] * H(k, k) + sn[k] * H(k + 1, k);
      H(k + 1, k) = 0.0; // Foi matematicamente aniquilado!

      // Atualiza o vetor de resíduos transformado
      g(k + 1, 0) = -sn[k] * g(k, 0);
      g(k, 0) = cs[k] * g(k, 0);

      // --- 3. CRITÉRIO DE PARADA ---
      // A beleza do GMRES: o erro atual cai direto no elemento g(k+1) !
      T error = std::abs(g(k + 1, 0)) / b_norm;
      if (error < tol)
      {
        k++; // Ajusta k para o tamanho real do subespaço construído
        break;
      }
    }

    // --- 4. RETRO-SUBSTITUIÇÃO (Resolve Hy = g) ---
    // O sistema foi reduzido de NxN para kxk !
    Matrix<T> y(k, 1);
    // Atenção: usando int aqui pois size_t em ordem decrescente causaria underflow se chegar a -1
    for (int i = k - 1; i >= 0; i--)
    {
      y(i, 0) = g(i, 0);
      for (int j = i + 1; j < (int)k; j++)
      {
        y(i, 0) -= H(i, j) * y(j, 0);
      }
      y(i, 0) /= H(i, i);
    }

    // --- 5. RECUPERAÇÃO DA SOLUÇÃO (x = x0 + Vy) ---
    for (size_t i = 0; i < k; i++)
    {
      x = x + (V.getcol(i) * y(i, 0));
    }

    return x;
  }

  // ==========================================
  // DECOMPOSIÇÃO LU E APLICAÇÕES
  // ==========================================

  // 1. O "Motor" Principal: Fatoração PA = LU
  template <typename T>
  static std::tuple<Matrix<T>, Matrix<T>, Matrix<T>, size_t> factorize_LU(const Matrix<T> &A)
  {
    if (!A.is_square())
    {
      throw std::invalid_argument("\nMatrixAlgLin::factorize_LU() Matrix must be square.\n");
    }

    size_t n = A.rows();
    Matrix<T> L(n, n);
    Matrix<T> U = A;
    Matrix<T> P(n, n);
    size_t swaps = 0;

    // Inicializa L e P como matrizes identidades
    for (size_t i = 0; i < n; i++)
    {
      L(i, i) = 1.0;
      P(i, i) = 1.0;
    }

    for (size_t k = 0; k < n; k++)
    {
      // --- Pivoteamento Parcial ---
      size_t pivot_row = k;
      T max_val = std::abs(U(k, k));

      for (size_t i = k + 1; i < n; i++)
      {
        if (std::abs(U(i, k)) > max_val)
        {
          max_val = std::abs(U(i, k));
          pivot_row = i;
        }
      }

      // Se o maior pivô for zero, a matriz é singular. O algoritmo ignora a coluna.
      if (max_val < 1e-14)
        continue;

      // Troca as linhas se um pivô maior foi encontrado
      if (pivot_row != k)
      {
        for (size_t j = 0; j < n; j++)
        {
          std::swap(U(k, j), U(pivot_row, j));
          std::swap(P(k, j), P(pivot_row, j));
        }
        // Na matriz L, trocamos apenas os multiplicadores já calculados (antes da coluna k)
        for (size_t j = 0; j < k; j++)
        {
          std::swap(L(k, j), L(pivot_row, j));
        }
        swaps++;
      }

      // --- Eliminação de Gauss para preencher L e U ---
      for (size_t i = k + 1; i < n; i++)
      {
        T factor = U(i, k) / U(k, k);
        L(i, k) = factor;
        U(i, k) = 0.0; // Garante o zero absoluto abaixo do pivô

        for (size_t j = k + 1; j < n; j++)
        {
          U(i, j) -= factor * U(k, j);
        }
      }
    }
    P.msg("[P,L,U,swaps] = MatrixLinAlg::factorize_LU(A) matrix \"P\", in which PA=LU");
    L.msg("[P,L,U,swaps] = MatrixLinAlg::factorize_LU(A) matrix \"L\", in which PA=LU");
    U.msg("[P,L,U,swaps] = MatrixLinAlg::factorize_LU(A) matrix \"U\", in which PA=LU");
    return {P, L, U, swaps};
  }

  // 2. Resolução de Sistemas (Usando o motor LU)
  template <typename T>
  static Matrix<T> solve_system_LU(const Matrix<T> &A, const Matrix<T> &b)
  {
    auto [P, L, U, swaps] = factorize_LU(A);

    // Aplica as permutações no vetor resultado: Pb = P * b
    Matrix<T> Pb = P * b;
    size_t n = A.rows();

    // Passo 1: Retro-substituição frontal (Resolve L * y = Pb)
    Matrix<T> y(n, 1);
    for (size_t i = 0; i < n; i++)
    {
      y(i, 0) = Pb(i, 0);
      for (size_t j = 0; j < i; j++)
      {
        y(i, 0) -= L(i, j) * y(j, 0);
      }
    }

    // Passo 2: Retro-substituição traseira (Resolve U * x = y)
    Matrix<T> x(n, 1);
    for (int i = n - 1; i >= 0; i--)
    { // Casting nativo usando int
      x(i, 0) = y(i, 0);
      for (int j = i + 1; j < (int)n; j++)
      {
        x(i, 0) -= U(i, j) * x(j, 0);
      }
      if (std::abs(U(i, i)) < 1e-14)
      {
        std::cout << _magenta << "\nWarning: System is singular or badly scaled.\n"
                  << _reset;
        break; // Proteção contra divisão por zero
      }
      x(i, 0) /= U(i, i);
    }

    return x;
  }

  // 3. Determinante Ultrarrápido O(N^3)
  template <typename T>
  static T det(const Matrix<T> &A)
  {
    auto [P, L, U, swaps] = factorize_LU(A);

    // O sinal inverte a cada troca de linha que fizemos no pivoteamento!
    T determinant = (swaps % 2 == 0) ? 1.0 : -1.0;

    // O determinante de matrizes triangulares é o produto da diagonal
    for (size_t i = 0; i < A.rows(); i++)
    {
      determinant *= U(i, i);
    }
    return determinant;
  }

  // 4. Checagem de Singularidade Eficiente
  template <typename T>
  static bool is_singular(const Matrix<T> &A, T tol = 1e-12)
  {
    auto [P, L, U, swaps] = factorize_LU(A);

    // Se houver qualquer zero na diagonal de U, a matriz não tem posto completo
    for (size_t i = 0; i < A.rows(); i++)
    {
      if (std::abs(U(i, i)) < tol)
      {
        return true;
      }
    }
    return false;
  }

  // 5. Cálculo do Posto (Rank) via LU
  template <typename T>
  static size_t fast_rank(const Matrix<T> &A, T tol = 1e-9)
  {
    // Nota: A nossa implementação atual de LU exige matrizes quadradas.
    // Para matrizes retangulares, precisaríamos do SVD ou QR.
    if (!A.is_square())
    {
      throw std::invalid_argument("\nMatrixAlgLin::rank() Current LU implementation requires square matrix.\n");
    }

    auto [P, L, U, swaps] = factorize_LU(A);
    size_t r = 0;

    // Conta quantos elementos da diagonal de U sobreviveram (são maiores que a tolerância)
    for (size_t i = 0; i < A.rows(); i++)
    {
      if (std::abs(U(i, i)) > tol)
      {
        r++;
      }
    }
    return r;
  }

  

template <typename T>
static T norm(const Matrix<T> &A, float kind = 2.0f) noexcept
{
    if (A.is_empty()) return T();

    // Norma do Máximo (-1): Maior valor absoluto entre todos os elementos
    if (kind == -1.0f) {
        T max_val = 0;
        for (size_t i = 0; i < A.rows() * A.cols(); i++) {
            max_val = std::max(max_val, std::abs(A(i)));
        }
        return max_val;
    }
    
    // Norma Infinito (-2): Máxima soma absoluta das linhas [max row sum]
    if (kind == -2.0f) {
        T max_sum = 0;
        for (size_t i = 0; i < A.rows(); i++) {
            T current_sum = 0;
            for (size_t j = 0; j < A.cols(); j++) {
                current_sum += std::abs(A(i, j));
            }
            max_sum = std::max(max_sum, current_sum);
        }
        return max_sum;
    }

    // Norma 1 Induzida (-3): Máxima soma absoluta das colunas [max column sum]
    if (kind == -3.0f) {
        T max_sum = 0;
        for (size_t j = 0; j < A.cols(); j++) {
            T current_sum = 0;
            for (size_t i = 0; i < A.rows(); i++) {
                current_sum += std::abs(A(i, j));
            }
            max_sum = std::max(max_sum, current_sum);
        }
        return max_sum;
    }

    // Norma-p elemento a elemento (Default)
    // Se kind == 2.0, esta é a Norma de Frobenius ultrarrápida O(N)
    T sum_ = 0;
    for (size_t i = 0; i < A.rows() * A.cols(); i++) {
        sum_ += std::pow(std::abs(A(i)), kind);
    }
    return std::pow(sum_, 1.0f / kind);
}

template <typename T>
static T norm(const Matrix<T> &A, const std::string& kind) 
{
if(kind=="max"){
  return norm(A,-1.0f);
}
if(kind=="inf"){
  return norm(A,-2.0f);
}
if(kind=="ind"||kind=="induced"){
  return norm(A,-3.0f);
}
if(kind=="fro"){
  return norm(A,2.0f);
}
throw std::invalid_argument(_red+"MatrixLinAlg::norm() Values allowed are positive numbers or\n'max', 'inf', 'ind'='induced' or 'fro' for Frobenius."
+_reset);
}

  // ==========================================
  // DECOMPOSIÇÃO SVD E RANK VERDADEIRO
  // ==========================================

  // 1. SVD via Método de Jacobi Unilateral (Hestenes)
  // Retorna a tupla [U, S, V] onde A = U * S * V.t()
  template <typename T>
  static std::tuple<Matrix<T>, Matrix<T>, Matrix<T>> svd(const Matrix<T> &A, size_t max_sweeps = 30, T tol = 1e-10)
  {
    size_t m = A.rows();
    size_t n = A.cols();

    Matrix<T> U = A;   // Inicialmente U recebe A (ao final será U ortogonal)
    Matrix<T> V(n, n); // V começa como identidade
    Matrix<T> S(n, n); // S será a matriz diagonal de valores singulares

    for (size_t i = 0; i < n; i++)
    {
      V(i, i) = 1.0;
    }

    T eps = 1e-15; // Limiar de zero absoluto

    // "Varreduras" (Sweeps) de ortogonalização
    for (size_t sweep = 0; sweep < max_sweeps; sweep++)
    {
      bool converged = true;

      // Compara todas as combinações possíveis de pares de colunas (j, k)
      for (size_t j = 0; j < n - 1; j++)
      {
        for (size_t k = j + 1; k < n; k++)
        {

          T p = 0.0, q = 0.0, r = 0.0;
          // Calcula o produto interno entre as colunas j e k de U
          for (size_t i = 0; i < m; i++)
          {
            p += U(i, j) * U(i, k); // Produto escalar (j * k)
            q += U(i, j) * U(i, j); // Norma da coluna j ao quadrado
            r += U(i, k) * U(i, k); // Norma da coluna k ao quadrado
          }

          // Se as colunas não são ortogonais, aplica a rotação de Jacobi
          if (std::abs(p) > tol * std::sqrt(q * r))
          {
            converged = false;

            // Cálculo do ângulo da Rotação de Givens
            T theta = (q - r) / (2.0 * p);
            T t = (theta >= 0.0 ? 1.0 : -1.0) / (std::abs(theta) + std::sqrt(1.0 + theta * theta));
            T c = 1.0 / std::sqrt(1.0 + t * t);
            T s = t * c;

            // Aplica a rotação nas colunas de U e V
            for (size_t i = 0; i < m; i++)
            {
              T u_ij = U(i, j);
              T u_ik = U(i, k);
              U(i, j) = c * u_ij + s * u_ik;
              U(i, k) = -s * u_ij + c * u_ik;
            }
            for (size_t i = 0; i < n; i++)
            {
              T v_ij = V(i, j);
              T v_ik = V(i, k);
              V(i, j) = c * v_ij + s * v_ik;
              V(i, k) = -s * v_ij + c * v_ik;
            }
          }
        }
      }
      if (converged)
        break; // Se nenhuma coluna precisou de rotação, terminámos!
    }

    // Extrai os Valores Singulares e Normaliza a matriz U
    for (size_t j = 0; j < n; j++)
    {
      T norm = 0.0;
      for (size_t i = 0; i < m; i++)
      {
        norm += U(i, j) * U(i, j);
      }
      norm = std::sqrt(norm);
      S(j, j) = norm; // Guarda na matriz diagonal

      // Normaliza a coluna de U para que tenha comprimento 1
      if (norm > eps)
      {
        for (size_t i = 0; i < m; i++)
        {
          U(i, j) /= norm;
        }
      }
      else
      {
        for (size_t i = 0; i < m; i++)
        {
          U(i, j) = 0.0;
        }
      }
    }

    U.msg("[U,S,V] = MatrixLinAlg::svd(A) - matrix \"U\" in which A = U * S * V.t()");
    S.msg("[U,S,V] = MatrixLinAlg::svd(A) - matrix \"S\" in which A = U * S * V.t()");
    V.msg("[U,S,V] = MatrixLinAlg::svd(A) - matrix \"V\" in which A = U * S * V.t()");
    return {U, S, V};
  }

  // 2. Cálculo do Rank Verdadeiro (Impossível de enganar)
  template <typename T>
  static size_t rank(const Matrix<T> &A, T tol = 1e-9)
  {
    auto [U, S, V] = svd(A);
    size_t r = 0;

    // O rank é exatamente o número de valores singulares maiores que zero!
    for (size_t i = 0; i < S.rows(); i++)
    {
      if (S(i, i) > tol)
      {
        r++;
      }
    }
    return r;
  }
};
