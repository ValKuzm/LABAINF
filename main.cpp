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

// Функция для чтения тестов из файла
void RunTestsFromFile(const std::string& filename) {
    std::wifstream file(filename);
    if (!file.is_open()) {
        std::wcout << L"Error: Cannot open file " << std::wstring(filename.begin(), filename.end()) << L"\n";
        return;
    }

    std::wstring line;
    int test_count = 0;
    int passed_count = 0;

    std::wcout << L"\nRunning tests from file...\n";
    std::wcout << L"===========================\n";

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == L'#') continue;

        std::wstringstream ss(line);
        std::wstring text;
        std::wstring shift_str;
        std::wstring expected_lang;

        if (std::getline(ss, text, L'|') &&
            std::getline(ss, shift_str, L'|') &&
            std::getline(ss, expected_lang)) {

            int shift = std::stoi(shift_str);
            test_count++;

            CaesarCipherW cipher(shift);
            std::string detected_lang = cipher.DetectLanguage(text);
            std::string expected(expected_lang.begin(), expected_lang.end());

            std::wcout << L"\nTest #" << test_count << L":\n";
            std::wcout << L"Text: " << (text.length() > 30 ? text.substr(0, 30) + L"..." : text) << L"\n";
            std::wcout << L"Shift: " << shift << L"\n";
            std::wcout << L"Expected language: " << expected_lang << L"\n";
            std::wcout << L"Detected language: " << std::wstring(detected_lang.begin(), detected_lang.end()) << L"\n";

            if (detected_lang == expected) {
                std::wcout << L"✓ PASSED\n";
                passed_count++;
            }
            else {
                std::wcout << L"✗ FAILED\n";
            }

            // Дешифруем и показываем результат
            std::wstring decrypted = cipher.SmartDecryptW(text);
            std::wcout << L"Decrypted: " << (decrypted.length() > 50 ? decrypted.substr(0, 50) + L"..." : decrypted) << L"\n";
        }
    }

    std::wcout << L"\n===========================\n";
    std::wcout << L"RESULTS: " << passed_count << L"/" << test_count << L" tests passed\n";
    if (test_count > 0) {
        double percentage = (passed_count * 100.0) / test_count;
        std::wcout << L"Success rate: " << std::fixed << std::setprecision(1) << percentage << L"%\n";
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