@echo off
chcp 65001
cls

echo ========================================
echo    Caesar Cipher Test Suite
echo ========================================
echo.

echo 1. Compiling program...
g++ -std=c++17 -o analyzer.exe main.cpp CaesarCipher_w.cpp GraphAnalyzer_w.cpp

if not exist analyzer.exe (
    echo Compilation failed!
    pause
    exit /b 1
)

echo 2. Running tests...
echo.
echo Test output will be saved to test_output.txt
analyzer.exe < input.txt > test_output.txt 2>&1

echo 3. Checking results...
if exist test_output.txt (
    echo Test output file created successfully.
    echo.
    echo === LAST 10 LINES OF OUTPUT ===
    tail -10 test_output.txt
) else (
    echo ERROR: No output file generated!
)

echo.
echo 4. Opening report...
python open_report.py

echo.
echo Test completed!
echo Press any key to exit...
pause > nul