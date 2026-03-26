#include <iostream>
#include "Matrix.hpp"
#include "MatrixShape.hpp"
#include "MatrixGen.hpp"
#include "MatrixLinAlg.hpp"
//#include <cmath>

int main()
{
    try
    {

        using alg = MatrixLinAlg;
        using shape = MatrixShape;
        using gen = MatrixGen;
        using mx = Matrix<float>;
        using mxi = Matrix<int>;
        mx::set_print_precision(4);
        mx::set_print_width(4);

        

// Matriz genérica retangular (4x3) para provarmos que funciona em qualquer dimensão!
mx A = {{12, -51, 4},
        { 6, 167, -68},
        {-4,  24, -41},
        {-1,   1,   0}};

A.msg("matrix A").print();

auto [Q, R] = alg::qr(A);

Q.print();
R.print();

// Prova 1: Reconstrução
mx A_rec = Q * R;
A_rec.msg("Reconstrucao (Q * R) -> Deve ser igual a A").print();

auto res = A - A_rec; strcc<< "residuo, norma = " << alg::norm(res);
strcc(res).print();

// Prova 2: Ortogonalidade
mx I_prova = Q.t() * Q;
I_prova.msg("Prova de Ortogonalidade (Q^T * Q) -> Deve ser a Identidade").print();
        













    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << "\n";
    }
    return j5r(100);
}