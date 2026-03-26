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
        using mx = Matrix<double>;
        using mxi = Matrix<int>;
        mx::set_print_precision(4);
        mx::set_print_width(4);

        



// Matriz Assimétrica (Autovalores conhecidos: 3, 2, 1)
mx A = {{ 7,  2,  0},
        {-1,  4,  9},
        { -2,  2,  2}};

A.print();



mx autovalores = alg::eigvals_general(A);
autovalores.print();
        













    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << "\n";
    }
    return j5r(100);
}