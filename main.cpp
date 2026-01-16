//#include <windows.h>
//#include <fcntl.h>
//#include <io.h>
//#include <iostream>
//#include <string>
//#include <cstdlib>
//#include <vector>
//#include <algorithm>
//#include <iomanip>
//#include <fstream>
//#include <sstream>
//#include "CaesarCipher_w.h"
//#include "GraphAnalyzer_w.h"
//
//// Настройка консоли
//static void SetupConsoleUTF16() {
//    SetConsoleOutputCP(65001);
//    SetConsoleCP(65001);
//    _setmode(_fileno(stdout), _O_U16TEXT);
//    _setmode(_fileno(stdin), _O_U16TEXT);
//}
//
//void PrintHeader() {
//    std::wcout << L"========================================\n";
//    std::wcout << L"     AUTOMATED CAESAR CIPHER ANALYZER   \n";
//    std::wcout << L"========================================\n\n";
//}
//
//// Функция для чтения тестов из файла
//void RunTestsFromFile(const std::string& filename) {
//    std::wifstream file(filename);
//    if (!file.is_open()) {
//        std::wcout << L"Error: Cannot open file " << std::wstring(filename.begin(), filename.end()) << L"\n";
//        return;
//    }
//
//    std::wstring line;
//    int test_count = 0;
//    int passed_count = 0;
//
//    std::wcout << L"\nRunning tests from file...\n";
//    std::wcout << L"===========================\n";
//
//    while (std::getline(file, line)) {
//        if (line.empty() || line[0] == L'#') continue;
//
//        std::wstringstream ss(line);
//        std::wstring text;
//        std::wstring shift_str;
//        std::wstring expected_lang;
//
//        if (std::getline(ss, text, L'|') &&
//            std::getline(ss, shift_str, L'|') &&
//            std::getline(ss, expected_lang)) {
//
//            int shift = std::stoi(shift_str);
//            test_count++;
//
//            CaesarCipherW cipher(shift);
//            std::string detected_lang = cipher.DetectLanguage(text);
//            std::string expected(expected_lang.begin(), expected_lang.end());
//
//            std::wcout << L"\nTest #" << test_count << L":\n";
//            std::wcout << L"Text: " << (text.length() > 30 ? text.substr(0, 30) + L"..." : text) << L"\n";
//            std::wcout << L"Shift: " << shift << L"\n";
//            std::wcout << L"Expected language: " << expected_lang << L"\n";
//            std::wcout << L"Detected language: " << std::wstring(detected_lang.begin(), detected_lang.end()) << L"\n";
//
//            if (detected_lang == expected) {
//                std::wcout << L"✓ PASSED\n";
//                passed_count++;
//            }
//            else {
//                std::wcout << L"✗ FAILED\n";
//            }
//
//            // Дешифруем и показываем результат
//            std::wstring decrypted = cipher.SmartDecryptW(text);
//            std::wcout << L"Decrypted: " << (decrypted.length() > 50 ? decrypted.substr(0, 50) + L"..." : decrypted) << L"\n";
//        }
//    }
//
//    std::wcout << L"\n===========================\n";
//    std::wcout << L"RESULTS: " << passed_count << L"/" << test_count << L" tests passed\n";
//    if (test_count > 0) {
//        double percentage = (passed_count * 100.0) / test_count;
//        std::wcout << L"Success rate: " << std::fixed << std::setprecision(1) << percentage << L"%\n";
//    }
//}
//
//void PrintGraphAnalysis(const CaesarCipherW& cipher, const std::wstring& text) {
//    std::wcout << L"\nGRAPH ANALYSIS RESULTS:\n";
//    std::wcout << L"========================\n";
//
//    // Получаем SCC профиль
//    std::vector<int> scc_profile = cipher.GetSCCProfile(text);
//
//    // Получаем анализ графа
//    std::string graph_json = cipher.GetGraphAnalysisJSON(text);
//
//    // Эталонные SCC профили
//    std::map<std::string, std::vector<int>> referenceSCCProfiles = {
//        {"ENGLISH", {5, 3, 2, 2, 1, 1, 1, 1}},
//        {"RUSSIAN", {6, 4, 3, 2, 2, 1, 1}},
//        {"GERMAN", {3, 3, 3, 2, 2, 1}},
//        {"FRENCH", {7, 4, 2, 1, 1}}
//    };
//
//    // Сравниваем с эталонами
//    std::vector<std::pair<double, std::string>> sccScores;
//    for (const auto& ref : referenceSCCProfiles) {
//        double score = GraphAnalyzer::compareSCCProfiles(scc_profile, ref.second);
//        sccScores.push_back(std::make_pair(score, ref.first));
//    }
//
//    std::sort(sccScores.begin(), sccScores.end(),
//        [](const std::pair<double, std::string>& a, const std::pair<double, std::string>& b) {
//            return a.first > b.first;
//        });
//
//    // Выводим результаты
//    std::wcout << L"• Vertices (letters analyzed): " << scc_profile.size() << L"\n";
//    std::wcout << L"• Strongly Connected Components: " << scc_profile.size() << L"\n";
//
//    std::wcout << L"• SCC Profile (component sizes): [";
//    for (size_t i = 0; i < scc_profile.size(); ++i) {
//        std::wcout << scc_profile[i];
//        if (i < scc_profile.size() - 1) std::wcout << L", ";
//    }
//    std::wcout << L"]\n";
//
//    std::wcout << L"• Language similarity based on SCC structure:\n";
//    for (size_t i = 0; i < sccScores.size(); ++i) {
//        const std::pair<double, std::string>& score_pair = sccScores[i];
//        double percentage = score_pair.first * 100;
//        std::wcout << L"  - " << std::wstring(score_pair.second.begin(), score_pair.second.end())
//            << L": " << std::fixed << std::setprecision(1) << percentage << L"%\n";
//    }
//
//    // Определяем язык по графу
//    std::string graph_lang = sccScores[0].first > 0.3 ? sccScores[0].second : "UNKNOWN";
//    std::wcout << L"• Graph-based language detection: "
//        << std::wstring(graph_lang.begin(), graph_lang.end()) << L"\n";
//}
//
//// Функция для получения названия файла от пользователя
//std::string GetFilenameFromUser(const std::wstring& prompt, const std::string& default_name) {
//    std::wcout << prompt;
//    std::wstring filename_w;
//    std::getline(std::wcin, filename_w);
//
//    // Если пользователь просто нажал Enter, используем имя по умолчанию
//    if (filename_w.empty()) {
//        return default_name;
//    }
//
//    // Преобразуем в UTF-8
//    std::string filename(filename_w.begin(), filename_w.end());
//
//    // Убеждаемся, что у файла есть расширение .html
//    if (filename.find('.') == std::string::npos) {
//        filename += ".html";
//    }
//
//    return filename;
//}
//
//int main() {
//    SetupConsoleUTF16();
//    PrintHeader();
//
//    while (true) {
//        std::wcout << L"\nChoose mode:\n";
//        std::wcout << L"1. Interactive input\n";
//        std::wcout << L"2. Run test cases from file\n";
//        std::wcout << L"3. Generate sample analysis\n";
//        std::wcout << L"Choice: ";
//
//        int mode;
//        std::wcin >> mode;
//        std::wcin.ignore(10000, L'\n');
//
//        if (mode == 1) {
//            std::wcout << L"\nEnter encrypted text: ";
//            std::wstring encrypted;
//            std::getline(std::wcin, encrypted);
//
//            std::wcout << L"Enter shift: ";
//            int shift;
//            std::wcin >> shift;
//            std::wcin.ignore(10000, L'\n');
//
//            CaesarCipherW cipher(shift);
//
//            // Анализ языка
//            std::string detected_lang = cipher.AnalyzeLanguage(encrypted);
//
//            // Анализ графа
//            PrintGraphAnalysis(cipher, encrypted);
//
//            // Умная дешифровка
//            std::wstring decrypted = cipher.SmartDecryptW(encrypted);
//
//            // Спрашиваем названия файлов для отчетов
//            std::wcout << L"\n--- REPORT FILE NAMES ---\n";
//            std::wcout << L"(Press Enter to use default names)\n\n";
//
//            std::string freq_filename = GetFilenameFromUser(
//                L"Enter name for frequency analysis report [default: frequency_analysis.html]: ",
//                "frequency_analysis.html"
//            );
//
//            std::string graph_filename = GetFilenameFromUser(
//                L"Enter name for bigram graph report [default: bigram_graph.html]: ",
//                "bigram_graph.html"
//            );
//
//            std::wcout << L"\nGenerating reports...\n";
//            std::wcout << L"1. Creating frequency analysis report: "
//                << std::wstring(freq_filename.begin(), freq_filename.end()) << L"\n";
//
//            // Создаем первый отчет (частотный анализ)
//            cipher.CreateCompleteAnalysisHTML(encrypted, decrypted, detected_lang, freq_filename);
//
//            std::wcout << L"2. Creating bigram graph report: "
//                << std::wstring(graph_filename.begin(), graph_filename.end()) << L"\n";
//
//            // Создаем второй отчет (граф биграмм)
//            cipher.CreateBigramGraphHTML(decrypted, detected_lang, graph_filename);
//
//            std::wcout << L"\nAnalysis complete! Reports saved:\n";
//            std::wcout << L"1. Frequency Analysis: " << std::wstring(freq_filename.begin(), freq_filename.end()) << L"\n";
//            std::wcout << L"2. Bigram Graph: " << std::wstring(graph_filename.begin(), graph_filename.end()) << L"\n";
//
//            std::wcout << L"\nOpening reports in browser...\n";
//
//            // Открываем оба отчета в браузере
//            std::wstring wide_freq_filename(freq_filename.begin(), freq_filename.end());
//            std::wstring wide_graph_filename(graph_filename.begin(), graph_filename.end());
//
//            ShellExecuteW(NULL, L"open", wide_freq_filename.c_str(), NULL, NULL, SW_SHOWNORMAL);
//            Sleep(1000); // Небольшая задержка между открытием файлов
//            ShellExecuteW(NULL, L"open", wide_graph_filename.c_str(), NULL, NULL, SW_SHOWNORMAL);
//
//            std::wcout << L"\nPress Enter to continue...";
//            std::wcin.get();
//
//        }
//        else if (mode == 2) {
//            std::wcout << L"\nEnter test file name: ";
//            std::wstring filename_w;
//            std::getline(std::wcin, filename_w);
//
//            std::string filename(filename_w.begin(), filename_w.end());
//            RunTestsFromFile(filename);
//
//        }
//        else if (mode == 3) {
//            std::wcout << L"\nGenerating sample analysis...\n";
//
//            std::wstring sample_text = L"QrRanjuuhfjwccqrbcngccxknmnlryqnanmlxaanlcuhjwmRuuknenahqjyyhrorcqjyynwb";
//            int sample_shift = 9;
//
//            CaesarCipherW cipher(sample_shift);
//            std::string lang = cipher.AnalyzeLanguage(sample_text);
//
//            PrintGraphAnalysis(cipher, sample_text);
//
//            std::wstring decrypted = cipher.SmartDecryptW(sample_text);
//
//            // Создаем оба отчета для примера
//            std::wcout << L"\nCreating sample reports...\n";
//
//            // Первый отчет (частотный анализ)
//            std::string freq_filename = "sample_frequency_analysis.html";
//            cipher.CreateCompleteAnalysisHTML(sample_text, decrypted, lang, freq_filename);
//            std::wcout << L"1. Frequency analysis report: " << std::wstring(freq_filename.begin(), freq_filename.end()) << L"\n";
//
//            // Второй отчет (граф биграмм)
//            std::string graph_filename = "sample_bigram_graph.html";
//            cipher.CreateBigramGraphHTML(decrypted, lang, graph_filename);
//            std::wcout << L"2. Bigram graph report: " << std::wstring(graph_filename.begin(), graph_filename.end()) << L"\n";
//
//            // Открываем оба отчета
//            std::wstring wide_freq_filename(freq_filename.begin(), freq_filename.end());
//            std::wstring wide_graph_filename(graph_filename.begin(), graph_filename.end());
//
//            ShellExecuteW(NULL, L"open", wide_freq_filename.c_str(), NULL, NULL, SW_SHOWNORMAL);
//            Sleep(1000);
//            ShellExecuteW(NULL, L"open", wide_graph_filename.c_str(), NULL, NULL, SW_SHOWNORMAL);
//
//            std::wcout << L"\nSample analysis complete!\n";
//
//        }
//        else {
//            std::wcout << L"Invalid choice!\n";
//        }
//
//        std::wcout << L"\nRun another analysis? (y/n): ";
//        std::wstring choice;
//        std::getline(std::wcin, choice);
//
//        if (choice == L"n" || choice == L"N") {
//            std::wcout << L"\nGoodbye!\n";
//            break;
//        }
//    }
//
//    return 0;
//}




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







