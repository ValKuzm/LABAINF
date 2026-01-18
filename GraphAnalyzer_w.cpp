#include "GraphAnalyzer_w.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <iterator>

// Конструктор
GraphAnalyzer::GraphAnalyzer() {}

// Построение графа из биграмм
void GraphAnalyzer::buildGraphFromBigrams(const std::map<std::wstring, double>& bigrams) {
    // Очищаем предыдущие данные
    adjacencyList.clear();
    vertices.clear();
    bigramFrequencies = bigrams;

    // Собираем все биграммы и их частоты
    for (const auto& [bigram, freq] : bigrams) {
        if (bigram.length() < 2) continue;

        wchar_t from = bigram[0];
        wchar_t to = bigram[1];

        // Добавляем вершины
        vertices.insert(from);
        vertices.insert(to);

        // Добавляем ребро в граф (только если частота > 0)
        if (freq > 0.0) {
            adjacencyList[from][to] = freq;
        }
    }

    // Очищаем результаты предыдущего анализа SCC
    clearSCCData();
}

// Очистка данных SCC
void GraphAnalyzer::clearSCCData() {
    index.clear();
    lowlink.clear();
    while (!tarjanStack.empty()) tarjanStack.pop();
    sccs.clear();
    stackContained.clear();
    currentIndex = 0;
}

// Рекурсивная функция для алгоритма Тарьяна
void GraphAnalyzer::strongConnect(wchar_t v) {
    // Устанавливаем глубину поиска для v
    index[v] = currentIndex;
    lowlink[v] = currentIndex;
    currentIndex++;

    // Помещаем v в стек
    tarjanStack.push(v);
    stackContained.push_back(v);

    // Рассматриваем всех соседей v
    auto it = adjacencyList.find(v);
    if (it != adjacencyList.end()) {
        for (const auto& neighbor : it->second) {
            wchar_t w = neighbor.first;

            // Если w еще не посещен
            if (index.find(w) == index.end()) {
                strongConnect(w);
                lowlink[v] = std::min(lowlink[v], lowlink[w]);
            }
            // Если w находится в стеке (и, следовательно, в текущем SCC)
            else if (std::find(stackContained.begin(), stackContained.end(), w) != stackContained.end()) {
                lowlink[v] = std::min(lowlink[v], index[w]);
            }
        }
    }

    // Если v - корень SCC, выталкиваем стек и формируем компоненту
    if (lowlink[v] == index[v]) {
        std::vector<wchar_t> component;
        wchar_t w;

        do {
            w = tarjanStack.top();
            tarjanStack.pop();

            // Удаляем w из stackContained
            auto pos = std::find(stackContained.begin(), stackContained.end(), w);
            if (pos != stackContained.end()) {
                stackContained.erase(pos);
            }

            component.push_back(w);
        } while (w != v);

        // Сортируем буквы в компоненте для удобства
        std::sort(component.begin(), component.end());
        sccs.push_back(component);
    }
}

// Поиск компонент сильной связности (алгоритм Тарьяна)
std::vector<std::vector<wchar_t>> GraphAnalyzer::getStronglyConnectedComponents() {
    if (!sccs.empty()) {
        return sccs; // Уже вычислено
    }

    // Сбрасываем данные
    clearSCCData();

    // Запускаем DFS для каждой непосещенной вершины
    for (wchar_t v : vertices) {
        if (index.find(v) == index.end()) {
            strongConnect(v);
        }
    }

    // Сортируем компоненты по размеру (от большего к меньшему)
    std::sort(sccs.begin(), sccs.end(),
        [](const std::vector<wchar_t>& a, const std::vector<wchar_t>& b) {
            return a.size() > b.size();
        });

    return sccs;
}

// Получение профиля SCC (размеры компонент)
std::vector<int> GraphAnalyzer::getSCCProfile() {
    auto components = getStronglyConnectedComponents();
    std::vector<int> profile;

    for (const auto& component : components) {
        profile.push_back(static_cast<int>(component.size()));
    }

    return profile;
}

