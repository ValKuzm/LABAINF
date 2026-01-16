#pragma once
#ifndef TURINGMACHINE_W_H
#define TURINGMACHINE_W_H

#include "LazySequence.h"
#include <map>
#include <tuple>
#include <string>
#include <vector>

class TuringMachineW {
private:
    LazySequence<wchar_t> tape;
    int headPosition;
    std::string currentState;
    std::map<std::pair<std::string, wchar_t>, std::tuple<std::string, wchar_t, int>> transitions;
    std::vector<std::string> stateHistory;
    std::vector<int> positionHistory;

public:
    TuringMachineW(wchar_t blankSymbol = L' ');

    void AddTransition(const std::string& state, wchar_t read, const std::string& newState, wchar_t write, int move);
    void SetInputW(const std::wstring& input);
    bool Step();
    void Run(bool verbose = false);
    void Reset();

    void Visualize(int radius = 8) const;
    void PrintFullTape(int start = -40, int end = 40) const;

    std::wstring GetResultW(int start = 0, int length = 1000) const;

    // helpers
    const std::vector<std::string>& GetStateHistory() const { return stateHistory; }
    const std::vector<int>& GetPositionHistory() const { return positionHistory; }
    int GetHeadPosition() const { return headPosition; }
    std::string GetCurrentState() const { return currentState; }
};

#endif // TURINGMACHINE_W_H