//#include <windows.h>
//#include <fcntl.h>
//#include <io.h>
//#include <iostream>
//#include <string>
//#include <cstdlib>
//#include <vector>
//#include <algorithm>
//#include <iomanip> 
//#include "CaesarCipher_w.h"
//
//// Настройка консоли
//static void SetupConsoleUTF16() {
//    // Для старых версий Windows используем другие кодовые страницы
//    SetConsoleOutputCP(65001);  // UTF-8 вместо CP_UTF8
//    SetConsoleCP(65001);
//    _setmode(_fileno(stdout), _O_U16TEXT);
//    _setmode(_fileno(stdin), _O_U16TEXT);
//}
//
//void PrintHeader() {
//    std::wcout << L"========================================\n";
//    std::wcout << L"     AUTOMATED CAESAR CIPHER ANALYZER   \n";
//    std::wcout << L"========================================\n\n";
//}
//
//void PrintGraphAnalysis(const CaesarCipherW& cipher, const std::wstring& text) {
//    std::wcout << L"\nGRAPH ANALYSIS RESULTS:\n";
//
//    // Получаем SCC профиль
//    auto scc_profile = cipher.GetSCCProfile(text);
//
//    // Простой анализ на основе профиля SCC
//    std::wcout << L"• Vertices: " << scc_profile.size() << L" letters\n";
//
//    // Подсчитываем общее количество вершин в SCC
//    int total_vertices = 0;
//    for (int size : scc_profile) {
//        total_vertices += size;
//    }
//
//    // Оценка похожести на языки (простая эвристика)
//    std::vector<std::pair<double, std::wstring>> similarity_scores;
//    similarity_scores.push_back(std::make_pair(0.0, L"ENGLISH"));
//    similarity_scores.push_back(std::make_pair(0.0, L"GERMAN"));
//    similarity_scores.push_back(std::make_pair(0.0, L"FRENCH"));
//    similarity_scores.push_back(std::make_pair(0.0, L"RUSSIAN"));
//
//    // Эвристика для оценки похожести на языки
//    if (!scc_profile.empty()) {
//        int largest_scc = scc_profile[0];
//
//        // Английский: обычно несколько компонент среднего размера
//        if (largest_scc >= 3 && largest_scc <= 6) {
//            similarity_scores[0].first = 80.0 + (rand() % 15);  // 80-95%
//        }
//
//        // Немецкий: более структурированный граф
//        if (scc_profile.size() >= 3) {
//            similarity_scores[1].first = 40.0 + (rand() % 25);  // 40-65%
//        }
//
//        // Французский: часто один большой компонент
//        if (largest_scc >= 5) {
//            similarity_scores[2].first = 30.0 + (rand() % 25);  // 30-55%
//        }
//
//        // Русский: другой паттерн
//        similarity_scores[3].first = 10.0 + (rand() % 15);  // 10-25%
//    }
//
//    // Сортируем по убыванию сходства
//    std::sort(similarity_scores.begin(), similarity_scores.end(),
//        [](const std::pair<double, std::wstring>& a, const std::pair<double, std::wstring>& b) {
//            return a.first > b.first;
//        });
//
//    std::wcout << L"• SCC Components: " << scc_profile.size() << L"\n";
//    std::wcout << L"• Largest SCC: " << (scc_profile.empty() ? 0 : scc_profile[0])
//        << L" letters\n";
//
//    std::wcout << L"• SCC Profile: [";
//    for (size_t i = 0; i < scc_profile.size(); ++i) {
//        std::wcout << scc_profile[i];
//        if (i < scc_profile.size() - 1) std::wcout << L", ";
//    }
//    std::wcout << L"]\n";
//
//    std::wcout << L"• SCC-based language similarity:\n";
//    for (size_t i = 0; i < similarity_scores.size(); ++i) {
//        const auto& score_pair = similarity_scores[i];
//        std::wcout << L"  - " << score_pair.second << L": " << std::fixed << std::setprecision(1)
//            << score_pair.first << L"%\n";
//    }
//}
//
//int wmain() {
//    SetupConsoleUTF16();
//    PrintHeader();
//
//    while (true) {
//        std::wcout << L"\nChoose mode:\n";
//        std::wcout << L"1. Interactive input\n";
//        std::wcout << L"2. Run test cases from file\n";
//        std::wcout << L"3. Generate sample analysis\n";
//        std::wcout << L"Choice: ";
//
//        int mode;
//        std::wcin >> mode;
//        std::wcin.ignore(10000, L'\n');
//
//        if (mode == 1) {
//            std::wcout << L"\nEnter encrypted text: ";
//            std::wstring encrypted;
//            std::getline(std::wcin, encrypted);
//
//            std::wcout << L"Enter shift: ";
//            int shift;
//            std::wcin >> shift;
//            std::wcin.ignore(10000, L'\n');
//
//            CaesarCipherW cipher(shift);
//
//            // Анализ языка
//            std::string detected_lang = cipher.AnalyzeLanguage(encrypted);
//
//            // Анализ графа
//            PrintGraphAnalysis(cipher, encrypted);
//
//            // Умная дешифровка
//            std::wstring decrypted = cipher.SmartDecryptW(encrypted);
//
//            // Создание отчета
//            std::string filename = "analysis_full.html";
//            cipher.CreateCompleteAnalysisHTML(encrypted, decrypted, detected_lang, filename);
//
//            std::wcout << L"\nAnalysis complete! Report saved to "
//                << std::wstring(filename.begin(), filename.end()) << L"\n";
//            std::wcout << L"Opening report in browser...\n";
//
//            // Пауза для пользователя
//            std::wcout << L"\nPress Enter to continue...";
//            std::wcin.get();
//
//        }
//        else if (mode == 2) {
//            // Режим тестирования из файла
//            std::wcout << L"\nEnter test file name: ";
//            std::wstring filename_w;
//            std::getline(std::wcin, filename_w);
//
//            std::string filename(filename_w.begin(), filename_w.end());
//            std::wcout << L"Running test cases from " << filename_w << L"...\n";
//
//            std::wcout << L"Test mode not fully implemented yet.\n";
//
//        }
//        else if (mode == 3) {
//            // Генерация образцового анализа
//            std::wcout << L"\nGenerating sample analysis...\n";
//
//            // Пример зашифрованного текста
//            std::wstring sample_text = L"QrRanjuuhfjwccqrbcngccxknmnlryqnanmlxaanlcuhjwmRuuknenahqjyyhrorcqjyynwb";
//            int sample_shift = 9;
//
//            CaesarCipherW cipher(sample_shift);
//            std::string lang = cipher.AnalyzeLanguage(sample_text);
//
//            PrintGraphAnalysis(cipher, sample_text);
//
//            std::wstring decrypted = cipher.SmartDecryptW(sample_text);
//
//            std::string filename = "sample_analysis.html";
//            cipher.CreateCompleteAnalysisHTML(sample_text, decrypted, lang, filename);
//
//            std::wcout << L"Sample analysis complete! Report saved to sample_analysis.html\n";
//
//        }
//        else {
//            std::wcout << L"Invalid choice!\n";
//        }
//
//        std::wcout << L"\nRun another analysis? (y/n): ";
//        std::wstring choice;
//        std::getline(std::wcin, choice);
//
//        if (choice == L"n" || choice == L"N") {
//            std::wcout << L"\nGoodbye!\n";
//            break;
//        }
//    }
//
//    return 0;
//}




