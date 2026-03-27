:: Removi o -march=native temporariamente (já explico o porquê)
g++ main.cpp -o main.exe -std=c++2c -ffast-math -march=native

:: O cmd /k executa o programa e RECUSA-SE a fechar a janela, aconteça o que acontecer!
:: cmd /k main.exe
start main.exe
cmd /c timeout 100