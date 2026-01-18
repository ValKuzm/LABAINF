#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "CaesarCipher_w.h"
#include "GraphAnalyzer_w.h"
#include <locale>
#include <codecvt>

// Настройка консоли
static void SetupConsoleUTF16() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
}

void PrintHeader() {
    std::wcout << L"========================================\n";
    std::wcout << L"     AUTOMATED CAESAR CIPHER ANALYZER   \n";
    std::wcout << L"========================================\n\n";
}
void RunTestsFromFile(const std::string& filename) {
    // Открываем файл как бинарный
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::wcout << L"Error: Cannot open file " << std::wstring(filename.begin(), filename.end()) << L"\n";
        return;
    }

    // Читаем весь файл
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Конвертируем UTF-8 в UTF-16
    std::wstring wcontent;
    int size = MultiByteToWideChar(CP_UTF8, 0, content.c_str(), (int)content.size(), NULL, 0);
    if (size > 0) {
        wcontent.resize(size);
        MultiByteToWideChar(CP_UTF8, 0, content.c_str(), (int)content.size(), &wcontent[0], size);
    }
    else {
        // Если не UTF-8, пробуем системную кодовую страницу
        size = MultiByteToWideChar(CP_ACP, 0, content.c_str(), (int)content.size(), NULL, 0);
        if (size > 0) {
            wcontent.resize(size);
            MultiByteToWideChar(CP_ACP, 0, content.c_str(), (int)content.size(), &wcontent[0], size);
        }
    }

    std::wstringstream wss(wcontent);
    std::wstring line;
    int test_count = 0;
    int passed_lang_count = 0;
    int passed_decrypt_count = 0;

    std::wcout << L"\nRunning tests from file...\n";
    std::wcout << L"===========================\n";

    while (std::getline(wss, line)) {
        // Удаляем \r если есть
        if (!line.empty() && line.back() == L'\r') {
            line.pop_back();
        }

        if (line.empty() || line[0] == L'#') continue;

        // Разделяем строку
        size_t pipe1 = line.find(L'|');
        size_t pipe2 = line.find(L'|', pipe1 + 1);
        size_t pipe3 = line.find(L'|', pipe2 + 1);

        if (pipe1 == std::wstring::npos || pipe2 == std::wstring::npos || pipe3 == std::wstring::npos) {
            std::wcout << L"WARNING: Invalid format in line: " << line << L"\n";
            continue;
        }

        std::wstring text = line.substr(0, pipe1);
        std::wstring shift_str = line.substr(pipe1 + 1, pipe2 - pipe1 - 1);
        std::wstring expected_lang = line.substr(pipe2 + 1, pipe3 - pipe2 - 1);
        std::wstring expected_decrypted = line.substr(pipe3 + 1);

        // Удаляем пробелы
        auto trim = [](std::wstring& s) {
            size_t start = s.find_first_not_of(L" \t");
            if (start == std::wstring::npos) {
                s.clear();
                return;
            }
            size_t end = s.find_last_not_of(L" \t");
            s = s.substr(start, end - start + 1);
            };

        trim(text);
        trim(shift_str);
        trim(expected_lang);
        trim(expected_decrypted);

        try {
            int shift = std::stoi(shift_str);
            test_count++;

            CaesarCipherW cipher(shift);
            std::string detected_lang = cipher.DetectLanguage(text);
            std::string expected_lang_str(expected_lang.begin(), expected_lang.end());

            std::wstring decrypted = cipher.SmartDecryptW(text);

            std::wcout << L"\nTest #" << test_count << L":\n";
            std::wcout << L"Text: " << text << L"\n";
            std::wcout << L"Shift: " << shift << L"\n";
            std::wcout << L"Expected language: " << expected_lang << L"\n";
            std::wcout << L"Detected language: " << std::wstring(detected_lang.begin(), detected_lang.end()) << L"\n";
            std::wcout << L"Expected decrypted: " << expected_decrypted << L"\n";
            std::wcout << L"Actual decrypted: " << decrypted << L"\n";

            // Проверяем детекцию языка
            bool lang_correct = (detected_lang == expected_lang_str);
            if (lang_correct) {
                std::wcout << L"✓ Language PASSED\n";
                passed_lang_count++;
            }
            else {
                std::wcout << L"✗ Language FAILED\n";
            }

            // Проверяем дешифровку
            bool decrypt_correct = (decrypted == expected_decrypted);
            if (decrypt_correct) {
                std::wcout << L"✓ Decryption PASSED\n";
                passed_decrypt_count++;
            }
            else {
                std::wcout << L"✗ Decryption FAILED\n";
            }

            if (lang_correct && decrypt_correct) {
                std::wcout << L"★ FULL TEST PASSED\n";
            }
        }
        catch (const std::exception& e) {
            std::wcout << L"ERROR processing test: " << e.what() << L"\n";
        }
        catch (...) {
            std::wcout << L"ERROR processing test\n";
        }
    }

    std::wcout << L"\n===========================\n";
    std::wcout << L"SUMMARY:\n";
    std::wcout << L"Total tests: " << test_count << L"\n";
    std::wcout << L"Language detection passed: " << passed_lang_count << L"/" << test_count << L"\n";
    std::wcout << L"Decryption passed: " << passed_decrypt_count << L"/" << test_count << L"\n";

    if (test_count > 0) {
        double lang_percentage = (passed_lang_count * 100.0) / test_count;
        double decrypt_percentage = (passed_decrypt_count * 100.0) / test_count;

        std::wcout << L"\nSuccess rates:\n";
        std::wcout << L"Language detection: " << std::fixed << std::setprecision(1) << lang_percentage << L"%\n";
        std::wcout << L"Decryption: " << std::fixed << std::setprecision(1) << decrypt_percentage << L"%\n";

        int full_passed = 0;
        for (int i = 0; i < test_count; i++) {
            // Здесь нужно отслеживать, какие тесты прошли полностью
            // Для простоты можно считать, что тест прошел полностью, если прошли обе проверки
            // В реальном коде нужно сохранять результаты каждого теста
        }
    }
}

