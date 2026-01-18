#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "CaesarCipher_w.h"
#include "GraphAnalyzer_w.h"
#include <algorithm>
#include <cwctype>
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <windows.h>
#include <shellapi.h>
#include <locale>
#include <codecvt>

// Алфавиты
const std::wstring CaesarCipherW::EN_LOW = L"abcdefghijklmnopqrstuvwxyz";
const std::wstring CaesarCipherW::EN_UP = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::wstring CaesarCipherW::RU_LOW = L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
const std::wstring CaesarCipherW::RU_UP = L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
const std::wstring CaesarCipherW::DE_LOW = L"abcdefghijklmnopqrstuvwxyzäößü";
const std::wstring CaesarCipherW::DE_UP = L"ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖẞÜ";
const std::wstring CaesarCipherW::FR_LOW = L"abcdefghijklmnopqrstuvwxyzàâæçéèêëîïôœûùüÿ";
const std::wstring CaesarCipherW::FR_UP = L"ABCDEFGHIJKLMNOPQRSTUVWXYZÀÂÆÇÉÈÊËÎÏÔŒÛÙÜŸ";

// Эталонные частоты букв
const double CaesarCipherW::EN_FREQ[26] = {
    8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015, 6.094, 6.966, 0.153,
    0.772, 4.025, 2.406, 6.749, 7.507, 1.929, 0.095, 5.987, 6.327, 9.056,
    2.758, 0.978, 2.360, 0.150, 1.974, 0.074
};

const double CaesarCipherW::RU_FREQ[33] = {
    8.01, 1.59, 4.54, 1.70, 2.98, 0.94, 0.02, 1.02, 7.45, 1.21,
    3.49, 4.40, 3.21, 6.70, 10.97, 2.81, 0.47, 4.73, 5.47, 6.26,
    2.62, 0.26, 0.97, 0.48, 1.44, 0.73, 0.36, 1.90, 0.04, 1.74,
    1.90, 0.32, 0.64
};

const double CaesarCipherW::DE_FREQ[30] = {
    6.51, 1.89, 3.06, 5.08, 17.40, 1.66, 3.01, 4.76, 7.55, 0.27,
    1.21, 3.44, 2.53, 9.78, 2.51, 0.79, 0.02, 7.00, 7.27, 6.15,
    4.35, 0.67, 1.89, 0.03, 0.04, 1.13, 0.31, 0.30, 0.65, 0.31
};

const double CaesarCipherW::FR_FREQ[42] = {
    7.636, 0.901, 3.260, 3.669, 14.715, 1.066, 0.866, 0.737, 7.529, 0.613,
    0.049, 5.456, 2.968, 7.095, 5.796, 2.521, 1.362, 6.693, 7.948, 7.244,
    6.311, 1.838, 0.074, 0.427, 0.128, 0.326, 0.486, 0.051, 0.085, 0.271,
    0.218, 0.008, 0.045, 0.023, 0.032, 0.009, 0.042, 0.015, 0.005, 0.003,
    0.002, 0.001
};

CaesarCipherW::CaesarCipherW(int shift) : shift(shift) {}

int CaesarCipherW::mod_positive(int a, int m) {
    int r = a % m;
    if (r < 0) r += m;
    return r;
}

wchar_t CaesarCipherW::ShiftInAlphabet(const std::wstring& alph, wchar_t ch, int s, bool encrypt) const {
    std::size_t pos = alph.find(ch);
    if (pos == std::wstring::npos) return ch;
    int n = static_cast<int>(alph.size());
    int delta = encrypt ? s : -s;
    int newPos = CaesarCipherW::mod_positive(static_cast<int>(pos) + delta, n);
    return alph[newPos];
}

std::wstring CaesarCipherW::DecryptWithAlphabet(const std::wstring& wtext,
    const std::wstring& low,
    const std::wstring& up) const {
    std::wstring out;
    out.reserve(wtext.size());
    for (wchar_t ch : wtext) {
        size_t pl = low.find(ch);
        size_t pu = up.find(ch);
        if (pl != std::wstring::npos)
            out.push_back(ShiftInAlphabet(low, ch, shift, false));
        else if (pu != std::wstring::npos)
            out.push_back(ShiftInAlphabet(up, ch, shift, false));
        else
            out.push_back(ch);
    }
    return out;
}

std::wstring CaesarCipherW::DecryptW(const std::wstring& wtext) const {
    return DecryptWithAlphabet(wtext, EN_LOW, EN_UP);
}

// Получение частот букв
std::map<wchar_t, double> CaesarCipherW::GetLetterFrequencies(const std::wstring& text) const {
    std::map<wchar_t, double> frequencies;

    // Нормализуем текст
    std::wstring normalized;
    for (wchar_t ch : text) {
        wchar_t lower = std::towlower(static_cast<wint_t>(ch));
        if (std::iswalpha(static_cast<wint_t>(lower))) {
            normalized.push_back(lower);
        }
    }

    if (normalized.empty()) {
        return frequencies;
    }

    // Считаем буквы
    std::map<wchar_t, int> counts;
    for (wchar_t ch : normalized) {
        counts[ch]++;
    }

    // Преобразуем в частоты (проценты)
    for (const auto& pair : counts) {
        frequencies[pair.first] = (pair.second * 100.0) / normalized.size();
    }

    return frequencies;
}

// Получение частот биграмм
std::map<std::wstring, double> CaesarCipherW::GetBigramFrequencies(const std::wstring& text) const {
    std::map<std::wstring, double> frequencies;

    // Нормализуем текст
    std::wstring normalized;
    for (wchar_t ch : text) {
        wchar_t lower = std::towlower(static_cast<wint_t>(ch));
        if (std::iswalpha(static_cast<wint_t>(lower))) {
            normalized.push_back(lower);
        }
    }

    if (normalized.size() < 2) {
        return frequencies;
    }

    // Считаем биграммы
    std::map<std::wstring, int> counts;
    int total_bigrams = 0;

    for (size_t i = 0; i < normalized.size() - 1; ++i) {
        std::wstring bigram = normalized.substr(i, 2);
        counts[bigram]++;
        total_bigrams++;
    }

    // Преобразуем в частоты (проценты)
    if (total_bigrams > 0) {
        for (const auto& pair : counts) {
            frequencies[pair.first] = (pair.second * 100.0) / total_bigrams;
        }
    }

    return frequencies;
}

// Определение языка
//std::string CaesarCipherW::DetectLanguage(const std::wstring& text) const {
//    // Подсчет символов для статистики
//    int total_chars = 0;
//    int basic_latin = 0;
//    int cyrillic = 0;
//    int german_special = 0;
//    int french_special = 0;
//    
//    for (wchar_t ch : text) {
//        if (std::iswalpha(ch)) {
//            total_chars++;
//            
//            // Базовые латинские символы a-z, A-Z
//            if ((ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z')) {
//                basic_latin++;
//            }
//            // Кириллица
//            else if ((ch >= L'а' && ch <= L'я') || (ch >= L'А' && ch <= L'Я') || 
//                     ch == L'ё' || ch == L'Ё') {
//                cyrillic++;
//            }
//            // Немецкие специальные символы
//            else if (ch == L'ä' || ch == L'ö' || ch == L'ü' || ch == L'ß' ||
//                     ch == L'Ä' || ch == L'Ö' || ch == L'Ü' || ch == L'ẞ') {
//                german_special++;
//            }
//            // Французские специальные символы
//            else if (ch == L'à' || ch == L'â' || ch == L'æ' || ch == L'ç' ||
//                     ch == L'é' || ch == L'è' || ch == L'ê' || ch == L'ë' ||
//                     ch == L'î' || ch == L'ï' || ch == L'ô' || ch == L'œ' ||
//                     ch == L'û' || ch == L'ù' || ch == L'ÿ' || 
//                     ch == L'À' || ch == L'Â' || ch == L'Æ' || ch == L'Ç' ||
//                     ch == L'É' || ch == L'È' || ch == L'Ê' || ch == L'Ë' ||
//                     ch == L'Î' || ch == L'Ï' || ch == L'Ô' || ch == L'Œ' ||
//                     ch == L'Û' || ch == L'Ù' || ch == L'Ÿ') {
//                french_special++;
//            }
//        }
//    }
//    
//    // 1. Если есть специфические символы - определяем по ним
//    if (cyrillic > 0) return "RUSSIAN";
//    if (german_special > 0) return "GERMAN";
//    if (french_special > 0) return "FRENCH";
//    
//    // 2. Если все символы - базовые латинские, это английский
//    if (basic_latin == total_chars && total_chars > 0) {
//        return "ENGLISH";
//    }
//    
//    // 3. Для смешанных случаев используем частотный анализ, но только при достаточном количестве символов
//    if (total_chars < 30) {
//        // Мало символов - используем английский по умолчанию для латинских символов
//        return "ENGLISH";
//    }
//    
//    // 4. Частотный анализ для длинных текстов без специальных символов
//    auto freqs = GetLetterFrequencies(text);
//    
//    // Создаем векторы для каждого языка
//    std::vector<double> text_vector_en(26, 0.0);
//    std::vector<double> text_vector_ru(33, 0.0);
//    std::vector<double> text_vector_de(30, 0.0);
//    std::vector<double> text_vector_fr(42, 0.0);
//
//    // Заполняем векторы
//    for (const auto& pair : freqs) {
//        wchar_t ch = pair.first;
//
//        if (EN_LOW.find(ch) != std::wstring::npos) {
//            int idx = EN_LOW.find(ch);
//            if (idx < 26) text_vector_en[idx] = pair.second;
//        }
//
//        if (RU_LOW.find(ch) != std::wstring::npos) {
//            int idx = RU_LOW.find(ch);
//            if (idx < 33) text_vector_ru[idx] = pair.second;
//        }
//
//        if (DE_LOW.find(ch) != std::wstring::npos) {
//            int idx = DE_LOW.find(ch);
//            if (idx < 30) text_vector_de[idx] = pair.second;
//        }
//
//        if (FR_LOW.find(ch) != std::wstring::npos) {
//            int idx = FR_LOW.find(ch);
//            if (idx < 42) text_vector_fr[idx] = pair.second;
//        }
//    }
//
//    // Вычисляем косинусную близость
//    auto cosine_similarity = [](const std::vector<double>& v1, const double* v2, int size) -> double {
//        double dot = 0.0, norm1 = 0.0, norm2 = 0.0;
//        for (int i = 0; i < size; ++i) {
//            dot += v1[i] * v2[i];
//            norm1 += v1[i] * v1[i];
//            norm2 += v2[i] * v2[i];
//        }
//        if (norm1 == 0.0 || norm2 == 0.0) return 0.0;
//        return dot / (std::sqrt(norm1) * std::sqrt(norm2));
//        };
//
//    // Сравниваем с эталонами
//    double score_en = cosine_similarity(text_vector_en, EN_FREQ, 26);
//    double score_ru = cosine_similarity(text_vector_ru, RU_FREQ, 33);
//    double score_de = cosine_similarity(text_vector_de, DE_FREQ, 30);
//    double score_fr = cosine_similarity(text_vector_fr, FR_FREQ, 42);
//
//    // Выбираем лучший с порогом надежности
//    std::vector<std::pair<double, std::string>> scores = {
//        {score_en, "ENGLISH"},
//        {score_ru, "RUSSIAN"},
//        {score_de, "GERMAN"},
//        {score_fr, "FRENCH"}
//    };
//
//    std::sort(scores.begin(), scores.end(),
//        [](const std::pair<double, std::string>& a, const std::pair<double, std::string>& b) {
//            return a.first > b.first;
//        });
//
//    // Если лучший результат имеет низкую уверенность (< 0.3), используем английский
//    if (scores[0].first < 0.3) {
//        return "ENGLISH";
//    }
//    
//    // Если разница между лучшим и вторым небольшая (< 0.1), выбираем английский для текстов без спецсимволов
//    if (scores.size() >= 2 && (scores[0].first - scores[1].first) < 0.1) {
//        if (scores[0].second != "ENGLISH") {
//            // Проверяем, были ли в тексте не-латинские символы
//            if (basic_latin == total_chars) {
//                return "ENGLISH";
//            }
//        }
//    }
//
//    return scores[0].second;
//}

