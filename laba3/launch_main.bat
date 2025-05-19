@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

cd /d %~dp0

echo === Запуск приложения ===

build\bin\sandpile_app.exe ^
  --length 10 --width 10 ^
  --input input\main_input.tsv ^
  --output output ^
  --max-iter 100 ^
  --freq 10

pause