void PrintGraphAnalysis(const CaesarCipherW& cipher, const std::wstring& text) {
    std::wcout << L"\nGRAPH ANALYSIS RESULTS:\n";
    std::wcout << L"========================\n";

    // Получаем SCC профиль
    std::vector<int> scc_profile = cipher.GetSCCProfile(text);

    // Получаем анализ графа
    std::string graph_json = cipher.GetGraphAnalysisJSON(text);

    // Эталонные SCC профили
    std::map<std::string, std::vector<int>> referenceSCCProfiles = {
        {"ENGLISH", {5, 3, 2, 2, 1, 1, 1, 1}},
        {"RUSSIAN", {6, 4, 3, 2, 2, 1, 1}},
        {"GERMAN", {3, 3, 3, 2, 2, 1}},
        {"FRENCH", {7, 4, 2, 1, 1}}
    };

    // Сравниваем с эталонами
    std::vector<std::pair<double, std::string>> sccScores;
    for (const auto& ref : referenceSCCProfiles) {
        double score = GraphAnalyzer::compareSCCProfiles(scc_profile, ref.second);
        sccScores.push_back(std::make_pair(score, ref.first));
    }

    std::sort(sccScores.begin(), sccScores.end(),
        [](const std::pair<double, std::string>& a, const std::pair<double, std::string>& b) {
            return a.first > b.first;
        });

    // Выводим результаты
    std::wcout << L"• Vertices (letters analyzed): " << scc_profile.size() << L"\n";
    std::wcout << L"• Strongly Connected Components: " << scc_profile.size() << L"\n";

    std::wcout << L"• SCC Profile (component sizes): [";
    for (size_t i = 0; i < scc_profile.size(); ++i) {
        std::wcout << scc_profile[i];
        if (i < scc_profile.size() - 1) std::wcout << L", ";
    }
    std::wcout << L"]\n";

    std::wcout << L"• Language similarity based on SCC structure:\n";
    for (size_t i = 0; i < sccScores.size(); ++i) {
        const std::pair<double, std::string>& score_pair = sccScores[i];
        double percentage = score_pair.first * 100;
        std::wcout << L"  - " << std::wstring(score_pair.second.begin(), score_pair.second.end())
            << L": " << std::fixed << std::setprecision(1) << percentage << L"%\n";
    }

    // Определяем язык по графу
    std::string graph_lang = sccScores[0].first > 0.3 ? sccScores[0].second : "UNKNOWN";
    std::wcout << L"• Graph-based language detection: "
        << std::wstring(graph_lang.begin(), graph_lang.end()) << L"\n";
}

