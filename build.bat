@echo off
echo Compilando SQLite...
gcc -c -o sqlite3.o sqlite3/include/sqlite3.c -I./sqlite3/include

echo Compilando o sistema bancario...
g++ -o bank_system_new.exe main.cpp database/DatabaseManager.cpp models/Company.cpp advanced_features.cpp sqlite3.o -I./sqlite3/include
if %errorlevel% equ 0 (
    echo Compilacao concluida com sucesso!
    echo Para executar, use: .\bank_system_new.exe
) else (
    echo Erro na compilacao!
)
pause 