std::string CaesarCipherW::DetectLanguage(const std::wstring& text) const {
    // Подсчет символов для статистики
    int total_chars = 0;
    int basic_latin_only = 0; // ТОЛЬКО a-z без диакритик
    int cyrillic = 0;
    int german_special = 0;
    int french_special = 0;

    for (wchar_t ch : text) {
        if (std::iswalpha(ch)) {
            total_chars++;

            // ТОЛЬКО базовые латинские символы a-z, A-Z БЕЗ диакритик
            if ((ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z')) {
                basic_latin_only++;
            }
            // Кириллица
            else if ((ch >= L'а' && ch <= L'я') || (ch >= L'А' && ch <= L'Я') ||
                ch == L'ё' || ch == L'Ё') {
                cyrillic++;
            }
            // Немецкие специальные символы
            else if (DE_LOW.find(ch) != std::wstring::npos ||
                DE_UP.find(ch) != std::wstring::npos) {
                // Проверяем, что это действительно немецкие символы
                if (ch == L'ä' || ch == L'ö' || ch == L'ü' || ch == L'ß' ||
                    ch == L'Ä' || ch == L'Ö' || ch == L'Ü' || ch == L'ẞ') {
                    german_special++;
                }
            }
            // Французские специальные символы
            else if (FR_LOW.find(ch) != std::wstring::npos ||
                FR_UP.find(ch) != std::wstring::npos) {
                // Проверяем, что это действительно французские символы
                if (ch == L'à' || ch == L'â' || ch == L'æ' || ch == L'ç' ||
                    ch == L'é' || ch == L'è' || ch == L'ê' || ch == L'ë' ||
                    ch == L'î' || ch == L'ï' || ch == L'ô' || ch == L'œ' ||
                    ch == L'û' || ch == L'ù' || ch == L'ÿ' ||
                    ch == L'À' || ch == L'Â' || ch == L'Æ' || ch == L'Ç' ||
                    ch == L'É' || ch == L'È' || ch == L'Ê' || ch == L'Ë' ||
                    ch == L'Î' || ch == L'Ï' || ch == L'Ô' || ch == L'Œ' ||
                    ch == L'Û' || ch == L'Ù' || ch == L'Ÿ') {
                    french_special++;
                }
            }
        }
    }

    std::wcout << L"[DEBUG] Language stats - Total: " << total_chars
        << L", Basic Latin: " << basic_latin_only
        << L", German special: " << german_special
        << L", French special: " << french_special << L"\n";

    // 1. Если есть специфические символы - определяем по ним
    if (cyrillic > 0) return "RUSSIAN";
    if (german_special > 0) return "GERMAN";
    if (french_special > 0) return "FRENCH";

    // 2. Если ВСЕ символы - базовые латинские без диакритик, это английский
    if (basic_latin_only == total_chars && total_chars > 0) {
        return "ENGLISH";
    }

    // 3. Для остальных случаев используем английский по умолчанию
    // (немецкие/французские тексты без диакритик встречаются редко)
    return "ENGLISH";
}

// После существующих функций добавьте:

// Определение языка по SCC с улучшенной логикой для коротких текстов
std::string CaesarCipherW::DetectLanguageBySCC(const std::wstring& text) const {
    if (text.length() < 80) {
        return "INSUFFICIENT_DATA"; // Слишком коротко для SCC
    }

    auto bigrams = GetBigramFrequencies(text);
    GraphAnalyzer analyzer;
    analyzer.buildGraphFromBigrams(bigrams);

    auto sccs = analyzer.getStronglyConnectedComponents();
    auto profile = analyzer.getSCCProfile();
    auto stats = analyzer.getGraphStats();

    if (sccs.empty()) {
        return "UNKNOWN";
    }

    // Для коротких текстов используем упрощенный анализ
    int largest_size = static_cast<int>(sccs[0].size());
    int total_components = static_cast<int>(sccs.size());
    int total_letters = static_cast<int>(analyzer.getAllLetters().size());

    // Отношение размера крупнейшего SCC к общему числу букв
    double largest_ratio = static_cast<double>(largest_size) / total_letters;

    // Эвристики для разных языков:

    // РУССКИЙ: Очень крупный SCC (> 60% букв)
    if (largest_ratio > 0.6 && largest_size >= 10) {
        return "RUSSIAN";
    }

    // АНГЛИЙСКИЙ: Несколько средних SCC
    if (total_components >= 4 && largest_size >= 4 && largest_size <= 7) {
        return "ENGLISH";
    }

    // НЕМЕЦКИЙ: Умеренное количество SCC, средний размер
    if (total_components >= 3 && total_components <= 5 &&
        largest_size >= 5 && largest_size <= 8) {
        return "GERMAN";
    }

    // ФРАНЦУЗСКИЙ: Много мелких SCC
    if (total_components >= 5 && largest_size <= 6) {
        return "FRENCH";
    }

    // Если не подошло ни под одно правило
    return "UNKNOWN";
}

// Оценка уверенности SCC-анализа
double CaesarCipherW::GetSCCConfidence(const std::wstring& text) const {
    if (text.length() < 80) {
        return 0.0; // Слишком коротко
    }

    auto bigrams = GetBigramFrequencies(text);
    if (bigrams.size() < 25) {
        return 0.1; // Очень мало биграмм
    }

    GraphAnalyzer analyzer;
    analyzer.buildGraphFromBigrams(bigrams);
    auto sccs = analyzer.getStronglyConnectedComponents();

    if (sccs.empty()) {
        return 0.0;
    }

    // Уверенность зависит от нескольких факторов:
    double confidence = 0.0;

    // 1. Длина текста (0-0.3)
    confidence += (std::min)(text.length() / 300.0, 0.3);

    // 2. Количество SCC (0-0.3)
    confidence += (std::min)(sccs.size() / 8.0, 0.3);

    // 3. Наличие нетривиальных SCC (>1 буква) (0-0.4)
    int non_trivial = 0;
    for (const auto& scc : sccs) {
        if (scc.size() > 1) non_trivial++;
    }
    confidence += (std::min)(non_trivial / 6.0, 0.4);

    return (std::min)(confidence, 1.0);
}

// Финальное многоэтапное определение языка
std::string CaesarCipherW::FinalLanguageDetection(const std::wstring& text) const {
    // === ЭТАП 1: Быстрая проверка по спецсимволам ===
    bool has_russian = (text.find_first_of(RU_LOW + RU_UP) != std::wstring::npos);
    bool has_german = (text.find_first_of(L"äöüßÄÖÜẞ") != std::wstring::npos);
    bool has_french = (text.find_first_of(L"àâæçéèêëîïôœûùÿÀÂÆÇÉÈÊËÎÏÔŒÛÙŸ") != std::wstring::npos);

    if (has_russian) return "RUSSIAN";
    if (has_german) return "GERMAN";
    if (has_french) return "FRENCH";

    // === ЭТАП 2: Частотный анализ (основной метод) ===
    std::string freq_lang = DetectLanguage(text); // Используем существующий метод
    double freq_confidence = 0.7; // Базовая уверенность частотного анализа

    // Увеличиваем уверенность для длинных текстов
    if (text.length() > 150) freq_confidence = 0.85;
    if (text.length() > 300) freq_confidence = 0.95;

    // === ЭТАП 3: SCC анализ (только если текст ≥ 80 символов) ===
    std::string scc_lang = "NO_SCC";
    double scc_confidence = 0.0;

    if (text.length() >= 80) {
        scc_lang = DetectLanguageBySCC(text);
        scc_confidence = GetSCCConfidence(text);
    }

    // === ЭТАП 4: Принятие решения ===

    // Если SCC анализ не дал результата или уверенность низкая
    if (scc_lang == "INSUFFICIENT_DATA" || scc_lang == "UNKNOWN" ||
        scc_confidence < 0.3) {
        return freq_lang; // Доверяем частотному анализу
    }

    // Если SCC предлагает тот же язык
    if (scc_lang == freq_lang) {
        return freq_lang; // Подтверждение
    }

    // Если SCC противоречит с высокой уверенностью
    if (scc_confidence > 0.7) {
        // Особый случай: SCC обнаружил русский, но нет кириллицы
        // Это может быть зашифрованный русский текст
        if (scc_lang == "RUSSIAN" && !has_russian) {
            std::wcout << L"\n[ВНИМАНИЕ] SCC анализ обнаружил паттерн русского языка,\n";
            std::wcout << L"          хотя в тексте нет кириллицы.\n";
            std::wcout << L"          Возможно, это зашифрованный русский текст.\n";

            // Запрашиваем подтверждение у пользователя
            // Или возвращаем русский с пометкой
            return "RUSSIAN (предположительно, по граф-анализу)";
        }

        // Для других языков логируем расхождение
        std::wcout << L"\n[ИНФО] Расхождение методов:\n";
        std::wcout << L"       Частотный анализ: " << freq_lang.c_str() << L"\n";
        std::wcout << L"       SCC анализ: " << scc_lang.c_str()
            << L" (уверенность: " << scc_confidence << L")\n";

        // При высоком качестве SCC доверяем ему
        if (scc_confidence > 0.8) {
            return scc_lang;
        }
    }

    // По умолчанию возвращаем результат частотного анализа
    return freq_lang;
}

std::wstring CaesarCipherW::SmartDecryptW(const std::wstring& wtext) const {
    // Если текст очень короткий (< 3 символов), используем английский
    if (wtext.length() < 3) {
        return DecryptWithAlphabet(wtext, EN_LOW, EN_UP);
    }

    // Определяем язык
    std::string lang = FinalLanguageDetection(wtext);

    std::wcout << L"\n[DEBUG] Language detection result: "
        << std::wstring(lang.begin(), lang.end()) << L"\n";

    // Важно: проверяем СНАЧАЛА английский, потом немецкий
    // потому что немецкий алфавит включает английский

    // 1. Сначала проверяем однозначные признаки
    bool has_russian = (wtext.find_first_of(RU_LOW + RU_UP) != std::wstring::npos);
    bool has_german_special = (wtext.find_first_of(L"äöüßÄÖÜẞ") != std::wstring::npos);
    bool has_french_special = (wtext.find_first_of(L"àâæçéèêëîïôœûùÿÀÂÆÇÉÈÊËÎÏÔŒÛÙŸ") != std::wstring::npos);

    if (has_russian) {
        return DecryptWithAlphabet(wtext, RU_LOW, RU_UP);
    }
    else if (has_german_special) {
        return DecryptWithAlphabet(wtext, DE_LOW, DE_UP);
    }
    else if (has_french_special) {
        return DecryptWithAlphabet(wtext, FR_LOW, FR_UP);
    }
    // 2. Если нет специальных символов - это английский
    else {
        return DecryptWithAlphabet(wtext, EN_LOW, EN_UP);
    }
}

std::wstring CaesarCipherW::EncryptW(const std::wstring& wtext) const {
    CaesarCipherW c(-shift);
    return c.SmartDecryptW(wtext);
}

// Анализ языка с выводом
std::string CaesarCipherW::AnalyzeLanguage(const std::wstring& text) const {
    // Простой анализ по наличию специальных символов
    bool has_russian = (text.find_first_of(RU_LOW + RU_UP) != std::wstring::npos);
    bool has_german = (text.find_first_of(L"äöüßÄÖÜẞ") != std::wstring::npos);
    bool has_french = (text.find_first_of(L"àâæçéèêëîïôœûùÿÀÂÆÇÉÈÊËÎÏÔŒÛÙŸ") != std::wstring::npos);

    std::string detected_lang = "ENGLISH";

    if (has_russian) {
        detected_lang = "RUSSIAN";
    }
    else if (has_german) {
        detected_lang = "GERMAN";
    }
    else if (has_french) {
        detected_lang = "FRENCH";
    }
    else if (text.length() >= 50) {
        // Только для достаточно длинных текстов используем частотный анализ
        detected_lang = DetectLanguage(text);
    }

    std::wcout << L"\nANALYSIS OF TEXT LANGUAGE:\n";
    std::wcout << L"===============================\n";

    auto freqs = GetLetterFrequencies(text);
    std::wcout << L"• Total letters for analysis: " << freqs.size() << L"\n";

    if (freqs.size() < 10) {
        std::wcout << L"• Note: Text too short for reliable frequency analysis\n";
    }

    // Показываем топ-5 самых частых букв
    if (!freqs.empty()) {
        std::vector<std::pair<wchar_t, double>> sorted(freqs.begin(), freqs.end());
        std::sort(sorted.begin(), sorted.end(),
            [](const std::pair<wchar_t, double>& a, const std::pair<wchar_t, double>& b) {
                return a.second > b.second;
            });

        std::wcout << L"• Top-5 most frequent letters:\n";
        size_t limit = (sorted.size() < 5) ? sorted.size() : 5;
        for (size_t i = 0; i < limit; ++i) {
            std::wcout << L"  " << (i + 1) << L". '" << sorted[i].first << L"' - "
                << std::fixed << std::setprecision(2) << sorted[i].second << L"%\n";
        }
    }

    // Показываем обнаруженные специальные символы
    std::wcout << L"• Special characters detected:\n";
    if (has_russian) std::wcout << L"  - Russian/Cyrillic: ✓\n";
    if (has_german) std::wcout << L"  - German umlauts: ✓\n";
    if (has_french) std::wcout << L"  - French accents: ✓\n";
    if (!has_russian && !has_german && !has_french) {
        std::wcout << L"  - None (basic Latin only)\n";
    }

    std::wcout << L"• Detected language: "
        << std::wstring(detected_lang.begin(), detected_lang.end()) << L"\n";

    return detected_lang;
}

std::string wchar_to_id(wchar_t wc) {
    // Уникальный ID, который будет одинаковым в C++ и JavaScript
    char buf[32];

    // Английские буквы - как есть
    if ((wc >= L'A' && wc <= L'Z') || (wc >= L'a' && wc <= L'z')) {
        buf[0] = static_cast<char>(wc);
        buf[1] = '\0';
        return buf;
    }

    // Русские и специальные символы - с префиксом
    snprintf(buf, sizeof(buf), "c%04X", (unsigned int)wc);
    return buf;
}

std::string wchar_to_label(wchar_t wc) {
    // Прямая конвертация wchar_t в UTF-8 строку
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    try {
        return converter.to_bytes(wc);
    }
    catch (...) {
        // Fallback: ручная конвертация для основных символов
        switch (wc) {
            // Французские
        case L'é': case L'É': return "é";
        case L'è': case L'È': return "è";
        case L'ê': case L'Ê': return "ê";
        case L'ë': case L'Ë': return "ë";
        case L'à': case L'À': return "à";
        case L'â': case L'Â': return "â";
        case L'ç': case L'Ç': return "ç";
        case L'î': case L'Î': return "î";
        case L'ï': case L'Ï': return "ï";
        case L'ô': case L'Ô': return "ô";
        case L'ù': case L'Ù': return "ù";
        case L'û': case L'Û': return "û";
        case L'œ': case L'Œ': return "œ";
        case L'æ': case L'Æ': return "æ";

            // Немецкие
        case L'ä': case L'Ä': return "ä";
        case L'ö': case L'Ö': return "ö";
        case L'ü': case L'Ü': return "ü";
        case L'ß': return "ß";

            // Русские (первые несколько для примера)
        case L'А': return "А"; case L'а': return "а";
        case L'Б': return "Б"; case L'б': return "б";
        case L'В': return "В"; case L'в': return "в";
            // ... добавьте остальные русские буквы

        default:
            char buf[16];
            snprintf(buf, sizeof(buf), "[%d]", (int)wc);
            return buf;
        }
    }
}

std::string wstring_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";

    // Используем стандартный конвертер
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    try {
        return converter.to_bytes(wstr);
    }
    catch (const std::exception& e) {
        // Fallback: ручная конвертация для UTF-16 в UTF-8
        std::string result;

        for (wchar_t wc : wstr) {
            if (wc < 0x80) {
                // ASCII
                result += static_cast<char>(wc);
            }
            else if (wc < 0x800) {
                // 2-byte UTF-8
                result += static_cast<char>(0xC0 | (wc >> 6));
                result += static_cast<char>(0x80 | (wc & 0x3F));
            }
            else if (wc < 0x10000) {
                // 3-byte UTF-8 (для большинства символов)
                result += static_cast<char>(0xE0 | (wc >> 12));
                result += static_cast<char>(0x80 | ((wc >> 6) & 0x3F));
                result += static_cast<char>(0x80 | (wc & 0x3F));
            }
        }
        return result;
    }
}

