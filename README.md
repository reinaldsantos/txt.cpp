# Sistema de Gerenciamento de Empréstimos

Sistema simples para gerenciamento de empréstimos desenvolvido em C++ utilizando SQLite como banco de dados.

## Funcionalidades

- Visualização do histórico de empréstimos
- Adição de novos empréstimos
- Registro de atividades em log
- Interface em console com suporte a UTF-8

## Requisitos

- Sistema Operacional: Windows
- Compilador: MinGW (GCC)
- CMake (versão 3.10 ou superior)
- SQLite3

## Estrutura do Projeto

```
├── main.cpp              # Código principal do sistema
├── view_data.cpp         # Visualizador de dados
├── CMakeLists.txt        # Configuração de compilação
├── build.bat            # Script de compilação
├── bank.bat             # Atalho para o sistema bancário
├── view.bat             # Atalho para o visualizador
├── database/            # Código do banco de dados
├── models/              # Classes do modelo
└── sqlite3/             # Biblioteca SQLite
```

## Como Compilar

1. Certifique-se de ter o MinGW instalado
2. Execute o script de compilação:
   ```
   .\build.bat
   ```

## Como Usar

1. Para adicionar empréstimos:
   ```
   .\bank.bat
   ```

2. Para visualizar registros:
   ```
   .\view.bat
   ```

## Formato dos Dados

Os empréstimos são registrados com:
- Nome da empresa
- Local
- Funcionário responsável
- Valor (em euros)
- Status (Aprovado/Reprovado)

## Licença

Este projeto está sob a licença MIT. Veja o arquivo LICENSE para mais detalhes. 