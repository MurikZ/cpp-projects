@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

cd /d %~dp0

echo === Запуск тестов ===
build\tests\sandpile_tests.exe

pause
