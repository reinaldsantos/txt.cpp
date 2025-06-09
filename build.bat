@echo off
echo Limpando build anterior...
if exist build rmdir /s /q build
mkdir build
cd build

echo Configurando CMake...
cmake -G "MinGW Makefiles" ..

echo Compilando...
cmake --build .

if %ERRORLEVEL% EQU 0 (
    echo Compilacao concluida com sucesso!
    echo Copiando executaveis para a pasta principal...
    copy /Y bank_system.exe ..
    copy /Y view_data.exe ..
) else (
    echo Erro na compilacao!
    exit /b %ERRORLEVEL%
)

cd ..

echo.
echo Para executar o sistema bancario:
echo bank_system.exe
echo.
echo Para visualizar os registros:
echo view_data.exe
echo. 