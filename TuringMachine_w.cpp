#include "TuringMachine_w.h"
#include <iostream>
#include <stdexcept>

TuringMachineW::TuringMachineW(wchar_t blankSymbol)
    : tape([blankSymbol](int) { return blankSymbol; }, blankSymbol),
    headPosition(0),
    currentState("start") {}

void TuringMachineW::AddTransition(const std::string& state, wchar_t read,
    const std::string& newState, wchar_t write, int move) {
    if (move != -1 && move != 0 && move != 1) {
        throw std::invalid_argument("Move direction must be -1,0,1");
    }
    transitions[{state, read}] = std::make_tuple(newState, write, move);
}

void TuringMachineW::SetInputW(const std::wstring& input) {
    // reset tape to blank generator
    tape = LazySequence<wchar_t>([this](int) { return L' '; }, L' ');
    for (size_t i = 0; i < input.size(); ++i) {
        tape.Set(static_cast<int>(i), input[i]);
    }
    // mark end
    tape.Set(static_cast<int>(input.size()), L'\0');

    headPosition = 0;
    currentState = "start";
    stateHistory.clear();
    positionHistory.clear();
    stateHistory.push_back(currentState);
    positionHistory.push_back(headPosition);
}

bool TuringMachineW::Step() {
    if (currentState == "halt") return false;

    wchar_t symbol = tape.Get(headPosition);

    // Ключ: состояние + текущий символ
    auto key = std::make_pair(currentState, symbol);
    auto it = transitions.find(key);

    // Если точного перехода нет — ищем wildcard (0)
    if (it == transitions.end()) {
        key.second = 0;
        it = transitions.find(key);
    }

    // Если и wildcard нет — стоп
    if (it == transitions.end()) {
        currentState = "halt";
        return false;
    }

    // Берём переход
    std::string newState = std::get<0>(it->second);
    wchar_t write = std::get<1>(it->second);
    int      move = std::get<2>(it->second);

    // КРИТИЧЕСКИ ВАЖНАЯ СТРОЧКА:
    if (write == 0) {
        write = symbol;                     // ← оставляем символ как есть!
    }

    tape.Set(headPosition, write);
    currentState = newState;
    headPosition += move;

    stateHistory.push_back(currentState);
    positionHistory.push_back(headPosition);

    return true;
}

void TuringMachineW::Run(bool verbose) {
    if (verbose) {
        std::wcout << L"Начальное состояние:" << std::endl;
        Visualize();
        std::wcout << std::endl;
    }

    int step = 0;
    while (Step()) {
        ++step;
        if (verbose) {
            std::wcout << L"Шаг " << step << L":" << std::endl;
            Visualize();
            std::wcout << std::endl;
        }
        if (step > 100000) {
            std::wcerr << L"Превышено max шагов" << std::endl;
            break;
        }
    }

    if (verbose) {
        std::wcout << L"Конечное состояние (шагов: " << step << L"):" << std::endl;
        Visualize();
    }
}

void TuringMachineW::Visualize(int radius) const {
    int start = headPosition - radius;
    int end = headPosition + radius;
    std::wcout << L"Лента: ";
    for (int i = start; i <= end; ++i) {
        wchar_t c;
        try { c = tape.Get(i); }
        catch (...) { c = L' '; }
        wchar_t display = c;
        if (c == L' ') display = L'_';
        if (c == L'\0') display = L'?';
        if (i == headPosition) std::wcout << L"[" << display << L"]";
        else std::wcout << L" " << display << L" ";
    }
    std::wcout << std::endl;
    std::wcout << L"Состояние: '" << std::wstring(currentState.begin(), currentState.end()) << L"', Позиция: " << headPosition << std::endl;
}

void TuringMachineW::PrintFullTape(int start, int end) const {
    std::wcout << L"Лента: ";
    for (int i = start; i <= end; ++i) {
        try { std::wcout << tape.Get(i); }
        catch (...) { std::wcout << L' '; }
    }
    std::wcout << std::endl;
    for (int i = start; i <= end; ++i) {
        if (i == headPosition) std::wcout << L"^";
        else std::wcout << L" ";
    }
    std::wcout << std::endl;
}

std::wstring TuringMachineW::GetResultW(int start, int length) const {
    std::wstring result;
    // find first non-blank
    int firstPos = -1;
    for (int i = start; i < start + length; ++i) {
        wchar_t c;
        try { c = tape.Get(i); }
        catch (...) { c = L' '; }
        if (c != L' ' && c != L'\0') {
            firstPos = i;
            break;
        }
    }
    if (firstPos == -1) return L"";
    for (int i = firstPos; i < start + length; ++i) {
        wchar_t c;
        try { c = tape.Get(i); }
        catch (...) { break; }
        if (c == L'\0') break;
        result.push_back(c);
    }
    return result;
}

void TuringMachineW::Reset() {
    headPosition = 0;
    currentState = "start";
    stateHistory.clear();
    positionHistory.clear();
    stateHistory.push_back(currentState);
    positionHistory.push_back(headPosition);

    // Полностью очищаем и пересоздаем ленту
    wchar_t blank = L' ';
    tape = LazySequence<wchar_t>([blank](int) { return blank; }, blank);
}