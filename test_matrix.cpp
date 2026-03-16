// Isso diz ao doctest para criar a função main() automaticamente para os testes
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "Matrix.hpp"

// Um bloco de teste lógico
TEST_CASE("Testando os construtores da classe Matrix")
{

    SUBCASE("Construtor padrao deve iniciar com zero linhas e colunas")
    {
        Matrix<int> m;

        // CHECK avalia se a condição é verdadeira. Se for falsa, o teste falha.
        CHECK(m.rows() == 0);
        CHECK(m.cols() == 0);
    }

    SUBCASE("Construtor com dimensoes e valor inicial")
    {
        Matrix<double> m(2, 2, -1.5);

        CHECK(m.rows() == 2);
        CHECK(m.cols() == 2);

        // Verificando se os valores foram preenchidos corretamente
        CHECK(m.get(0, 0) == -1.5);
        CHECK(m.get(1, 1) == -1.5);
    }
}

TEST_CASE("Testando o metodo de impressao da classe Matrix")
{

    SUBCASE("Impressao de matriz 3x3")
    {
        Matrix<int> m(3, 3);

        // Preenchendo a matriz com valores de exemplo
        for (size_t i = 0; i < m.rows(); ++i)
        {
            for (size_t j = 0; j < m.cols(); ++j)
            {
                m(i, j) = i * m.cols() + j + 1; // Valores de 1 a 9
            }
        }

        // Imprimindo a matriz para verificar visualmente
        m.print();
    }

    SUBCASE("Impressao de matriz grande com truncamento")
    {
        Matrix<int> m(12, 10);

        // Preenchendo a matriz com valores de exemplo
        for (size_t i = 0; i < m.rows(); ++i)
        {
            for (size_t j = 0; j < m.cols(); ++j)
            {
                m(i, j) = i * m.cols() + j + 1; // Valores sequenciais
            }
        }

        // Imprimindo a matriz para verificar o truncamento visual
        m.print();
    }

    SUBCASE("Impressao com largura personalizada")
    {
        Matrix<double> m(2, 2, 3.14159);

        // Imprimindo a matriz com largura de 10 caracteres
        m.print(10);
    }


    SUBCASE("Impressao com largura menor que o minimo deve lancar erro")
    {
        Matrix<double> m(2, 5, -128.54397856435);

        // Verificando se a função lança uma exceção quando a largura é menor que 5
        for(int width = 0; width < 5; ++width) {
            std::cout << "Testing print with width = " << width << "... (error)\n";
            CHECK_THROWS_AS(m.print(width), std::runtime_error);
        }

        for(int width = 5; width < 15; ++width) {
            std::cout << "Testing print with width = " << width << "... (not error)\n";
            CHECK_NOTHROW(m.print(width));
        }

    }

    SUBCASE("Impressao com cor personalizada")
    {
        Matrix<int> m(2, 2, 7);

        // Imprimindo a matriz com cor vermelha (color_int = 1)
        for(int color = 1; color <= 6; ++color) {
            m.print(9, color);
        }
    }
}