// Получение статистики графа
GraphStats GraphAnalyzer::getGraphStats() const {
    GraphStats stats;
    stats.numVertices = static_cast<int>(vertices.size());
    stats.numEdges = 0;
    double totalWeight = 0.0;  // <-- ДОБАВЬТЕ ЭТУ ПЕРЕМЕННУЮ

    // Считаем общее количество ребер и суммарный вес
    for (const auto& [from, neighbors] : adjacencyList) {
        stats.numEdges += static_cast<int>(neighbors.size());
        for (const auto& [to, weight] : neighbors) {
            totalWeight += weight;
        }
    }

    // Средний вес ребра (ДОБАВЬТЕ ЭТОТ РАСЧЕТ)
    if (stats.numEdges > 0) {
        stats.averageEdgeWeight = totalWeight / stats.numEdges;
    }

    // Средняя степень вершины
    if (stats.numVertices > 0) {
        stats.averageDegree = static_cast<double>(stats.numEdges) / stats.numVertices;
    }

    // Плотность графа
    if (stats.numVertices > 1) {
        double maxEdges = stats.numVertices * (stats.numVertices - 1);
        if (maxEdges > 0) {
            stats.density = static_cast<double>(stats.numEdges) / maxEdges;
        }
    }

    // Получаем SCC для статистики
    auto tempThis = const_cast<GraphAnalyzer*>(this);
    auto components = tempThis->getStronglyConnectedComponents();
    stats.numComponents = static_cast<int>(components.size());

    if (!components.empty()) {
        stats.maxComponentSize = static_cast<int>(components[0].size());
    }

    return stats;
}

// Получение всех букв (вершин графа)
std::set<wchar_t> GraphAnalyzer::getAllLetters() const {
    return vertices;
}

// Получение всех биграмм с частотами
std::map<std::wstring, double> GraphAnalyzer::getAllBigramsWithFrequencies() const {
    return bigramFrequencies;
}

// Получение всех ребер графа (биграмм) с частотами для визуализации
std::vector<std::tuple<wchar_t, wchar_t, double>> GraphAnalyzer::getAllEdgesWithWeights() const {
    std::vector<std::tuple<wchar_t, wchar_t, double>> edges;

    for (const auto& [from, neighbors] : adjacencyList) {
        for (const auto& [to, weight] : neighbors) {
            edges.emplace_back(from, to, weight);
        }
    }

    // Сортируем по весу (частоте) в порядке убывания
    std::sort(edges.begin(), edges.end(),
        [](const std::tuple<wchar_t, wchar_t, double>& a,
            const std::tuple<wchar_t, wchar_t, double>& b) {
                return std::get<2>(a) > std::get<2>(b);
        });

    return edges;
}

double GraphAnalyzer::compareSCCProfiles(const std::vector<int>& profile1,
    const std::vector<int>& profile2) {
    if (profile1.empty() || profile2.empty()) return 0.0;

    // 1. Нормализуем профили (делаем сумму = 1)
    double sum1 = 0.0, sum2 = 0.0;
    for (int val : profile1) sum1 += val;
    for (int val : profile2) sum2 += val;

    // Проверка деления на ноль
    if (sum1 == 0.0 || sum2 == 0.0) return 0.0;

    std::vector<double> norm1, norm2;
    for (int val : profile1) norm1.push_back(val / sum1);
    for (int val : profile2) norm2.push_back(val / sum2);

    // 2. Доводим до одинаковой длины
    size_t max_len = std::max(norm1.size(), norm2.size());
    std::vector<double> v1(max_len, 0.0), v2(max_len, 0.0);

    for (size_t i = 0; i < max_len; ++i) {
        v1[i] = (i < norm1.size()) ? norm1[i] : 0.0;
        v2[i] = (i < norm2.size()) ? norm2[i] : 0.0;
    }

    // 3. Взвешенное сравнение с экспоненциальным затуханием
    double weighted_dot = 0.0;
    double norm1_sq = 0.0, norm2_sq = 0.0;

    for (size_t i = 0; i < max_len; ++i) {
        // ИСПРАВЛЕНИЕ: приводим i к знаковому типу перед унарным минусом
        double weight = std::exp(-static_cast<int>(i) * 0.5);
        // ИЛИ альтернатива:
        // double weight = std::exp(-static_cast<double>(i) * 0.5);

        weighted_dot += weight * v1[i] * v2[i];
        norm1_sq += weight * v1[i] * v1[i];
        norm2_sq += weight * v2[i] * v2[i];
    }

    if (norm1_sq == 0.0 || norm2_sq == 0.0) return 0.0;
    return weighted_dot / (std::sqrt(norm1_sq) * std::sqrt(norm2_sq));
}

