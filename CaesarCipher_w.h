#ifndef CAESARCIPHER_W_H
#define CAESARCIPHER_W_H

#include <string>
#include <map>
#include "TuringMachine_w.h"

class CaesarCipherW {
private:
    int shift; // Сдвиг для шифра Цезаря

    // Алфавиты
    static const std::wstring EN_LOW;
    static const std::wstring EN_UP;
    static const std::wstring RU_LOW;
    static const std::wstring RU_UP;
    static const std::wstring DE_LOW;
    static const std::wstring DE_UP;
    static const std::wstring FR_LOW;
    static const std::wstring FR_UP;

    // Эталонные частоты букв
    static const double EN_FREQ[26];
    static const double RU_FREQ[33];
    static const double DE_FREQ[30];
    static const double FR_FREQ[42];

    // Вспомогательные методы
    static int mod_positive(int a, int m); // Это должно быть здесь!
    wchar_t ShiftInAlphabet(const std::wstring& alph, wchar_t ch, int s, bool encrypt) const;
    std::wstring DecryptWithAlphabet(const std::wstring& wtext,
        const std::wstring& low,
        const std::wstring& up) const;
    bool hasCyrillic(const std::wstring& text) const {
        return text.find_first_of(RU_LOW + RU_UP) != std::wstring::npos;
    }

    bool hasGermanUmlauts(const std::wstring& text) const {
        return text.find_first_of(L"äöüßÄÖÜẞ") != std::wstring::npos;
    }

    bool hasFrenchAccents(const std::wstring& text) const {
        return text.find_first_of(L"àâæçéèêëîïôœûùÿÀÂÆÇÉÈÊËÎÏÔŒÛÙŸ") != std::wstring::npos;
    }

public:
    // Конструктор
    CaesarCipherW(int shift = 3);

    // Основные методы шифрования/дешифрования
    std::wstring DecryptW(const std::wstring& wtext) const;
    std::wstring SmartDecryptW(const std::wstring& wtext) const;
    std::wstring EncryptW(const std::wstring& wtext) const;

    // Методы частотного анализа
    std::map<wchar_t, double> GetLetterFrequencies(const std::wstring& text) const;
    std::map<std::wstring, double> GetBigramFrequencies(const std::wstring& text) const;

    // Определение языка
    std::string DetectLanguage(const std::wstring& text) const;
    std::string AnalyzeLanguage(const std::wstring& text) const;

    // Работа с машиной Тьюринга
    void SetupDecryptionMachine(TuringMachineW& tm) const;
    void SetupEncryptionMachine(TuringMachineW& tm) const;

    // Генерация HTML-отчета
    void CreateCompleteAnalysisHTML(const std::wstring& encrypted,
        const std::wstring& decrypted,
        const std::string& detected_lang,
        const std::string& filename) const;

    //void CreateBigramGraphHTML(const std::wstring& text,
    //    const std::string& detected_lang,
    //    const std::string& filename) const;

    // Геттер/сеттер для сдвига
    int getShift() const { return shift; }
    void setShift(int newShift) { shift = newShift; }

    // Добавляем в публичные методы:
    std::vector<int> GetSCCProfile(const std::wstring& text) const;
    std::string GetGraphAnalysisJSON(const std::wstring& text) const;
    void CreateExtendedHTMLReport(const std::wstring& encrypted,
        const std::wstring& decrypted,
        const std::string& detected_lang,
        const std::string& filename) const;
    std::string DetectLanguageBySCC(const std::wstring& text) const;
    double GetSCCConfidence(const std::wstring& text) const;
    std::string FinalLanguageDetection(const std::wstring& text) const;
};

std::string escape_js(const std::string& input);
std::string escape_html(const std::string& input);
std::string wstring_to_utf8(const std::wstring& wstr);

#endif // CAESARCIPHER_W_H



//#ifndef CAESARCIPHER_W_H
//#define CAESARCIPHER_W_H
//
//#include <string>
//#include <map>
//#include "TuringMachine_w.h"
//
//class CaesarCipherW {
//private:
//    int shift; // Сдвиг для шифра Цезаря
//
//    // Алфавиты
//    static const std::wstring EN_LOW;
//    static const std::wstring EN_UP;
//    static const std::wstring RU_LOW;
//    static const std::wstring RU_UP;
//    static const std::wstring DE_LOW;
//    static const std::wstring DE_UP;
//    static const std::wstring FR_LOW;
//    static const std::wstring FR_UP;
//
//    // Эталонные частоты букв
//    static const double EN_FREQ[26];
//    static const double RU_FREQ[33];
//    static const double DE_FREQ[30];
//    static const double FR_FREQ[42];
//
//    // Вспомогательные методы
//    wchar_t ShiftInAlphabet(const std::wstring& alph, wchar_t ch, int s, bool encrypt) const;
//    std::wstring DecryptWithAlphabet(const std::wstring& wtext,
//        const std::wstring& low,
//        const std::wstring& up) const;
//
//public:
//    // Конструктор
//    CaesarCipherW(int shift = 3);
//
//    // Основные методы шифрования/дешифрования
//    std::wstring DecryptW(const std::wstring& wtext) const;
//    std::wstring SmartDecryptW(const std::wstring& wtext) const;
//    std::wstring EncryptW(const std::wstring& wtext) const;
//
//    // Методы частотного анализа
//    std::map<wchar_t, double> GetLetterFrequencies(const std::wstring& text) const;
//    std::map<std::wstring, double> GetBigramFrequencies(const std::wstring& text) const;
//
//    // Определение языка
//    std::string DetectLanguage(const std::wstring& text) const;
//    std::string AnalyzeLanguage(const std::wstring& text) const;
//
//    // Работа с машиной Тьюринга
//    void SetupDecryptionMachine(TuringMachineW& tm) const;
//    void SetupEncryptionMachine(TuringMachineW& tm) const;
//
//    // Генерация HTML-отчета
//    void CreateCompleteAnalysisHTML(const std::wstring& encrypted,
//        const std::wstring& decrypted,
//        const std::string& detected_lang,
//        const std::string& filename) const;
//
//    // Геттер/сеттер для сдвига
//    int getShift() const { return shift; }
//    void setShift(int newShift) { shift = newShift; }
//};
//
//#endif // CAESARCIPHER_W_H