int main() {
    SetupConsoleUTF16();
    PrintHeader();
    setlocale(LC_ALL, "Russian");

    while (true) {
        std::wcout << L"\nChoose mode:\n";
        std::wcout << L"1. Interactive input\n";
        std::wcout << L"2. Run test cases from file\n";
        std::wcout << L"3. Generate sample analysis\n";
        std::wcout << L"Choice: ";

        int mode;
        std::wcin >> mode;
        std::wcin.ignore(10000, L'\n');

        if (mode == 1) {
            std::wcout << L"\nEnter encrypted text: ";
            std::wstring encrypted;
            std::getline(std::wcin, encrypted);

            std::wcout << L"Enter shift: ";
            int shift;
            std::wcin >> shift;
            std::wcin.ignore(10000, L'\n');

            std::wcout << L"Enter report filename (without .html, press Enter for default): ";
            std::wstring filename_w;
            std::getline(std::wcin, filename_w);

            CaesarCipherW cipher(shift);

            // Анализ языка
            std::string detected_lang = cipher.AnalyzeLanguage(encrypted);

            // Анализ графа
            PrintGraphAnalysis(cipher, encrypted);

            // Умная дешифровка
            std::wstring decrypted = cipher.SmartDecryptW(encrypted);

            // Создание отчета
            std::string filename = "analysis_report.html";
            cipher.CreateCompleteAnalysisHTML(encrypted, decrypted, detected_lang, filename);

            std::wcout << L"\nAnalysis complete! Report saved to "
                << std::wstring(filename.begin(), filename.end()) << L"\n";
            std::wcout << L"Opening report in browser...\n";

            // Пауза
            std::wcout << L"\nPress Enter to continue...";
            std::wcin.get();

        }
        else if (mode == 2) {
            std::wcout << L"\nEnter test file name: ";
            std::wstring filename_w;
            std::getline(std::wcin, filename_w);

            std::string filename(filename_w.begin(), filename_w.end());
            RunTestsFromFile(filename);

        }
        else if (mode == 3) {
            std::wcout << L"\nGenerating sample analysis...\n";

            std::wstring sample_text = L"QrRanjuuhfjwccqrbcngccxknmnlryqnanmlxaanlcuhjwmRuuknenahqjyyhrorcqjyynwb";
            int sample_shift = 9;

            CaesarCipherW cipher(sample_shift);
            std::string lang = cipher.AnalyzeLanguage(sample_text);

            PrintGraphAnalysis(cipher, sample_text);

            std::wstring decrypted = cipher.SmartDecryptW(sample_text);

            std::string filename = "sample_analysis.html";
            cipher.CreateCompleteAnalysisHTML(sample_text, decrypted, lang, filename);

            std::wcout << L"Sample analysis complete! Report saved to sample_analysis.html\n";

        }
        else {
            std::wcout << L"Invalid choice!\n";
        }

        std::wcout << L"\nRun another analysis? (y/n): ";
        std::wstring choice;
        std::getline(std::wcin, choice);

        if (choice == L"n" || choice == L"N") {
            std::wcout << L"\nGoodbye!\n";
            break;
        }
    }

    return 0;
}
