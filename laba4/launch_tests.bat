@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo === Тесты программы ===
build\tests\argparser_tests.exe

echo.
pause