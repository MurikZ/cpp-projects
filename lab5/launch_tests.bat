@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo === Тесты программы ===
build\tests\parser_tests.exe

echo.
pause