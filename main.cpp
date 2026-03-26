#include <iostream>
#include "Matrix.hpp"
#include "MatrixShape.hpp"
#include "MatrixGen.hpp"
#include "MatrixLinAlg.hpp"
// #include <cmath>

int main()
{
    try
    {

        using alg = MatrixLinAlg;
        using shape = MatrixShape;
        using gen = MatrixGen;
        using mx = Matrix<double>;
        using mxi = Matrix<int>;
        using mxb = Matrixbool;

        // Matriz Assimétrica (Autovalores conhecidos: 3, 2, 1)
        mx A = {{7, 0, 0, 9},
                {-1, 0, 9, 11},
                {-2, 2, 2, 1},
                {0, 1, 17, -1}};

        A | 0;
        A.debug(1);
        A >> "Matrix A";
        A.print();

        mxi B = A;
        B.debug(1);
        B >> "Matrix B";
        B.print();

        mxb C = A;
        C.debug(1);
        C >> "Matrix C";
        C.print();
        mxb D = 1 - (mxb)(7 - A);
        D >> "Matrix D";
        D.print();
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << "\n";
    }
    return j5r(100);
}