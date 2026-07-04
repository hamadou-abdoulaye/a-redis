@echo off
chcp 65001 >nul

REM Vérifier si l'exécutable existe
if not exist bin\aredis.exe (
    echo.
    echo ╔════════════════════════════════════════════════════════════════════╗
    echo ║                                                                    ║
    echo ║          \033[1;36mα-Redis\033[0m - Compilation necessaire                    ║
    echo ║                                                                    ║
    echo ╚════════════════════════════════════════════════════════════════════╝
    echo.
    echo L'executable n'existe pas. Compilation en cours...
    echo.
    call build.bat
    echo.
)

REM Lancer le programme
bin\aredis.exe