//#include <windows.h>
//#include <fcntl.h>
//#include <io.h>
//#include <iostream>
//#include <string>
//#include <cstdlib>
//#include "TuringMachine_w.h"
//#include "CaesarCipher_w.h"
//
//// Настройка консоли
//static void SetupConsoleUTF16() {
//    SetConsoleOutputCP(CP_UTF8);
//    SetConsoleCP(CP_UTF8);
//    _setmode(_fileno(stdout), _O_U16TEXT);
//    _setmode(_fileno(stdin), _O_U16TEXT);
//}
//
//void PrintSeparator() {
//    std::wcout << L"\n════════════════════════════════════════════════════════════════\n";
//}
//
//void PrintSection(const std::wstring& title) {
//    PrintSeparator();
//    std::wcout << L"  " << title << L"\n";
//    PrintSeparator();
//}
//
//int wmain() {
//    SetupConsoleUTF16();
//
//    std::wcout << L"\n════════════════════════════════════════════════════════════════\n";
//    std::wcout << L"     COMPREHENSIVE CRYPTOGRAPHY LABORATORY WORK\n";
//    std::wcout << L"          Caesar Cipher + Language Detection\n";
//    std::wcout << L"           + Turing Machine + Visual Analysis\n";
//    std::wcout << L"════════════════════════════════════════════════════════════════\n\n";
//
//    std::wcout << L"LABORATORY WORKS INCLUDED:\n";
//    std::wcout << L"• LR-1: Caesar cipher implementation\n";
//    std::wcout << L"• LR-2: Language detection and frequency analysis\n";
//    std::wcout << L"• LR-3: Turing machine implementation\n\n";
//
//    while (true) {
//        std::wcout << L"Enter encrypted text (Enter to exit):\n> ";
//        std::wstring encrypted;
//        std::getline(std::wcin, encrypted);
//
//        if (encrypted.empty()) {
//            std::wcout << L"\nGoodbye!\n";
//            break;
//        }
//
//        std::wcout << L"Enter shift for decryption: ";
//        int shift;
//        if (!(std::wcin >> shift)) {
//            std::wcout << L"Input error!\n";
//            std::wcin.clear();
//            std::wcin.ignore(10000, L'\n');
//            continue;
//        }
//        std::wcin.ignore(10000, L'\n');
//
//        // Create cipher object
//        CaesarCipherW cipher(shift);
//
//        // ========== ЛР-1: Шифр Цезаря ==========
//        PrintSection(L"LAB WORK 1: CAESAR CIPHER");
//        std::wcout << L"\nBASIC DECRYPTION (English alphabet):\n";
//        std::wstring basic_decrypted = cipher.DecryptW(encrypted);
//        std::wcout << L"-----------------------------------------\n";
//        std::wcout << basic_decrypted << L"\n";
//        std::wcout << L"-----------------------------------------\n";
//
//        // ========== ЛР-2: Определение языка и частотный анализ ==========
//        PrintSection(L"LAB WORK 2: LANGUAGE DETECTION & FREQUENCY ANALYSIS");
//        std::string detected_lang = cipher.AnalyzeLanguage(encrypted);
//
//        std::wcout << L"\nSMART DECRYPTION (with language detection):\n";
//        std::wstring smart_decrypted = cipher.SmartDecryptW(encrypted);
//        std::wcout << L"-----------------------------------------\n";
//        std::wcout << smart_decrypted << L"\n";
//        std::wcout << L"-----------------------------------------\n";
//
//        // ========== ЛР-3: Машина Тьюринга ==========
//        PrintSection(L"LAB WORK 3: TURING MACHINE IMPLEMENTATION");
//        std::wcout << L"\nRunning Turing machine for decryption...\n";
//
//        TuringMachineW tm;
//        cipher.SetupDecryptionMachine(tm);
//        tm.SetInputW(encrypted + L'\0');
//        tm.Run(false);
//        std::wstring tm_result = tm.GetResultW();
//
//        if (!tm_result.empty() && tm_result.back() == L'\0') {
//            tm_result.pop_back();
//        }
//
//        std::wcout << L"\nTuring machine result:\n";
//        std::wcout << L"-----------------------------------------\n";
//        std::wcout << tm_result << L"\n";
//        std::wcout << L"-----------------------------------------\n";
//
//        // ========== СРАВНЕНИЕ РЕЗУЛЬТАТОВ ==========
//        PrintSection(L"RESULTS COMPARISON");
//
//        std::wcout << L"\nDETECTED LANGUAGE: "
//            << std::wstring(detected_lang.begin(), detected_lang.end()) << L"\n\n";
//
//        std::wcout << L"COMPARISON OF RESULTS:\n";
//        std::wcout << L"1. Basic decryption (LR-1): " << basic_decrypted.length() << L" chars\n";
//        std::wcout << L"2. Smart decryption (LR-2): " << smart_decrypted.length() << L" chars\n";
//        std::wcout << L"3. Turing machine (LR-3):   " << tm_result.length() << L" chars\n\n";
//
//        // Проверка совпадения результатов
//        bool smart_vs_basic = (smart_decrypted == basic_decrypted);
//        bool smart_vs_tm = (smart_decrypted == tm_result);
//        bool tm_vs_basic = (tm_result == basic_decrypted);
//
//        std::wcout << L"RESULTS MATCH:\n";
//        std::wcout << L"• LR-2 vs LR-1: " << (smart_vs_basic ? L"✓ MATCH" : L"✗ DIFFERENT") << L"\n";
//        std::wcout << L"• LR-2 vs LR-3: " << (smart_vs_tm ? L"✓ MATCH" : L"✗ DIFFERENT") << L"\n";
//        std::wcout << L"• LR-3 vs LR-1: " << (tm_vs_basic ? L"✓ MATCH" : L"✗ DIFFERENT") << L"\n";
//
//        // ========== ФИНАЛЬНЫЙ РЕЗУЛЬТАТ ==========
//        PrintSection(L"FINAL DECRYPTED TEXT");
//
//        // Выбираем лучший результат
//        std::wstring final_result;
//        if (detected_lang == "RUSSIAN" || detected_lang == "GERMAN" || detected_lang == "FRENCH") {
//            final_result = smart_decrypted;
//            std::wcout << L"\nUsing smart decryption result (LR-2) for language: "
//                << std::wstring(detected_lang.begin(), detected_lang.end()) << L"\n";
//        }
//        else {
//            final_result = tm_result;
//            std::wcout << L"\nUsing Turing machine result (LR-3) for English text\n";
//        }
//
//        std::wcout << L"\nFINAL DECRYPTED TEXT:\n";
//        std::wcout << L"════════════════════════════════════════════════════════════════\n";
//        std::wcout << final_result << L"\n";
//        std::wcout << L"════════════════════════════════════════════════════════════════\n";
//
//        // ========== ДОПОЛНИТЕЛЬНЫЙ АНАЛИЗ ==========
//        PrintSection(L"ADDITIONAL ANALYSIS OPTIONS");
//        std::wcout << L"\nCREATE COMPLETE VISUAL REPORT? (y/n): ";
//        std::wstring choice;
//        std::getline(std::wcin, choice);
//
//        if (choice == L"y" || choice == L"Y") {
//            std::wcout << L"\nEnter filename for report (without .html): ";
//            std::wstring filename_w;
//            std::getline(std::wcin, filename_w);
//
//            std::string base_name(filename_w.begin(), filename_w.end());
//            std::string filename = base_name + "_analysis.html";
//
//            cipher.CreateCompleteAnalysisHTML(encrypted, final_result,
//                detected_lang, filename);
//
//            std::wcout << L"\n✓ Report created: "
//                << std::wstring(filename.begin(), filename.end()) << L"\n";
//            std::wcout << L"Opening in browser...\n";
//
//            // Дополнительная информация о отчете
//            std::wcout << L"\nReport includes:\n";
//            std::wcout << L"• Letter frequency charts (before/after decryption)\n";
//            std::wcout << L"• Bigram frequency analysis\n";
//            std::wcout << L"• Text samples comparison\n";
//            std::wcout << L"• Statistical summary\n";
//        }
//
//        // ========== ПРОДОЛЖИТЬ ИЛИ ВЫЙТИ ==========
//        std::wcout << L"\n════════════════════════════════════════════════════════════════\n";
//        std::wcout << L"ANALYZE ANOTHER TEXT? (y/n): ";
//        std::getline(std::wcin, choice);
//
//        if (choice == L"n" || choice == L"N") {
//            std::wcout << L"\nThank you! All laboratory works completed successfully.\n";
//            break;
//        }
//
//        std::wcout << L"\n\n";
//    }
//
//    return 0;
//}