// Получение матрицы смежности для отладки
std::string GraphAnalyzer::getAdjacencyMatrix() const {
    if (vertices.empty()) {
        return "Graph is empty";
    }

    // Сортируем вершины для единообразия
    std::vector<wchar_t> sortedVertices(vertices.begin(), vertices.end());
    std::sort(sortedVertices.begin(), sortedVertices.end());

    std::stringstream ss;
    ss << "Adjacency Matrix:\n";
    ss << "    ";

    // Заголовок строк
    for (const auto& vertex : sortedVertices) {
        ss << vertex << "   ";
    }
    ss << "\n";

    // Сама матрица
    for (const auto& from : sortedVertices) {
        ss << from << ": ";

        for (const auto& to : sortedVertices) {
            auto fromIt = adjacencyList.find(from);
            if (fromIt != adjacencyList.end()) {
                auto toIt = fromIt->second.find(to);
                if (toIt != fromIt->second.end()) {
                    ss << std::fixed << std::setprecision(3) << toIt->second << " ";
                }
                else {
                    ss << "0    ";
                }
            }
            else {
                ss << "0    ";
            }
        }
        ss << "\n";
    }

    return ss.str();
}

// Вывод информации о графе
void GraphAnalyzer::printGraphInfo() const {
    auto stats = getGraphStats();
    std::wcout << L"=== Graph Information ===\n";
    std::wcout << L"Vertices: " << stats.numVertices << L"\n";
    std::wcout << L"Edges: " << stats.numEdges << L"\n";
    std::wcout << L"Average degree: " << stats.averageDegree << L"\n";
    std::wcout << L"Density: " << stats.density << L"\n";

    auto components = const_cast<GraphAnalyzer*>(this)->getStronglyConnectedComponents();
    std::wcout << L"Strongly Connected Components: " << components.size() << L"\n";

    for (size_t i = 0; i < components.size(); ++i) {
        std::wcout << L"Component " << i + 1 << L" (" << components[i].size() << L" letters): ";
        for (wchar_t letter : components[i]) {
            std::wcout << letter << L" ";
        }
        std::wcout << L"\n";
    }
}

// Вспомогательный метод для конвертации wchar_t в строку
std::string GraphAnalyzer::wcharToString(wchar_t wc) {
    // Простая конвертация для ASCII символов
    if (wc < 128) {
        return std::string(1, static_cast<char>(wc));
    }

    // Для не-ASCII символов используем UTF-8 конвертацию
    std::wstring ws(1, wc);
    std::string result;

    if (wc < 0x80) {
        result.push_back(static_cast<char>(wc));
    }
    else if (wc < 0x800) {
        result.push_back(static_cast<char>(0xC0 | (wc >> 6)));
        result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
    }
    else if (wc < 0x10000) {
        result.push_back(static_cast<char>(0xE0 | (wc >> 12)));
        result.push_back(static_cast<char>(0x80 | ((wc >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
    }

    return result;
}

// Вспомогательный метод для конвертации wstring в UTF-8
std::string GraphAnalyzer::wstringToUtf8(const std::wstring& wstr) {
    std::string result;

    for (wchar_t wc : wstr) {
        if (wc < 0x80) {
            // ASCII символы
            result.push_back(static_cast<char>(wc));
        }
        else if (wc < 0x800) {
            // 2-byte UTF-8
            result.push_back(static_cast<char>(0xC0 | (wc >> 6)));
            result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
        }
        else if (wc < 0x10000) {
            // 3-byte UTF-8
            result.push_back(static_cast<char>(0xE0 | (wc >> 12)));
            result.push_back(static_cast<char>(0x80 | ((wc >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
        }
        else {
            // 4-byte UTF-8 (редко для wchar_t)
            result.push_back(static_cast<char>(0xF0 | (wc >> 18)));
            result.push_back(static_cast<char>(0x80 | ((wc >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((wc >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
        }
    }

    return result;
}