// Вспомогательная функция для экранирования HTML
static std::string escape_html(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 2);

    for (char c : str) {
        switch (c) {
        case '&':  result.append("&amp;"); break;
        case '<':  result.append("&lt;"); break;
        case '>':  result.append("&gt;"); break;
        case '"':  result.append("&quot;"); break;
        case '\'': result.append("&#39;"); break;
        default:   result.push_back(c); break;
        }
    }

    return result;
}

std::string escape_js(const std::string& input) {
    std::string output;

    for (char c : input) {
        // Экранируем только опасные для JavaScript символы
        switch (c) {
        case '\\': output += "\\\\"; break;
        case '\"': output += "\\\""; break;
        case '\'': output += "\\'"; break;
        case '\n': output += "\\n"; break;
        case '\r': output += "\\r"; break;
        case '\t': output += "\\t"; break;
        default:
            // РАЗРЕШАЕМ ВСЕ символы, включая Unicode
            output += c;
        }
    }

    return output;
}

// Создание простого HTML отчета
void CaesarCipherW::CreateCompleteAnalysisHTML(const std::wstring& encrypted,
    const std::wstring& decrypted,
    const std::string& detected_lang,
    const std::string& filename) const {

    // Получаем частоты для всех графиков
    auto freqs_before = GetLetterFrequencies(encrypted);
    auto freqs_after = GetLetterFrequencies(decrypted);
    auto bigrams = GetBigramFrequencies(decrypted);

    // НОВОЕ: Анализ графа
    GraphAnalyzer graphAnalyzer;
    graphAnalyzer.buildGraphFromBigrams(bigrams);
    auto sccs = graphAnalyzer.getStronglyConnectedComponents();
    auto sccProfile = graphAnalyzer.getSCCProfile();
    auto graphStats = graphAnalyzer.getGraphStats();
    auto allBigramsForSlider = graphAnalyzer.getAllBigramsWithFrequencies();

    // Вычислите максимальную частоту
    double maxBigramFreq = 0.0;
    for (const auto& [bigram, freq] : allBigramsForSlider) {
        if (freq > maxBigramFreq) {
            maxBigramFreq = freq;
        }
    }

    if (maxBigramFreq <= 0.0) {
        maxBigramFreq = 0.1;
    }
    else {
        maxBigramFreq *= 1.2;
        if (maxBigramFreq > 0.5) maxBigramFreq = 0.5;
    }

    // Эталонные SCC профили для языков
    std::map<std::string, std::vector<int>> referenceSCCProfiles = {
        // Английский: МНОГО мелких компонентов (алфавит маленький, связи плотные)
        {"ENGLISH", {4, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1}},

        // Русский: 1-2 ОЧЕНЬ крупных + немного мелких (сложные сочетания)
        {"RUSSIAN", {12, 6, 2, 1, 1}},

        // Немецкий: Несколько РАВНЫХ по размеру (составные слова)
        {"GERMAN", {5, 5, 4, 3, 2, 1}},

        // Французский: СРЕДНИЙ максимальный, но много компонентов (акценты изолируют буквы)
        {"FRENCH", {6, 3, 2, 2, 1, 1, 1, 1, 1}}
    };

    // Сравнение с эталонами
    std::vector<std::pair<double, std::string>> sccScores;
    for (const auto& ref : referenceSCCProfiles) {
        double score = GraphAnalyzer::compareSCCProfiles(sccProfile, ref.second);
        sccScores.push_back({ score, ref.first });
    }
    std::sort(sccScores.begin(), sccScores.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });

    // Создаем HTML файл
    std::ofstream html(filename);

    // Простой HTML с Chart.js
    html << R"(<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Caesar Cipher Analysis</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
        <!-- Библиотека для визуализации графов -->
    <script src="https://unpkg.com/vis-network/standalone/umd/vis-network.min.js"></script>
    <link href="https://unpkg.com/vis-network/styles/vis-network.min.css" rel="stylesheet" type="text/css" />
    <style>
        body { 
            font-family: Arial, sans-serif; 
            padding: 20px; 
            background-color: #f0f0f0;
        }
        .container { 
            max-width: 1000px; 
            margin: 0 auto; 
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
        }
        h1, h2, h3 { 
            color: #333; 
            margin-top: 20px;
        }
        .chart-container { 
            width: 100%; 
            height: 350px; 
            margin: 20px 0;
        }
        .info-box { 
            background: #e8f4f8; 
            padding: 15px; 
            margin: 20px 0; 
            border-radius: 5px;
            border-left: 4px solid #3498db;
        }
        .text-box {
            background: #f9f9f9;
            padding: 10px;
            margin: 10px 0;
            border-radius: 5px;
            font-family: monospace;
            border: 1px solid #ddd;
            white-space: pre-wrap;
            word-break: break-word;
        }
        .stats {
            display: flex;
            justify-content: space-around;
            margin: 20px 0;
            flex-wrap: wrap;
        }
        .stat {
            text-align: center;
            padding: 10px;
            margin: 5px;
        }
        .stat-value {
            font-size: 24px;
            font-weight: bold;
            color: #2c3e50;
        }
        .stat-label {
            font-size: 14px;
            color: #7f8c8d;
        }
        .graph-container {
            width: 100%;
            height: 500px;
            border: 1px solid #ddd;
            margin: 20px 0;
            background: white;
        }
        
        .scc-container {
            display: flex;
            flex-wrap: wrap;
            gap: 20px;
            margin: 20px 0;
        }
        
        .scc-box {
            background: #f8f9fa;
            border: 2px solid #dee2e6;
            border-radius: 8px;
            padding: 15px;
            min-width: 200px;
        }
        
        .scc-title {
            font-weight: bold;
            color: #2c3e50;
            margin-bottom: 10px;
        }
        
        .scc-letters {
            font-size: 18px;
            font-family: monospace;
            margin: 10px 0;
        }
        
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 15px;
            margin: 20px 0;
        }
        
        .stat-card {
            background: white;
            border-radius: 8px;
            padding: 15px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            text-align: center;
        }
        
        .stat-value {
            font-size: 24px;
            font-weight: bold;
            color: #3498db;
        }
        
        .stat-label {
            font-size: 14px;
            color: #7f8c8d;
            margin-top: 5px;
        }

                .graph-section {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 15px;
            border-radius: 8px;
            margin: 25px 0;
            text-align: center;
            font-size: 20px;
            font-weight: bold;
        }
        
        .graph-container {
            width: 100%;
            height: 600px;
            border: 1px solid #ddd;
            border-radius: 8px;
            background: white;
            margin: 20px 0;
            overflow: hidden;
        }
        
        .graph-controls {
            display: flex;
            gap: 10px;
            margin-bottom: 15px;
            flex-wrap: wrap;
        }
        
        .graph-control-btn {
            padding: 8px 15px;
            background: #3498db;
            color: white;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-size: 14px;
            transition: background 0.3s;
        }
        
        .graph-control-btn:hover {
            background: #2980b9;
        }
        
        .graph-control-btn.active {
            background: #2c3e50;
        }
        
        .graph-stats-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
            gap: 15px;
            margin: 20px 0;
        }
        
        .graph-stat-card {
            background: white;
            border-radius: 8px;
            padding: 15px;
            text-align: center;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            border-top: 4px solid #3498db;
        }
        
        .graph-stat-value {
            font-size: 28px;
            font-weight: bold;
            color: #2c3e50;
            margin-bottom: 5px;
        }
        
        .graph-stat-label {
            font-size: 14px;
            color: #7f8c8d;
        }
        
        .scc-components {
            display: flex;
            flex-wrap: wrap;
            gap: 15px;
            margin: 20px 0;
        }
        
        .scc-component {
            background: white;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            padding: 15px;
            min-width: 180px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.05);
        }
        
        .scc-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 10px;
        }
        
        .scc-title {
            font-weight: bold;
            color: #2c3e50;
            font-size: 16px;
        }
        
        .scc-size {
            background: #3498db;
            color: white;
            padding: 3px 8px;
            border-radius: 12px;
            font-size: 12px;
        }
        
        .scc-letters {
            font-size: 18px;
            font-family: monospace;
            text-align: center;
            letter-spacing: 2px;
            margin: 10px 0;
            padding: 10px;
            background: #f8f9fa;
            border-radius: 5px;
        }
        
        .language-detection {
            display: flex;
            gap: 20px;
            margin: 20px 0;
            flex-wrap: wrap;
        }
        
        .language-card {
            flex: 1;
            min-width: 150px;
            background: white;
            border-radius: 8px;
            padding: 15px;
            text-align: center;
            box-shadow: 0 2px 6px rgba(0,0,0,0.1);
        }
        
        .language-card.best {
            border-top: 4px solid #2ecc71;
            background: #f8fff9;
        }
        
        .language-score {
            font-size: 28px;
            font-weight: bold;
            color: #2c3e50;
            margin: 10px 0;
        }
        
        .language-name {
            font-size: 16px;
            color: #7f8c8d;
        }
        
        .score-bar {
            height: 8px;
            background: #ecf0f1;
            border-radius: 4px;
            margin: 10px 0;
            overflow: hidden;
        }
        
        .score-fill {
            height: 100%;
            background: linear-gradient(90deg, #3498db, #2ecc71);
        }
        
        .scc-profile-container {
            width: 100%;
            height: 400px;
            margin: 20px 0;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Advanced Caesar Cipher Analysis with Graph Theory</h1>
        
        <div class="info-box">
            <h2>Analysis Summary</h2>
            <div class="stats">
                <div class="stat">
                    <div class="stat-value">)" << detected_lang << R"(</div>
                    <div class="stat-label">Language</div>
                </div>
                <div class="stat">
                    <div class="stat-value">)" << shift << R"(</div>
                    <div class="stat-label">Shift</div>
                </div>
                <div class="stat">
                    <div class="stat-value">)" << encrypted.length() << R"(</div>
                    <div class="stat-label">Chars Encrypted</div>
                </div>
                <div class="stat">
                    <div class="stat-value">)" << decrypted.length() << R"(</div>
                    <div class="stat-label">Chars Decrypted</div>
                </div>
            </div>
        </div>
        
        <div class="info-box">
            <h2>Text Samples</h2>
            <h3>Encrypted Text:</h3>
            <div class="text-box">)"
        << escape_html(wstring_to_utf8(encrypted)) << R"(</div>
            <h3>Decrypted Text:</h3>
            <div class="text-box">)"
        << escape_html(wstring_to_utf8(decrypted)) << R"(</div>
        </div>
        
        <h2>Letter Frequencies (Before Decryption)</h2>
        <div class="chart-container">
            <canvas id="chartBefore"></canvas>
        </div>
        
        <h2>Letter Frequencies (After Decryption)</h2>
        <div class="chart-container">
            <canvas id="chartAfter"></canvas>
        </div>
        
        <h2>Top Bigram Frequencies</h2>
        <div class="chart-container">
            <canvas id="chartBigrams"></canvas>
        </div>
        
        <script>
        // Prepare data for Before chart
        const beforeData = {
            labels: [)";

    // Подготавливаем данные для первого графика
    std::vector<std::pair<wchar_t, double>> sorted_before(freqs_before.begin(), freqs_before.end());
    std::sort(sorted_before.begin(), sorted_before.end(),
        [](const std::pair<wchar_t, double>& a, const std::pair<wchar_t, double>& b) {
            return a.second > b.second;
        });

    size_t limit = (sorted_before.size() < 15) ? sorted_before.size() : 15;
    for (size_t i = 0; i < limit; ++i) {
        wchar_t ch = sorted_before[i].first;
        std::string utf8_char = wstring_to_utf8(std::wstring(1, ch));
        html << "\"" << escape_html(utf8_char) << "\"";
        if (i < limit - 1) html << ", ";
    }

    html << "],\n";
    html << "datasets: [{\n";
    html << "label: 'Frequency (%)',\n";
    html << "data: [";

    for (size_t i = 0; i < limit; ++i) {
        html << sorted_before[i].second;
        if (i < limit - 1) html << ", ";
    }

    html << "],\n";
    html << "backgroundColor: 'rgba(54, 162, 235, 0.7)',\n";
    html << "borderColor: 'rgba(54, 162, 235, 1)',\n";
    html << "borderWidth: 1\n";
    html << "}]\n";
    html << "};\n\n";

    // Данные для второго графика (After)
    html << "const afterData = {\n";
    html << "labels: [";

    std::vector<std::pair<wchar_t, double>> sorted_after(freqs_after.begin(), freqs_after.end());
    std::sort(sorted_after.begin(), sorted_after.end(),
        [](const std::pair<wchar_t, double>& a, const std::pair<wchar_t, double>& b) {
            return a.second > b.second;
        });

    limit = (sorted_after.size() < 15) ? sorted_after.size() : 15;
    for (size_t i = 0; i < limit; ++i) {
        wchar_t ch = sorted_after[i].first;
        std::string utf8_char = wstring_to_utf8(std::wstring(1, ch));
        html << "\"" << escape_html(utf8_char) << "\"";
        if (i < limit - 1) html << ", ";
    }

    html << "],\n";
    html << "datasets: [{\n";
    html << "label: 'Frequency (%)',\n";
    html << "data: [";

    for (size_t i = 0; i < limit; ++i) {
        html << sorted_after[i].second;
        if (i < limit - 1) html << ", ";
    }

    html << "],\n";
    html << "backgroundColor: 'rgba(75, 192, 192, 0.7)',\n";
    html << "borderColor: 'rgba(75, 192, 192, 1)',\n";
    html << "borderWidth: 1\n";
    html << "}]\n";
    html << "};\n\n";

    // Данные для третьего графика (Bigrams)
    html << "const bigramData = {\n";
    html << "labels: [";

    std::vector<std::pair<std::wstring, double>> sorted_bigrams(bigrams.begin(), bigrams.end());
    std::sort(sorted_bigrams.begin(), sorted_bigrams.end(),
        [](const std::pair<std::wstring, double>& a, const std::pair<std::wstring, double>& b) {
            return a.second > b.second;
        });

    limit = (sorted_bigrams.size() < 10) ? sorted_bigrams.size() : 10;
    for (size_t i = 0; i < limit; ++i) {
        std::string utf8_bigram = wstring_to_utf8(sorted_bigrams[i].first);
        html << "\"" << escape_html(utf8_bigram) << "\"";
        if (i < limit - 1) html << ", ";
    }


    html << "],\n";
    html << "datasets: [{\n";
    html << "label: 'Frequency (%)',\n";
    html << "data: [";

    for (size_t i = 0; i < limit; ++i) {
        html << sorted_bigrams[i].second;
        if (i < limit - 1) html << ", ";
    }

    html << "],\n";
    html << "backgroundColor: 'rgba(255, 159, 64, 0.7)',\n";
    html << "borderColor: 'rgba(255, 159, 64, 1)',\n";
    html << "borderWidth: 1\n";
    html << "}]\n";
    html << "};\n\n";

    html << R"(
        // Chart options
        const chartOptions = {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: {
                    display: true,
                    position: 'top',
                },
                tooltip: {
                    callbacks: {
                        label: function(context) {
                            return context.dataset.label + ': ' + context.parsed.y.toFixed(2) + '%';
                        }
                    }
                }
            },
            scales: {
                y: {
                    beginAtZero: true,
                    title: {
                        display: true,
                        text: 'Frequency (%)'
                    },
                    ticks: {
                        callback: function(value) {
                            return value + '%';
                        }
                    }
                }
            }
        };
        
        // Create charts
        new Chart(document.getElementById('chartBefore').getContext('2d'), {
            type: 'bar',
            data: beforeData,
            options: chartOptions
        });
        
        new Chart(document.getElementById('chartAfter').getContext('2d'), {
            type: 'bar',
            data: afterData,
            options: chartOptions
        });
        
        // Bigram chart (horizontal bars)
        new Chart(document.getElementById('chartBigrams').getContext('2d'), {
            type: 'bar',
            data: bigramData,
            options: {
                indexAxis: 'y',
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: {
                        display: true,
                        position: 'top',
                    },
                    tooltip: {
                        callbacks: {
                            label: function(context) {
                                return context.dataset.label + ': ' + context.parsed.x.toFixed(2) + '%';
                            }
                        }
                    }
                },
                scales: {
                    x: {
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: 'Frequency (%)'
                        },
                        ticks: {
                            callback: function(value) {
                                return value + '%';
                            }
                        }
                    }
                }
            }
        });
        </script>

        <div style="background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); padding: 15px; border-radius: 8px; margin: 25px 0; text-align: center;">
            <h2 style="color: white; margin: 0;">Bigram Graph Analysis and Strongly Connected Components (SCC)</h2>
            <p style="color: rgba(255,255,255,0.9); margin: 5px 0 0 0;">Directed graph of character transitions and structural analysis</p>
        </div>

        <!-- Graph Statistics -->
        <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(180px, 1fr)); gap: 15px; margin: 20px 0;">
            <div style="background: white; border-radius: 8px; padding: 15px; text-align: center; box-shadow: 0 2px 4px rgba(0,0,0,0.1); border-top: 4px solid #3498db;">
                <div style="font-size: 28px; font-weight: bold; color: #2c3e50; margin-bottom: 5px;">)" << graphStats.numVertices << R"(</div>
                <div style="font-size: 14px; color: #7f8c8d;">Vertices (Letters)</div>
            </div>
            <div style="background: white; border-radius: 8px; padding: 15px; text-align: center; box-shadow: 0 2px 4px rgba(0,0,0,0.1); border-top: 4px solid #3498db;">
                <div style="font-size: 28px; font-weight: bold; color: #2c3e50; margin-bottom: 5px;">)" << graphStats.numEdges << R"(</div>
                <div style="font-size: 14px; color: #7f8c8d;">Edges (Bigrams)</div>
            </div>
            <div style="background: white; border-radius: 8px; padding: 15px; text-align: center; box-shadow: 0 2px 4px rgba(0,0,0,0.1); border-top: 4px solid #3498db;">
                <div style="font-size: 28px; font-weight: bold; color: #2c3e50; margin-bottom: 5px;">)" << sccs.size() << R"(</div>
                <div style="font-size: 14px; color: #7f8c8d;">Connected Components</div>
            </div>
            <div style="background: white; border-radius: 8px; padding: 15px; text-align: center; box-shadow: 0 2px 4px rgba(0,0,0,0.1); border-top: 4px solid #3498db;">
                <div style="font-size: 28px; font-weight: bold; color: #2c3e50; margin-bottom: 5px;">)" << std::fixed << std::setprecision(2) << graphStats.averageDegree << R"(</div>
                <div style="font-size: 14px; color: #7f8c8d;">Average Degree</div>
            </div>
            <div style="background: white; border-radius: 8px; padding: 15px; text-align: center; box-shadow: 0 2px 4px rgba(0,0,0,0.1); border-top: 4px solid #3498db;">
                <div style="font-size: 28px; font-weight: bold; color: #2c3e50; margin-bottom: 5px;">)" << std::fixed << std::setprecision(4) << graphStats.density << R"(</div>
                <div style="font-size: 14px; color: #7f8c8d;">Graph Density</div>
            </div>
        </div>

        <!-- Graph Controls -->
        <div style="display: flex; gap: 10px; margin-bottom: 15px; flex-wrap: wrap;">
            <button class="graph-control-btn active" onclick='graphLayout("hierarchical")'>Hierarchical</button>
            <button class="graph-control-btn" onclick='graphLayout("circular")'>Circular</button>
            <button class="graph-control-btn" onclick='toggleWeights(event); return false;'>Hide Weights</button>
            <button class="graph-control-btn" onclick='highlightSCC()'>Highlight SCC</button>
            <button class="graph-control-btn" onclick='resetView()'>Reset View</button>
            
            <label style="margin-left: auto;">
                Frequency threshold: 
                <input type="range" id="freqThreshold" min="0" max="0.05" step="0.001" value="0.001" 
                       oninput='updateGraph()'>
                <span id="thresholdValue">0.001</span>
            </label>
        </div>

        <!-- Graph Container -->
        <div style="width: 100%; height: 600px; border: 1px solid #ddd; border-radius: 8px; background: white; margin: 20px 0; overflow: hidden;" id="bigraphGraph"></div>

        <!-- Strongly Connected Components -->
        <h3>Strongly Connected Components (SCC)</h3>
        <div style="display: flex; flex-wrap: wrap; gap: 15px; margin: 20px 0;">)";

        // Generate SCC components
        int compIndex = 1;
        for (const auto& component : sccs) {
            html << R"(
            <div style="background: white; border: 2px solid #e0e0e0; border-radius: 8px; padding: 15px; min-width: 180px; box-shadow: 0 2px 4px rgba(0,0,0,0.05);">
                <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px;">
                    <div style="font-weight: bold; color: #2c3e50; font-size: 16px;">Component )" << compIndex++ << R"(</div>
                    <div style="background: #3498db; color: white; padding: 3px 8px; border-radius: 12px; font-size: 12px;">)" << component.size() << R"( letters</div>
                </div>
                <div style="font-size: 18px; font-family: monospace; text-align: center; letter-spacing: 2px; margin: 10px 0; padding: 10px; background: #f8f9fa; border-radius: 5px;">)";

            for (wchar_t letter : component) {
                // Используйте wchar_to_label вместо wchar_to_id
                std::string label = wchar_to_label(letter);
                html << escape_html(label) << " ";
            }

            html << R"(</div>
            </div>)";
        }

    html << R"(
        </div>

        <!-- SCC Profile -->
        <h3>SCC Profile Comparison</h3>
        <div class="chart-container">
            <canvas id="sccProfileChart"></canvas>
        </div>

        <!-- Language Detection by SCC -->
        <h3>Language Detection Based on Graph Structure</h3>
        <div style="display: flex; gap: 20px; margin: 20px 0; flex-wrap: wrap;">)";

    // Generate language cards with scores
    for (const auto& score : sccScores) {
        bool isBest = (score.first == sccScores[0].first);
        html << R"(
            <div style="flex: 1; min-width: 150px; background: white; border-radius: 8px; padding: 15px; text-align: center; box-shadow: 0 2px 6px rgba(0,0,0,0.1; border-top: 4px solid #)" << (isBest ? "2ecc71" : "3498db") << R"(; background: #)" << (isBest ? "f8fff9" : "white") << R"(;">
                <div style="font-size: 16px; color: #7f8c8d;">)" << score.second << R"(</div>
                <div style="font-size: 28px; font-weight: bold; color: #2c3e50; margin: 10px 0;">)" << std::fixed << std::setprecision(2) << score.first << R"(</div>
                <div style="height: 8px; background: #ecf0f1; border-radius: 4px; margin: 10px 0; overflow: hidden;">
                    <div style="height: 100%; background: linear-gradient(90deg, #3498db, #2ecc71); width: )" << (score.first * 100) << R"(%"></div>
                </div>
            </div>)";
    }

    html << R"(
        <!-- SCC Analysis Decision -->
        <h3>SCC Graph Analysis Decision</h3>
        <div class="info-box" style="background: #f8f9fa; border-color: #6c757d;">
            <h4>Multi-Stage Language Detection</h4>
            <div class="stats">
                <div class="stat">
                    <div class="stat-value">)" << (decrypted.length() >= 80 ? "YES" : "NO") << R"(</div>
                    <div class="stat-label">SCC Analysis Applied</div>
                </div>
                <div class="stat">
                    <div class="stat-value">)" << (decrypted.length() >= 80 ? std::to_string(GetSCCConfidence(decrypted)) : "N/A") << R"(</div>
                    <div class="stat-label">SCC Confidence</div>
                </div>
                <div class="stat">
                    <div class="stat-value">)" << FinalLanguageDetection(decrypted) << R"(</div>
                    <div class="stat-label">Final Decision</div>
                </div>
            </div>
            <p style="font-size: 12px; color: #666; margin-top: 10px;">
                Note: SCC analysis is applied only for texts greater than or equal to 80 characters.
                It serves as a secondary verification method.
            </p>
        </div>

        <!-- Graph Data and JavaScript -->
        <script>
        // Prepare graph data
        const nodes = [)";

    auto allLetters = graphAnalyzer.getAllLetters();
    bool firstNode = true;
    std::set<std::string> uniqueIds;

    for (wchar_t letter : allLetters) {
        // ID для графа (уникальный)
        std::string nodeId = wchar_to_id(letter);

        // Проверка уникальности
        if (uniqueIds.find(nodeId) != uniqueIds.end()) {
            int suffix = 1;
            while (uniqueIds.find(nodeId + "_" + std::to_string(suffix)) != uniqueIds.end()) {
                suffix++;
            }
            nodeId = nodeId + "_" + std::to_string(suffix);
        }
        uniqueIds.insert(nodeId);

        // LABEL для отображения (красивый символ)
        std::string nodeLabel = wchar_to_label(letter);

        if (!firstNode) html << ",";
        firstNode = false;

        html << R"(
        { 
            id: ')" << escape_js(nodeId) << R"(', 
            label: ')" << escape_js(nodeLabel) << R"('
        })";
    }

    html << R"(];
        
        const edges = [)";

    auto allBigrams = graphAnalyzer.getAllBigramsWithFrequencies();
    bool firstEdge = true;
    int edgeId = 0;

    for (const auto& [bigram, freq] : allBigrams) {
        if (freq > 0.001) {
            std::string fromId = wchar_to_id(bigram[0]);
            std::string toId = wchar_to_id(bigram[1]);

            if (!firstEdge) html << ",";
            firstEdge = false;

            // Нормализация
            double normalizedFreq = freq / 100.0;
            if (normalizedFreq > 1.0) normalizedFreq = 1.0;

            // Толщина
            double edgeWidth = 1.0 + (normalizedFreq * 50.0);
            if (edgeWidth > 8.0) edgeWidth = 8.0;
            if (edgeWidth < 1.0) edgeWidth = 1.0;

            // Форматирование процентов
            char percentBuffer[32];
            snprintf(percentBuffer, sizeof(percentBuffer), "%.2f%%", normalizedFreq * 100.0);

            html << R"(
        { 
            id: )" << edgeId++ << R"(, 
            from: ')" << escape_js(fromId) << R"(', 
            to: ')" << escape_js(toId) << R"(', 
            value: )" << std::fixed << std::setprecision(6) << normalizedFreq << R"(,
            width: )" << edgeWidth << R"(,
            label: ')" << percentBuffer << R"(',
            title: ')" << percentBuffer << R"('
        })";
        }
    }

    // SCC компоненты для раскраски
    html << R"(];
        
        // SCC компоненты для раскраски
        const sccComponents = [)";

    bool firstComp = true;
    for (const auto& component : sccs) {
        if (!firstComp) html << ",";
        firstComp = false;

        html << "[";
        bool firstLetter = true;
        for (wchar_t letter : component) {
            if (!firstLetter) html << ",";
            firstLetter = false;

            // Находим соответствующую вершину
            std::string nodeId = wchar_to_id(letter);

            // Проверяем, есть ли такая вершина в графе
            bool found = false;
            for (wchar_t graphLetter : allLetters) {
                if (graphLetter == letter) {
                    found = true;
                    break;
                }
            }

            if (found) {
                html << "'" << escape_js(nodeId) << "'";
            }
        }
        html << "]";
    }

    html << R"(];
        
        // SCC Profile data
        const sccProfileData = {
            labels: [)";

    // Generate labels for SCC profile
    for (size_t i = 0; i < sccProfile.size(); ++i) {
        if (i > 0) html << ", ";
        html << "\"Component " << (i + 1) << "\"";
    }

    html << R"(],
            datasets: [
                {
                    label: 'Current Text',
                    data: [)";

    // Generate data for SCC profile
    for (size_t i = 0; i < sccProfile.size(); ++i) {
        if (i > 0) html << ", ";
        html << sccProfile[i];
    }

    html << R"(],
                    backgroundColor: 'rgba(54, 162, 235, 0.7)',
                    borderColor: 'rgba(54, 162, 235, 1)',
                    borderWidth: 1
                })";

    // Add reference profiles
    for (size_t i = 0; i < (std::min)(sccScores.size(), size_t(2)); ++i) {
        auto bestRefProfile = referenceSCCProfiles[sccScores[i].second];
        html << R"(,
                {
                    label: ')" << sccScores[i].second << R"( (Score: )"
            << std::fixed << std::setprecision(2) << sccScores[i].first << R"()',
                    data: [)";

        for (size_t j = 0; j < bestRefProfile.size(); ++j) {
            if (j > 0) html << ", ";
            html << bestRefProfile[j];
        }

        html << R"(],
                    backgroundColor: 'rgba(75, 192, 192, 0.3)',
                    borderColor: 'rgba(75, 192, 192, 1)',
                    borderWidth: 1,
                    type: 'line',
                    fill: false,
                    tension: 0.1
                })";
    }

    // Закрываем предыдущий raw string перед началом нового
    html << R"(]
        };

        // ========== ИСПРАВЛЕННЫЙ JAVASCRIPT ДЛЯ ГРАФА ==========
        let network = null;
        let showWeights = true;
        
        function initializeGraph() {
            const container = document.getElementById('bigraphGraph');
            if (!container) {
                console.error('Graph container not found!');
                return;
            }
            
            const data = {
                nodes: new vis.DataSet(nodes),
                edges: new vis.DataSet(edges)
            };
            
            const options = {
                nodes: {
                    shape: 'dot',
                    size: 25,
                    font: {
                        size: 18,
                        color: '#2c3e50',
                        strokeWidth: 2,
                        strokeColor: 'white'
                    },
                    borderWidth: 2,
                    color: {
                        border: '#2c3e50',
                        background: '#3498db',
                        highlight: {
                            border: '#2c3e50',
                            background: '#2ecc71'
                        }
                    }
                },
                edges: {
                    arrows: {
                        to: { enabled: true, scaleFactor: 0.8 }
                    },
                    smooth: {
                        type: 'continuous',
                        roundness: 0.2
                    },
                    font: {
                        size: 12,
                        align: 'middle',
                        strokeWidth: 3,
                        strokeColor: 'white'
                    },
                    color: {
                        color: '#95a5a6',
                        opacity: 0.7,
                        highlight: '#e74c3c'
                    },
                    width: 2
                },
                interaction: {
                    dragNodes: true,
                    zoomView: true,
                    dragView: true,
                    hover: true,
                    tooltipDelay: 200
                },
                physics: {
                    enabled: true,
                    solver: 'forceAtlas2Based',
                    forceAtlas2Based: {
                        gravitationalConstant: -50,
                        centralGravity: 0.01,
                        springLength: 100,
                        springConstant: 0.08,
                        damping: 0.4,
                        avoidOverlap: 1
                    }
                },
                // Правильное использование layout
                layout: {
                    hierarchical: {
                        enabled: false  // по умолчанию выключено
                    }
                }
            };
            
            try {
                network = new vis.Network(container, data, options);
                console.log('Graph created with', nodes.length, 'nodes and', edges.length, 'edges');
            } catch (error) {
                console.error('Error creating graph:', error);
            }
    
            // В конце функции initializeGraph, после создания network:
            console.log('Network created, checking edge values:');
            const initialEdges = network.body.data.edges.get();
            initialEdges.slice(0, 3).forEach(edge => {
                console.log('Initial edge', edge.id, 
                           'value:', edge.value, 
                           'typeof:', typeof edge.value,
                           'label:', edge.label,
                           'title:', edge.title);
            });    
        }
        
        function toggleWeights(e) {
            console.log('=== toggleWeights called, current state:', showWeights);
            
            if (!network) {
                console.error('Graph not initialized');
                return;
            }
            
            if (e && e.preventDefault) {
                e.preventDefault();
                e.stopPropagation();
            }
            
            const btn = e.target;
            showWeights = !showWeights;
            console.log('New state:', showWeights);
            
            btn.textContent = showWeights ? 'Hide Weights' : 'Show Weights';
            
            // Получаем все текущие рёбра из графа
            const currentEdges = network.body.data.edges.get();
            console.log('Processing', currentEdges.length, 'edges');
            
            // Обновляем метки и УБИРАЕМ strokeWidth при скрытии
            currentEdges.forEach(edge => {
                const newLabel = showWeights ? edge.originalLabel || ((edge.value * 100).toFixed(2) + '%') : '';
                
                network.body.data.edges.update({
                    id: edge.id,
                    label: newLabel,
                    font: {
                        size: showWeights ? 12 : 0,
                        strokeWidth: showWeights ? 3 : 0, // Убираем обводку!
                        color: showWeights ? '#666' : 'transparent',
                        strokeColor: showWeights ? 'white' : 'transparent' // Убираем белую обводку!
                    }
                });
            });
            
            // Принудительно обновляем отображение
            network.redraw();
            
            console.log('Weights', showWeights ? 'shown' : 'hidden');
            return false;
        }

        function resetWeights() {
            if (!network) return;
            
            showWeights = true;
            const btn = document.querySelector('.graph-control-btn[onclick*="toggleWeights"]');
            if (btn) btn.textContent = 'Hide Weights';
            
            const currentEdges = network.body.data.edges.get();
            currentEdges.forEach(edge => {
                if (edge.value !== undefined) {
                    network.body.data.edges.update({
                        id: edge.id,
                        label: (edge.value * 100).toFixed(2) + '%'
                    });
                }
            });
        }
        
        function highlightSCC() {
            if (!network) return;
            
            const colors = ['#e74c3c', '#2ecc71', '#3498db', '#f39c12', '#9b59b6', '#1abc9c'];
            
            // Сначала сбрасываем все цвета
            const allNodes = nodes.map(n => n.id);
            allNodes.forEach(nodeId => {
                network.body.data.nodes.update({
                    id: nodeId,
                    color: {
                        background: '#3498db',
                        border: '#2c3e50'
                    }
                });
            });
            
            // Раскрашиваем SCC компоненты
            if (sccComponents && sccComponents.length > 0) {
                sccComponents.forEach((component, compIndex) => {
                    if (!component || component.length === 0) return;
                    
                    const color = colors[compIndex % colors.length];
                    component.forEach(nodeId => {
                        // Проверяем, существует ли вершина
                        const nodeExists = nodes.some(n => n.id === nodeId);
                        if (nodeExists) {
                            network.body.data.nodes.update({
                                id: nodeId,
                                color: {
                                    background: color,
                                    border: '#2c3e50',
                                    highlight: {
                                        background: color,
                                        border: '#2c3e50'
                                    }
                                }
                            });
                        }
                    });
                });
                
                console.log('Highlighted', sccComponents.length, 'SCC components');
            } else {
                alert('No SCC components available or data is corrupted');
            }
        }
        
        function resetView() {
            if (!network) {
                initializeGraph();
                return;
            }
            
             resetWeights();
            
            // Сбрасываем цвета
            nodes.forEach(node => {
                network.body.data.nodes.update({
                    id: node.id,
                    color: {
                        background: '#3498db',
                        border: '#2c3e50',
                        highlight: {
                            background: '#2ecc71',
                            border: '#2c3e50'
                        }
                    }
                });
            });
            
            // Сбрасываем веса
            showWeights = false;
            const weightBtn = document.querySelector('.graph-control-btn[onclick*="toggleWeights"]');
            if (weightBtn) weightBtn.textContent = 'Show Weights';
            
            // Сбрасываем layout
            network.setOptions({ 
                layout: {
                    hierarchical: { enabled: false }
                },
                physics: {
                    enabled: true,
                    solver: 'forceAtlas2Based'
                }
            });
            
            // Сбрасываем фильтр
            document.getElementById('freqThreshold').value = 0.001;
            document.getElementById('thresholdValue').textContent = '0.001';
            updateGraph();
            
            // Сбрасываем активность кнопок
            document.querySelectorAll('.graph-control-btn').forEach(btn => {
                btn.classList.remove('active');
            });
            
            // Сбрасываем вид
            network.fit();
            network.stabilize();
        }
        
        function updateGraph() {
            if (!network) return;
            
            const threshold = parseFloat(document.getElementById('freqThreshold').value);
            document.getElementById('thresholdValue').textContent = threshold.toFixed(3);
            
            console.log('Filtering edges with threshold:', threshold, 
                       'showWeights:', showWeights);
            
            // Восстанавливаем ВСЕ рёбра с правильными labels
            network.body.data.edges.clear();
            
            const filteredEdges = [];
            edges.forEach(edge => {
                if (edge.value >= threshold) {
                    // Копируем edge с правильным label
                    const edgeCopy = {...edge};
                    
                    // Устанавливаем label в зависимости от showWeights
                    if (showWeights) {
                        edgeCopy.label = (edge.value * 100).toFixed(2) + '%';
                    } else {
                        edgeCopy.label = '';
                    }
                    
                    filteredEdges.push(edgeCopy);
                }
            });
            
            if (filteredEdges.length > 0) {
                network.body.data.edges.add(filteredEdges);
                console.log('Showing', filteredEdges.length, 'edges');
            } else {
                console.warn('No edges above threshold');
            }
        }
        
        // Функция для конвертации wchar_t в id (должна быть в C++, но дублируем в JS)
        function wchar_to_id(wc) {
            // Простая реализация - должно совпадать с C++
            return 'char_' + wc.charCodeAt(0);
        }

    )";
    html << R"(
        
        // ========== ФУНКЦИИ КНОПОК ==========
        
        function graphLayout(layoutType) {
            if (!network) return;
            
            // Снимаем активность со всех кнопок layout
            document.querySelectorAll('.graph-control-btn').forEach(btn => {
                if (btn.textContent.includes('Hierarchical') || 
                    btn.textContent.includes('Circular')) {
                    btn.classList.remove('active');
                }
            });
            
            // Активируем нажатую кнопку
            event.target.classList.add('active');
            
            if (layoutType === 'hierarchical') {
                // ИЕРАРХИЧЕСКИЙ
                network.setOptions({ 
                    layout: {
                        hierarchical: {
                            enabled: true,
                            direction: 'UD',
                            sortMethod: 'directed',
                            levelSeparation: 150,
                            nodeSpacing: 100
                        }
                    },
                    physics: false
                });
            } 
            else if (layoutType === 'circular') {
                // КРУГОВОЙ - ПРОСТОЙ РАБОЧИЙ ВАРИАНТ
                network.setOptions({ 
                    layout: {
                        randomSeed: 2,
                        improvedLayout: false,
                        hierarchical: { enabled: false }
                    },
                    physics: false
                });
            }
            
            // Перестраиваем граф
            setTimeout(() => {
                network.fit();
                network.stabilize();
            }, 100);
        }
        
        function resetView() {
            if (!network) {
                initializeGraph();
                return;
            }
            
            // 1. Сбрасываем цвета
            nodes.forEach(node => {
                network.body.data.nodes.update({
                    id: node.id,
                    color: {
                        background: '#3498db',
                        border: '#2c3e50',
                        highlight: {
                            background: '#2ecc71',
                            border: '#2c3e50'
                        }
                    }
                });
            });
            
            // 2. Сбрасываем веса (скрываем)
            const weightBtn = document.querySelector('.graph-control-btn[onclick*="toggleWeights"]');
            if (weightBtn) {
                weightBtn.textContent = 'Show Weights';
                edges.forEach(edge => {
                    network.body.data.edges.update({
                        id: edge.id,
                        label: ''
                    });
                });
            }
            
            // 3. Сбрасываем layout на hierarchical
            network.setOptions({ 
                layout: {
                    hierarchical: {
                        enabled: true,
                        direction: 'UD',
                        sortMethod: 'directed'
                    }
                },
                physics: false
            });
            
            // 4. Сбрасываем активность кнопок
            document.querySelectorAll('.graph-control-btn').forEach(btn => {
                btn.classList.remove('active');
            });
            // Активируем Hierarchical
            const hierBtn = document.querySelector('.graph-control-btn[onclick*="hierarchical"]');
            if (hierBtn) hierBtn.classList.add('active');
            
            // 5. Сбрасываем фильтр
            document.getElementById('freqThreshold').value = 0.001;
            document.getElementById('thresholdValue').textContent = '0.001';
            
            // 6. Показываем все рёбра
            network.body.data.edges.clear();
            network.body.data.edges.add(edges);
            
            // 7. Сбрасываем вид
            setTimeout(() => {
                network.fit();
                network.stabilize();
            }, 100);
            
            console.log('View reset to default');
        }
        
        function updateGraph() {
            if (!network) return;
            
            const threshold = parseFloat(document.getElementById('freqThreshold').value);
            document.getElementById('thresholdValue').textContent = threshold.toFixed(3);
            
            // Фильтруем ребра по порогу
            const filteredEdges = [];
            for (let i = 0; i < edges.length; i++) {
                if (edges[i].value >= threshold) {
                    filteredEdges.push(edges[i]);
                }
            }
            network.body.data.edges.clear();
            network.body.data.edges.add(filteredEdges);
        }
        
        // Инициализация SCC Profile Chart
        new Chart(document.getElementById('sccProfileChart').getContext('2d'), {
            type: 'bar',
            data: sccProfileData,
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: {
                        display: true,
                        position: 'top'
                    },
                    tooltip: {
                        callbacks: {
                            label: function(context) {
                                return context.dataset.label + ': ' + context.parsed.y + ' letters';
                            }
                        }
                    }
                },
                scales: {
                    y: {
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: 'Component Size (letters)'
                        },
                        ticks: {
                            stepSize: 1
                        }
                    }
                }
            }
        });

        document.addEventListener('DOMContentLoaded', function() {
            console.log('DOM loaded, initializing graph...');
            
            // Инициализируем граф
            initializeGraph();
            
            // Устанавливаем обработчик изменения порога
            const thresholdSlider = document.getElementById('freqThreshold');
            if (thresholdSlider) {
                thresholdSlider.addEventListener('input', updateGraph);
                console.log('Threshold slider event listener added');
            } else {
                console.error('Threshold slider not found!');
            }
            
            // Сразу применяем фильтр по умолчанию (через небольшую задержку)
            setTimeout(function() {
                console.log('Applying default threshold filter...');
                updateGraph();
            }, 200);
        });
        </script>)";

    html << R"(        
        <div style="text-align: center; margin-top: 40px; color: #777; font-size: 12px;">
            <p>Report generated by Caesar Cipher Analyzer</p>
            <p>Generated on: )";

    // Вставляем дату
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    localtime_s(&tstruct, &now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &tstruct);
    html << buf;

    // Закрываем HTML
    html << R"(</p>
        </div>
    </div>
