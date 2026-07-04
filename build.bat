@echo off
chcp 65001 >nul
echo ========================================
echo   α-Redis - Compilation Windows
echo ========================================
echo.

REM Créer les répertoires
if not exist obj mkdir obj
if not exist bin mkdir bin

echo [1/6] Compilation de linked_list.c...
gcc -Wall -Wextra -std=c99 -O2 -c src/linked_list.c -o obj/linked_list.o
if errorlevel 1 (
    echo ERREUR: linked_list.c
    pause
    exit /b 1
)

echo [2/6] Compilation de hash_table.c...
gcc -Wall -Wextra -std=c99 -O2 -c src/hash_table.c -o obj/hash_table.o
if errorlevel 1 (
    echo ERREUR: hash_table.c
    pause
    exit /b 1
)

echo [3/6] Compilation de engine.c...
gcc -Wall -Wextra -std=c99 -O2 -c src/engine.c -o obj/engine.o
if errorlevel 1 (
    echo ERREUR: engine.c
    pause
    exit /b 1
)

echo [4/6] Compilation de web_server.c...
gcc -Wall -Wextra -std=c99 -O2 -c web_server.c -o obj/web_server.o
if errorlevel 1 (
    echo ERREUR: web_server.c
    pause
    exit /b 1
)

echo [5/6] Compilation de main.c...
gcc -Wall -Wextra -std=c99 -O2 -c src/main.c -o obj/main.o
if errorlevel 1 (
    echo ERREUR: main.c
    pause
    exit /b 1
)

echo [6/6] Edition des liens...
gcc obj/linked_list.o obj/hash_table.o obj/engine.o obj/web_server.o obj/main.o -o bin/aredis.exe -lws2_32
if errorlevel 1 (
    echo ERREUR: Edition des liens
    pause
    exit /b 1
)

echo.
echo ========================================
echo   ✓ Compilation reussie!
echo ========================================
echo.
echo Pour executer: bin\aredis.exe
echo.
pause