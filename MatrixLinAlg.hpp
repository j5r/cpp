#pragma once
#include "Matrix.hpp"
#include "MatrixShape.hpp"

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
    static Matrix<T> gmres(const Matrix<T>& A, const Matrix<T>& b, size_t m = 50, T tol = 1e-6) 
    {
        if (!A.is_square() || A.rows() != b.rows()) {
            throw std::invalid_argument("\nMatrixAlgLin::gmres() Dimension mismatch.\n");
        }

        size_t n = b.rows();
        Matrix<T> x(n, 1); // Chute inicial x0 = vetor de zeros
        Matrix<T> r = b - (A * x);
        
        T b_norm = std::sqrt((b.t() * b)(0, 0));
        if (b_norm == 0.0) return x; // O sistema já está resolvido

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
        for(size_t i = 0; i < n; i++) {
            V(i, 0) = r(i, 0) / r_norm;
        }

        size_t k = 0;
        for (; k < m; k++) 
        {
            // --- 1. PROCESSO DE ARNOLDI (Ortogonalização de Gram-Schmidt) ---
            Matrix<T> w = A * V.getcol(k); // Pega a coluna k e multiplica por A

            for (size_t i = 0; i <= k; i++) {
                Matrix<T> vi = V.getcol(i);
                H(i, k) = (vi.t() * w)(0, 0);  // Produto escalar
                w = w - (vi * H(i, k));        // Subtração de projeções
            }

            H(k + 1, k) = std::sqrt((w.t() * w)(0, 0)); // Norma do vetor ortogonalizado
            
            // Se a norma não for zero (evita divisão por zero)
            if (H(k + 1, k) > 1e-14) {
                for(size_t i = 0; i < n; i++) {
                    V(i, k + 1) = w(i, 0) / H(k + 1, k);
                }
            }

            // --- 2. ROTAÇÕES DE GIVENS (Transforma H numa matriz triangular) ---
            // Aplica as rotações antigas à nova coluna gerada
            for (size_t i = 0; i < k; i++) {
                T temp = cs[i] * H(i, k) + sn[i] * H(i + 1, k);
                H(i + 1, k) = -sn[i] * H(i, k) + cs[i] * H(i + 1, k);
                H(i, k) = temp;
            }

            // Calcula o novo ângulo (seno e cosseno) para zerar o elemento H(k+1, k)
            T beta = std::sqrt(H(k, k) * H(k, k) + H(k + 1, k) * H(k + 1, k));
            if (beta == 0.0) {
                cs[k] = 1.0; 
                sn[k] = 0.0;
            } else {
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
            if (error < tol) {
                k++; // Ajusta k para o tamanho real do subespaço construído
                break;
            }
        }

        // --- 4. RETRO-SUBSTITUIÇÃO (Resolve Hy = g) ---
        // O sistema foi reduzido de NxN para kxk !
        Matrix<T> y(k, 1);
        // Atenção: usando int aqui pois size_t em ordem decrescente causaria underflow se chegar a -1
        for (int i = k - 1; i >= 0; i--) { 
            y(i, 0) = g(i, 0);
            for (int j = i + 1; j < (int)k; j++) {
                y(i, 0) -= H(i, j) * y(j, 0);
            }
            y(i, 0) /= H(i, i);
        }

        // --- 5. RECUPERAÇÃO DA SOLUÇÃO (x = x0 + Vy) ---
        for (size_t i = 0; i < k; i++) {
            x = x + (V.getcol(i) * y(i, 0));
        }

        return x;
    }
};
