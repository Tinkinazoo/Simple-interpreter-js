JavaScript interpretator. First version.

Project structure:

```
interpreter/
│
├── src/                    # Исходные файлы
│   ├── main.cpp           # Точка входа (main)
│   ├── lexer.h            # Объявление лексера
│   ├── lexer.cpp          # Реализация лексера
│   ├── token.h            # Определение токенов
│   ├── parser.h           # Объявление парсера
│   ├── parser.cpp         # Реализация парсера
│   ├── ast.h              # Определение AST (абстрактного синтаксического дерева)
│   ├── interpreter.h      # Объявление интерпретатора
│   ├── interpreter.cpp    # Реализация интерпретатора
│   ├── environment.h      # Объявление окружения (таблицы символов)
│   └── environment.cpp    # Реализация окружения
│
├── include/               # Заголовочные файлы для внешнего использования
│
├── test/                  # Тесты
│
├── test_programs/         # Примеры программ для интерпретатора
│
└── CMakeLists.txt         # Файл сборки CMake
```
