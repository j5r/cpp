#pragma once
#include "Matrix.hpp"
#include "MatrixShape.hpp"
#include "MatrixGen.hpp"
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
    P >> "[P,L,U,swaps] = MatrixLinAlg::factorize_LU(A) matrix \"P\", in which PA=LU";
    L >> "[P,L,U,swaps] = MatrixLinAlg::factorize_LU(A) matrix \"L\", in which PA=LU";
    U >> "[P,L,U,swaps] = MatrixLinAlg::factorize_LU(A) matrix \"U\", in which PA=LU";
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
  static T norm(const Matrix<T> &A, T kind = 2.0) noexcept
  {
    if (A.is_empty())
      return T();

    // Norma do Máximo (-1): Maior valor absoluto entre todos os elementos
    if (kind == -1.0)
    {
      T max_val = 0;
      for (size_t i = 0; i < A.rows() * A.cols(); i++)
      {
        max_val = std::max(max_val, std::abs(A(i)));
      }
      return max_val;
    }

    // Norma Infinito (-2): Máxima soma absoluta das linhas [max row sum]
    if (kind == -2.0)
    {
      T max_sum = 0;
      for (size_t i = 0; i < A.rows(); i++)
      {
        T current_sum = 0;
        for (size_t j = 0; j < A.cols(); j++)
        {
          current_sum += std::abs(A(i, j));
        }
        max_sum = std::max(max_sum, current_sum);
      }
      return max_sum;
    }

    // Norma 1 Induzida (-3): Máxima soma absoluta das colunas [max column sum]
    if (kind == -3.0)
    {
      T max_sum = 0;
      for (size_t j = 0; j < A.cols(); j++)
      {
        T current_sum = 0;
        for (size_t i = 0; i < A.rows(); i++)
        {
          current_sum += std::abs(A(i, j));
        }
        max_sum = std::max(max_sum, current_sum);
      }
      return max_sum;
    }

    // Norma-p elemento a elemento (Default)
    // Se kind == 2.0, esta é a Norma de Frobenius ultrarrápida O(N)
    T sum_ = 0;
    for (size_t i = 0; i < A.rows() * A.cols(); i++)
    {
      sum_ += std::pow(std::abs(A(i)), kind);
    }
    return std::pow(sum_, T(1.0) / kind);
  }

  template <typename T>
  static T norm(const Matrix<T> &A, const std::string &kind)
  {
    if (kind == "max")
    {
      return norm(A, -1.0);
    }
    if (kind == "inf")
    {
      return norm(A, -2.0);
    }
    if (kind == "ind" || kind == "induced")
    {
      return norm(A, -3.0);
    }
    if (kind == "fro")
    {
      return norm(A, 2.0);
    }
    throw std::invalid_argument(_red + "MatrixLinAlg::norm() Values allowed are positive numbers or\n'max', 'inf', 'ind'='induced' or 'fro' for Frobenius." + _reset);
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

    U >> "[U,S,V] = MatrixLinAlg::svd(A) - matrix \"U\" in which A = U * S * V.t()";
    S >> "[U,S,V] = MatrixLinAlg::svd(A) - matrix \"S\" in which A = U * S * V.t()";
    V >> "[U,S,V] = MatrixLinAlg::svd(A) - matrix \"V\" in which A = U * S * V.t()";
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

  // ==========================================
  // DECOMPOSIÇÃO DE CHOLESKY (Matrizes Simétricas Positivas Definidas)
  // ==========================================

  // 1. O Motor Cholesky (Retorna {Matriz L, Sucesso})
  template <typename T>
  static std::pair<Matrix<T>, bool> cholesky(const Matrix<T> &A)
  {
    if (!A.is_square())
    {
      throw std::invalid_argument(_red + "\nMatrixAlgLin::cholesky() Matrix must be square.\n" + _reset);
    }

    size_t n = A.rows();
    Matrix<T> L(n, n); // Já inicializada com zeros
    L >> "[L,is_ok] = MatrixAlgLin::cholesky(A) - matrix \"L\" such that A = L * L.t()";

    // Verifica simetria rapidamente (opcional, mas recomendado)
    if (!A.is_symmetric())
      return {L, false};

    for (size_t i = 0; i < n; i++)
    {
      for (size_t j = 0; j <= i; j++)
      {
        T sum = 0.0;

        // Produto escalar da linha i com a linha j de L
        for (size_t k = 0; k < j; k++)
        {
          sum += L(i, k) * L(j, k);
        }

        if (i == j) // Elementos da diagonal principal
        {
          T diag_val = A(i, i) - sum;

          // Se o valor for <= 0, a matriz NÃO é Positiva Definida!
          if (diag_val <= 1e-14)
          {
            return {L, false};
          }

          L(i, i) = std::sqrt(diag_val);
        }
        else // Elementos abaixo da diagonal
        {
          L(i, j) = (A(i, j) - sum) / L(j, j);
        }
      }
    }
    L >> "[L,is_ok] = MatrixAlgLin::cholesky(A) - matrix \"L\" such that A = L * L.t()";
    return {L, true};
  }

  // 2. Verificador Booleano Direto
  template <typename T>
  static bool is_positive_definite(const Matrix<T> &A)
  {
    auto [L, is_pd] = cholesky(A);
    return is_pd;
  }

  // ==========================================
  // APLICAÇÕES DE CHOLESKY
  // ==========================================

  // 1. Resolve o sistema H * dx = -g (O jeito certo de usar no NR)
  // Exige: Matriz Triangular Inferior L e o vetor do lado direito b (ou -g)
  template <typename T>
  static Matrix<T> solve_cholesky(const Matrix<T> &L, const Matrix<T> &b)
  {
    size_t n = L.rows();

    // Passo 1: Resolve L * y = b (Retro-substituição frontal)
    Matrix<T> y(n, 1);
    for (size_t i = 0; i < n; i++)
    {
      y(i, 0) = b(i, 0);
      for (size_t j = 0; j < i; j++)
      {
        y(i, 0) -= L(i, j) * y(j, 0);
      }
      y(i, 0) /= L(i, i);
    }

    // Passo 2: Resolve L^T * x = y (Retro-substituição traseira)
    Matrix<T> x(n, 1);
    for (int i = n - 1; i >= 0; i--)
    {
      x(i, 0) = y(i, 0);
      for (int j = i + 1; j < (int)n; j++)
      {
        x(i, 0) -= L(j, i) * x(j, 0); // Note o L(j, i) simulando a transposta!
      }
      x(i, 0) /= L(i, i);
    }
    x >> "x = MatrixLinAlg::solve_cholesky(L,b) Solve L * L.t() * x = b, for low triangular L";
    return x;
  }

  // 2. Inverte a matriz original SPD explicitamente: H^{-1} = (L * L^T)^{-1}
  template <typename T>
  static Matrix<T> invert_cholesky(const Matrix<T> &L)
  {
    size_t n = L.rows();
    Matrix<T> Inv(n, n);

    // Primeiro, invertemos a matriz L (vamos chamar de L_inv)
    Matrix<T> L_inv(n, n);
    for (size_t j = 0; j < n; j++)
    {
      L_inv(j, j) = 1.0 / L(j, j);
      for (size_t i = j + 1; i < n; i++)
      {
        T sum = 0.0;
        for (size_t k = j; k < i; k++)
        {
          sum -= L(i, k) * L_inv(k, j);
        }
        L_inv(i, j) = sum / L(i, i);
      }
    }

    // Agora, H^{-1} = L_inv^T * L_inv
    // Otimização: Só precisamos calcular a parte triangular e espelhar, pois a inversa de SPD é SPD!
    for (size_t i = 0; i < n; i++)
    {
      for (size_t j = i; j < n; j++)
      {
        T sum = 0.0;
        for (size_t k = j; k < n; k++)
        {
          sum += L_inv(k, i) * L_inv(k, j);
        }
        Inv(i, j) = sum;
        Inv(j, i) = sum; // Espelha para baixo da diagonal
      }
    }
    Inv >> "Ainv = MatrixLinAlg::invert_cholesky(L) Inverse of A such that A = L * L.t()";
    return Inv;
  }

  // ==========================================
  // FAST TRUNCATED SVD (Para PCA em Grande Escala)
  // ==========================================
  template <typename T>
  static std::tuple<Matrix<T>, Matrix<T>, Matrix<T>> fast_svd(const Matrix<T> &A, T threshold = 0.95, size_t max_power_iters = 100)
  {
    size_t m = A.rows();
    size_t n = A.cols();
    size_t max_k = std::min(m, n);

    // Matrizes temporárias para armazenar os componentes à medida que os encontramos
    Matrix<T> U_temp(m, max_k);
    Matrix<T> S_temp(max_k, max_k);
    Matrix<T> V_temp(n, max_k);

    Matrix<T> A_k = A; // Cópia de trabalho que será "esvaziada"

    // 1. Calcula a Variância Total (Norma de Frobenius ao quadrado)
    T total_variance = 0.0;
    for (size_t i = 0; i < m; i++)
    {
      for (size_t j = 0; j < n; j++)
      {
        total_variance += A(i, j) * A(i, j);
      }
    }

    if (total_variance == 0.0)
      return {U_temp, S_temp, V_temp};

    T accumulated_variance = 0.0;
    size_t k = 0;

    // 2. Extrai componentes até atingir o threshold de informação
    for (; k < max_k; k++)
    {
      // Chute inicial para o vetor V (um vetor de uns normalizado)
      Matrix<T> v(n, 1, T(1));
      v /= norm(v, 2.0);

      Matrix<T> u(m, 1);
      T sigma = 0.0;

      // --- Power Iteration ---
      for (size_t iter = 0; iter < max_power_iters; iter++)
      {
        // Multiplica pela matriz e normaliza (encontra vetor u)
        u = A_k * v;
        T u_norm = norm(u, 2.0);
        if (u_norm < 1e-14)
          break;
        u /= u_norm;

        // Multiplica pela transposta e normaliza (encontra vetor v)
        Matrix<T> v_new = A_k.t() * u;
        T v_new_norm = norm(v_new, 2.0);
        if (v_new_norm < 1e-14)
          break;
        v_new /= v_new_norm;

        // Verifica se a direção do vetor estabilizou (produto escalar ~ 1)
        T dot = (v.t() * v_new)(0, 0);
        v = v_new;
        if (std::abs(1.0 - dot) < 1e-6)
        {
          break;
        }
      }

      // --- Extração Final do Componente ---
      u = A_k * v;
      sigma = norm(u, 2.0);
      if (sigma < 1e-14)
        break; // Não há mais informação útil
      u /= sigma;

      // Armazena nas matrizes temporárias
      for (size_t i = 0; i < m; i++)
        U_temp(i, k) = u(i, 0);
      for (size_t i = 0; i < n; i++)
        V_temp(i, k) = v(i, 0);
      S_temp(k, k) = sigma;

      // --- Deflation ---
      // Removemos a informação (matriz de posto 1) que acabamos de extrair
      A_k = A_k - (u * (v.t() * sigma));

      // Atualiza a variância acumulada (sigma^2)
      accumulated_variance += sigma * sigma;

      // Verifica se já extraímos informação suficiente
      if (accumulated_variance / total_variance >= threshold)
      {
        k++; // Contabiliza o componente atual e encerra a extração
        break;
      }
    }

    // 3. Monta e retorna as matrizes finais cortadas com o tamanho exato (k)
    Matrix<T> U_trunc(m, k);
    Matrix<T> S_trunc(k, k);
    Matrix<T> V_trunc(n, k);

    for (size_t j = 0; j < k; j++)
    {
      S_trunc(j, j) = S_temp(j, j);
      for (size_t i = 0; i < m; i++)
        U_trunc(i, j) = U_temp(i, j);
      for (size_t i = 0; i < n; i++)
        V_trunc(i, j) = V_temp(i, j);
    }

    U_trunc >> "[U,S,V] = MatrixLinAlg::fast_svd(A,treshold) - matrix \"U\", [A ~ U * S * V.t()] treshold=" + std::to_string(threshold);
    S_trunc >> "[U,S,V] = MatrixLinAlg::fast_svd(A,treshold) - matrix \"S\", [A ~ U * S * V.t()] treshold=" + std::to_string(threshold);
    V_trunc >> "[U,S,V] = MatrixLinAlg::fast_svd(A,treshold) - matrix \"V\", [A ~ U * S * V.t()] treshold=" + std::to_string(threshold);
    return {U_trunc, S_trunc, V_trunc};
  }

  // ==========================================
  // DECOMPOSIÇÃO ESPECTRAL (Autovalores e Autovetores)
  // ==========================================

  // Decomposição Espectral via Método de Jacobi (Apenas para Matrizes Simétricas)
  // Retorna a tupla {V, D} onde A = V * D * V.t()
  // V = Matriz cujas colunas são os Autovetores Ortogonais
  // D = Matriz Diagonal com os Autovalores
  template <typename T>
  static std::tuple<Matrix<T>, Matrix<T>> eigen_sym(const Matrix<T> &A, size_t max_sweeps = 50, T tol = 1e-10)
  {
    if (!A.is_square())
    {
      throw std::invalid_argument("\nMatrixAlgLin::eigen_sym() Matrix must be square.\n");
    }

    size_t n = A.rows();
    Matrix<T> D = A;         // D começará como A e terminará estritamente diagonal
    Matrix<T> V(n, n, T(0)); // V armazenará os autovetores (inicializada com zeros)

    // Verifica a simetria rapidamente
    for (size_t i = 0; i < n; i++)
    {
      V(i, i) = 1.0; // V começa como matriz Identidade
      for (size_t j = i + 1; j < n; j++)
      {
        if (std::abs(A(i, j) - A(j, i)) > 1e-9)
        {
          std::cout << _yellow << "\nWarning: Matrix is not strictly symmetric. "
                    << "Jacobi method may not converge properly.\n"
                    << _reset;
        }
      }
    }

    for (size_t sweep = 0; sweep < max_sweeps; sweep++)
    {
      bool converged = true;

      // Varredura Cíclica: Passa por todos os elementos acima da diagonal principal
      for (size_t p = 0; p < n - 1; p++)
      {
        for (size_t q = p + 1; q < n; q++)
        {
          // Se o elemento fora da diagonal for maior que a tolerância, precisamos rotacionar!
          if (std::abs(D(p, q)) > tol)
          {
            converged = false;

            // Cálculo do ângulo da Rotação de Jacobi
            T theta = (D(q, q) - D(p, p)) / (2.0 * D(p, q));
            T t = (theta >= 0.0 ? 1.0 : -1.0) / (std::abs(theta) + std::sqrt(1.0 + theta * theta));
            T c = 1.0 / std::sqrt(1.0 + t * t);
            T s = t * c;

            // Aplica a Rotação na matriz D (esmagando o elemento D(p,q) a zero)
            T d_pp = D(p, p);
            T d_qq = D(q, q);
            T d_pq = D(p, q);

            D(p, p) = c * c * d_pp - 2.0 * s * c * d_pq + s * s * d_qq;
            D(q, q) = s * s * d_pp + 2.0 * s * c * d_pq + c * c * d_qq;
            D(p, q) = 0.0;
            D(q, p) = 0.0;

            // Atualiza as outras entradas das linhas/colunas p e q
            for (size_t i = 0; i < n; i++)
            {
              if (i != p && i != q)
              {
                T d_ip = D(i, p);
                T d_iq = D(i, q);
                D(i, p) = c * d_ip - s * d_iq;
                D(p, i) = D(i, p); // Mantém a simetria
                D(i, q) = s * d_ip + c * d_iq;
                D(q, i) = D(i, q); // Mantém a simetria
              }

              // Acumula a rotação na matriz de Autovetores (V)
              T v_ip = V(i, p);
              T v_iq = V(i, q);
              V(i, p) = c * v_ip - s * v_iq;
              V(i, q) = s * v_ip + c * v_iq;
            }
          }
        }
      }
      if (converged)
        break; // A matriz já é puramente diagonal!
    }
    V >> "[V,D] = MatrixLinAlg::eigen_sym(A) - matrix \"V\", such that A = V * D * V.t()";
    D >> "[V,D] = MatrixLinAlg::eigen_sym(A) - matrix \"D\", such that A = V * D * V.t()";

    return {V, D};
  }

  template <typename T>
  static T cos_angle(const Matrix<T> &u, const Matrix<T> &v)
  {
    if (!u.is_vector() || !v.is_vector())
    {
      throw std::invalid_argument(_red + "MatrixLinAlg::cos_angle(u,v) Matrices u and v must be vectors." +
                                  _reset);
    }
    if (u.numel() != v.numel())
    {
      throw std::invalid_argument(_red + "MatrixLinAlg::cos_angle(u,v) Matrices u and v must have the same number of entries." +
                                  "Got u(" + u.size() + ") vs v(" + v.size() + ")." + _reset);
    }
    return (u.flatten() * v.flatten().t())(0) / (norm(u) * norm(v));
  }

  template <typename T>
  static T angle_between_rad(const Matrix<T> u, const Matrix<T> v)
  {
    return std::acos(cos_angle(u, v));
  }

  template <typename T>
  static T angle_between_deg(const Matrix<T> u, const Matrix<T> v)
  {
    return 180 * std::acos(cos_angle(u, v)) / PI;
  }

  // ==========================================
  // GEOMETRIA ANALÍTICA 3D
  // ==========================================

  template <typename T>
  static Matrix<T> cross_product(const Matrix<T> &u, const Matrix<T> &v)
  {
    // [Garantia de Dimensões] O Produto Vetorial é estrito para o R^3
    if (!u.is_vector() || !v.is_vector() || u.numel() != 3 || v.numel() != 3)
    {
      throw std::invalid_argument("\nMatrixAlgLin::cross_product() Both inputs must be 3D vectors.\n");
    }

    // Uniformiza os vetores para garantir que acessamos os índices corretamente
    Matrix<T> uf = u.flatten();
    Matrix<T> vf = v.flatten();

    Matrix<T> result(3, 1); // Retorna sempre um vetor coluna padronizado

    // Regra da Mão Direita:
    // cx = uy*vz - uz*vy
    result(0) = uf(1) * vf(2) - uf(2) * vf(1);
    // cy = uz*vx - ux*vz
    result(1) = uf(2) * vf(0) - uf(0) * vf(2);
    // cz = ux*vy - uy*vx
    result(2) = uf(0) * vf(1) - uf(1) * vf(0);

    result >> "MatrixLinAlg::cross_product() resulting vector";
    return result;
  }

  // ==========================================
  // EXTENSÃO ORTOGONAL (Householder)
  // ==========================================
  template <typename T>
  static Matrix<T> complete_basis_from_vector(const Matrix<T> &u)
  {
    if (u.is_empty())
    {
      throw std::invalid_argument("\nMatrixAlgLin::complete_basis_from_vector() Requires a single vector.\n");
    }

    if (!u.is_vector())
    {
      throw std::invalid_argument("\nMatrixAlgLin::complete_basis_from_vector() Requires a single vector.\n");
    }

    size_t n = u.numel();
    Matrix<T> Id = MatrixGen::eye<T>(n, n);

    Matrix<T> x = u.flatten().t() / norm(u); // Força a ser vetor coluna

    T norm_x = norm(x, 2.0);
    if (norm_x < 1e-14)
    {
      return Id; // Vetor nulo retorna identidade
    }

    // 1. Cria o vetor de reflexão v = x + sign(x0) * ||x|| * e1
    Matrix<T> v = x;
    // Escolhe o sinal para evitar cancelamento catastrófico
    T sign = (x(0, 0) >= 0) ? 1.0 : -1.0;
    v(0, 0) += sign * norm_x;

    // 2. Calcula H = I - 2 * (v * v^T) / (v^T * v)
    T v_norm_sq = (v.t() * v)(0, 0);
    if (v_norm_sq < 1e-14)
      return Id;

    Matrix<T> H = Id - T(2) * (v * v.t()) / v_norm_sq;

    // Ajuste de sinal para garantir que a 1ª coluna aponte no mesmo sentido de u
    H *= (-sign);
    H >> "MatrixAlgLin::complete_basis_from_vector() Orthogonal basis";
    return H;
  }

  // ==========================================
  // BASE ORTONORMAL (Padrão MATLAB/SciPy)
  // ==========================================
  template <typename T>
  static Matrix<T> orth(const Matrix<T> &A, T tol = 1e-9)
  {
    // 1. Roda o SVD
    auto [U, S, V] = svd(A);

    // 2. Descobre o rank (quantos vetores são linearmente independentes)
    size_t r = 0;
    for (size_t i = 0; i < S.rows(); i++)
    {
      if (S(i, i) > tol)
        r++;
    }

    // Se a matriz for nula, retorna um vetor de zeros
    if (r == 0)
      return Matrix<T>(A.rows(), 1, T(0));

    // 3. Extrai APENAS as colunas ortogonais válidas
    Matrix<T> basis(A.rows(), r);
    size_t col_idx = 0; // Controla em qual coluna da 'basis' estamos a escrever

    for (size_t j = 0; j < S.rows(); j++)
    {
      // Só copia a coluna de U se o valor singular dela sobreviveu à tolerância!
      if (S(j, j) > tol)
      {
        for (size_t i = 0; i < A.rows(); i++)
        {
          basis(i, col_idx) = U(i, j);
        }
        col_idx++;
      }
    }
    basis >> "MatrixLinAlg::orth() Orthogonal basis";
    return basis;
  }

  // ==========================================
  // FATORAÇÃO QR (Via Reflexões de Householder)
  // ==========================================

  // Retorna a tupla {Q, R} onde:
  // Q é uma matriz Ortogonal (Q.t() * Q = Identidade)
  // R é uma matriz Triangular Superior
  template <typename T>
  static std::tuple<Matrix<T>, Matrix<T>> qr(const Matrix<T> &A)
  {
    if (A.is_empty())
    {
      throw std::invalid_argument("\nMatrixLinAlg::qr() Matrix cannot be empty.\n");
    }

    size_t m = A.rows();
    size_t n = A.cols();

    Matrix<T> R = A;
    Matrix<T> Q = MatrixGen::eye<T>(m, m); // Reciclando o seu utilitário brilhante!

    // O limite é o mínimo entre m e n para tratar matrizes retangulares corretamente
    size_t loops = std::min(m, n);

    for (size_t k = 0; k < loops; k++)
    {
      // 1. Extrai o vetor 'x' da coluna 'k', apenas da diagonal para baixo
      size_t sub_len = m - k;
      Matrix<T> x(sub_len, 1);
      for (size_t i = k; i < m; i++)
      {
        x(i - k, 0) = R(i, k);
      }

      T norm_x = norm(x, 2.0);
      if (norm_x < 1e-14)
        continue; // Se a sub-coluna já for zero, pula a iteração

      // 2. Constrói o vetor de Householder 'v'
      Matrix<T> v = x;
      T sign = (x(0, 0) >= 0) ? 1.0 : -1.0;
      v(0, 0) += sign * norm_x;

      T norm_v_sq = (v.t() * v)(0, 0);
      if (norm_v_sq < 1e-14)
        continue;

      // 3. Atualiza a matriz R (aplica a reflexão apenas no bloco inferior direito)
      // Fórmula: R_sub = R_sub - 2 * v * (v^T * R_sub) / (v^T * v)
      for (size_t j = k; j < n; j++)
      {
        T dot = 0.0;
        for (size_t i = k; i < m; i++)
        {
          dot += v(i - k, 0) * R(i, j); // Produto escalar parcial
        }
        dot *= (2.0 / norm_v_sq);
        for (size_t i = k; i < m; i++)
        {
          R(i, j) -= v(i - k, 0) * dot; // Atualiza R in-place
        }
      }

      // Garante zeros cravados abaixo da diagonal por causa de erros de ponto flutuante
      for (size_t i = k + 1; i < m; i++)
      {
        R(i, k) = 0.0;
      }

      // 4. Atualiza a matriz Q (Acumula as reflexões: Q = Q * H_k)
      for (size_t i = 0; i < m; i++)
      {
        T dot = 0.0;
        for (size_t j = k; j < m; j++)
        {
          dot += Q(i, j) * v(j - k, 0);
        }
        dot *= (2.0 / norm_v_sq);
        for (size_t j = k; j < m; j++)
        {
          Q(i, j) -= dot * v(j - k, 0); // Atualiza Q in-place
        }
      }
    }
    Q >> "[Q,R] = MatrixLinAlg::qr(A) - matrix \"Q\" orthogonal in which A = Q * R";
    R >> "[Q,R] = MatrixLinAlg::qr(A) - matrix \"R\" upper triangular in which A = Q * R";
    return {Q, R};
  }

  // ==========================================
  // BASE PARA O NULLSPACE (Núcleo)
  // ==========================================
  template <typename T>
  static Matrix<T> nullspace(const Matrix<T> &A, T tol = 1e-9)
  {
    // 1. Roda o SVD
    auto [U, S, V] = svd(A);

    // 2. Descobre a dimensão do Nullspace (Nulidade)
    // A nulidade é a quantidade de valores singulares que "morreram" (são quase zero)
    size_t nullity = 0;
    for (size_t i = 0; i < S.rows(); i++)
    {
      if (S(i, i) <= tol)
        nullity++;
    }

    // Se a nulidade for zero (matriz de posto completo), o único vetor no núcleo é o vetor nulo.
    // Retornamos uma matriz vazia para indicar que a base é vazia.
    if (nullity == 0)
    {
      Matrix<T> result(0, 0); // Construtor padrão gerando matriz vazia
      result >> "MatrixLinAlg::nullspace(A) - matrix A is full rank, so it has only the trivial zero";
      return result;
    }

    // 3. Extrai APENAS as colunas de V correspondentes aos valores singulares nulos
    Matrix<T> basis(V.rows(), nullity);
    size_t col_idx = 0;

    for (size_t j = 0; j < S.rows(); j++)
    {
      if (S(j, j) <= tol)
      {
        for (size_t i = 0; i < V.rows(); i++)
        {
          basis(i, col_idx) = V(i, j); // Extrai da matriz V (não U!)
        }
        col_idx++;
      }
    }
    basis >> "MatrixLinAlg::nullspace(A) - matrix \"N\" such that A * N = 0";
    return basis;
  }

  // ==========================================
  // AUTOVALORES GERAIS (Algoritmo QR Básico)
  // ==========================================

  // Retorna um vetor coluna com os autovalores reais da matriz A
  template <typename T>
  static Matrix<T> eigvals_general(Matrix<T> A, size_t max_iters = 1000, T tol = 1e-9)
  {
    if (!A.is_square())
    {
      throw std::invalid_argument("\nMatrixAlgLin::eigvals_general() Matrix must be square.\n");
    }

    size_t n = A.rows();

    for (size_t iter = 0; iter < max_iters; iter++)
    {
      // 1. Fatora a matriz atual
      auto [Q, R] = qr(A);

      // 2. Multiplica na ordem inversa
      A = R * Q;

      // 3. Verifica a convergência
      // O algoritmo converge quando a subdiagonal (elementos logo abaixo da diagonal principal) tende a zero
      bool converged = true;
      for (size_t i = 1; i < n; i++)
      {
        if (std::abs(A(i, i - 1)) > tol)
        {
          converged = false;
          break;
        }
      }
      if (converged)
        break;
    }

    // Extrai a diagonal principal (os autovalores)
    Matrix<T> eigenvalues = A.diag();
    eigenvalues >> "MatrixLinAlg::eigvals_general(A) - REAL EIGVALS ONLY! BE CAREFUL";

    return eigenvalues;
  }
};
