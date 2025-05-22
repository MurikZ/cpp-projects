@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo === Пример 1: Суммирование чисел ===
build\bin\labwork5.exe 23 43 76 9 1 217 4 --sum
echo.

echo === Пример 2: Умножение чисел ===
build\bin\labwork5.exe 7 6 4 9 3 --mult
echo.

pause