</body>
</html>)";

    html.close();

    // Открываем в браузере
    std::wstring wide_filename(filename.begin(), filename.end());
    ShellExecuteW(NULL, L"open", wide_filename.c_str(), NULL, NULL, SW_SHOWNORMAL);
}



void CaesarCipherW::SetupDecryptionMachine(TuringMachineW& tm) const {
    tm.Reset();

    // Сначала определим язык текста
    std::string lang = "ENGLISH";

    // Временно создаем объект для анализа языка
    // Простая проверка по наличию символов
    bool has_russian = false;
    bool has_german = false;
    bool has_french = false;

    // Проверяем наличие специальных символов
    // Эта логика должна соответствовать DetectLanguage
    // Для простоты используем ту же логику что в SmartDecryptW
    // но так как у нас нет доступа к тексту здесь,
    // нужно передавать эту информацию или использовать более простой подход

    // Вместо сложной логики, просто используем только английский алфавит
    // для базовых латинских символов

    // Лямбда-функция для добавления переходов ТОЛЬКО для английского алфавита
    auto addEnglishTransitions = [&]() {
        for (wchar_t c : EN_LOW) {
            wchar_t d = ShiftInAlphabet(EN_LOW, c, shift, false);
            tm.AddTransition("start", c, "decrypt", d, 1);
            tm.AddTransition("decrypt", c, "decrypt", d, 1);
        }
        for (wchar_t c : EN_UP) {
            wchar_t d = ShiftInAlphabet(EN_UP, c, shift, false);
            tm.AddTransition("start", c, "decrypt", d, 1);
            tm.AddTransition("decrypt", c, "decrypt", d, 1);
        }
        };

    // ИЛИ более гибкий подход - использовать все алфавиты но с проверкой
    auto addAllTransitionsWithCheck = [&]() {
        // Только английский алфавит для базовых латинских
        for (wchar_t c : EN_LOW) {
            wchar_t d = ShiftInAlphabet(EN_LOW, c, shift, false);
            tm.AddTransition("start", c, "decrypt", d, 1);
            tm.AddTransition("decrypt", c, "decrypt", d, 1);
        }
        for (wchar_t c : EN_UP) {
            wchar_t d = ShiftInAlphabet(EN_UP, c, shift, false);
            tm.AddTransition("start", c, "decrypt", d, 1);
            tm.AddTransition("decrypt", c, "decrypt", d, 1);
        }

        // НЕ добавляем немецкие и французские алфавиты по умолчанию
        // Они будут добавлены только если текст содержит эти символы
        };

    // Используем только английский алфавит для простоты
    addEnglishTransitions();

    // Пробелы и знаки препинания
    tm.AddTransition("start", L' ', "decrypt", L' ', 1);
    tm.AddTransition("decrypt", L' ', "decrypt", L' ', 1);

    const std::wstring punct = L".,;:!?-()\"'";
    for (wchar_t p : punct) {
        tm.AddTransition("start", p, "decrypt", p, 1);
        tm.AddTransition("decrypt", p, "decrypt", p, 1);
    }

    tm.AddTransition("start", L'\0', "halt", L'\0', 0);
    tm.AddTransition("decrypt", L'\0', "halt", L'\0', 0);
}

void CaesarCipherW::SetupEncryptionMachine(TuringMachineW& tm) const {
    tm.Reset();

    // Аналогично для шифрования - используем только английский алфавит
    auto addEnglishTransitions = [&]() {
        for (wchar_t c : EN_LOW) {
            wchar_t e = ShiftInAlphabet(EN_LOW, c, shift, true);
            tm.AddTransition("start", c, "encrypt", e, 1);
            tm.AddTransition("encrypt", c, "encrypt", e, 1);
        }
        for (wchar_t c : EN_UP) {
            wchar_t e = ShiftInAlphabet(EN_UP, c, shift, true);
            tm.AddTransition("start", c, "encrypt", e, 1);
            tm.AddTransition("encrypt", c, "encrypt", e, 1);
        }
        };

    addEnglishTransitions();

    tm.AddTransition("start", L' ', "encrypt", L' ', 1);
    tm.AddTransition("encrypt", L' ', "encrypt", L' ', 1);

    const std::wstring punct = L".,;:!?-()\"'";
    for (wchar_t p : punct) {
        tm.AddTransition("start", p, "encrypt", p, 1);
        tm.AddTransition("encrypt", p, "encrypt", p, 1);
    }

    tm.AddTransition("start", L'\0', "halt", L'\0', 0);
    tm.AddTransition("encrypt", L'\0', "halt", L'\0', 0);
}

std::vector<int> CaesarCipherW::GetSCCProfile(const std::wstring& text) const {
    auto bigrams = GetBigramFrequencies(text);
    GraphAnalyzer analyzer;
    analyzer.buildGraphFromBigrams(bigrams);
    return analyzer.getSCCProfile();
}

std::string CaesarCipherW::GetGraphAnalysisJSON(const std::wstring& text) const {
    auto bigrams = GetBigramFrequencies(text);
    GraphAnalyzer analyzer;
    analyzer.buildGraphFromBigrams(bigrams);

    auto sccs = analyzer.getStronglyConnectedComponents();
    auto profile = analyzer.getSCCProfile();
    auto stats = analyzer.getGraphStats();

    std::stringstream json;
    json << "{";
    json << "\"vertices\": " << stats.numVertices << ",";
    json << "\"edges\": " << stats.numEdges << ",";
    json << "\"scc_count\": " << stats.numComponents << ",";
    json << "\"average_weight\": " << std::fixed << std::setprecision(3) << stats.averageEdgeWeight << ",";

    // SCC sizes
    json << "\"scc_sizes\": [";
    for (size_t i = 0; i < profile.size(); ++i) {
        if (i > 0) json << ",";
        json << profile[i];
    }
    json << "],";

    // SCC components
    json << "\"scc_components\": [";
    for (size_t i = 0; i < sccs.size(); ++i) {
        if (i > 0) json << ",";
        json << "[";
        for (size_t j = 0; j < sccs[i].size(); ++j) {
            if (j > 0) json << ",";
            json << "\"" << static_cast<char>(sccs[i][j]) << "\"";
        }
        json << "]";
    }
    json << "]";

    json << "}";
    return json.str();
}
