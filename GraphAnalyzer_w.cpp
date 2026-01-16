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




//#include "GraphAnalyzer_w.h"
//#include <algorithm>
//#include <cmath>
//#include <stack>
//#include <limits>
//#include <sstream>
//#include <iomanip>
//#include <tuple>
//
//GraphAnalyzer::GraphAnalyzer() {}
//
//void GraphAnalyzer::buildGraphFromBigrams(const std::map<std::wstring, double>& bigrams) {
//    adjacencyMatrix.clear();
//    sccComponents.clear();
//    bigramData = bigrams;
//
//    // Заполняем матрицу смежности
//    for (const auto& bigram : bigrams) {
//        if (bigram.first.length() == 2) {
//            wchar_t from = bigram.first[0];
//            wchar_t to = bigram.first[1];
//            double weight = bigram.second;
//
//            adjacencyMatrix[from][to] = weight;
//        }
//    }
//
//    // Находим компоненты сильной связности
//    tarjanSCC();
//}
//
//std::vector<std::vector<wchar_t>> GraphAnalyzer::getStronglyConnectedComponents() const {
//    return sccComponents;
//}
//
//std::vector<int> GraphAnalyzer::getSCCProfile() const {
//    std::vector<int> profile;
//    for (const auto& component : sccComponents) {
//        profile.push_back(static_cast<int>(component.size()));
//    }
//    // Сортируем по убыванию
//    std::sort(profile.rbegin(), profile.rend());
//    return profile;
//}
//
//GraphStats GraphAnalyzer::getGraphStats() const {
//    GraphStats stats;
//    stats.vertices = static_cast<int>(adjacencyMatrix.size());
//
//    // Подсчитываем ребра и веса
//    stats.edges = 0;
//    double totalWeight = 0.0;
//    stats.maxEdgeWeight = 0.0;
//    stats.minEdgeWeight = std::numeric_limits<double>::max();
//
//    for (const auto& row : adjacencyMatrix) {
//        stats.edges += static_cast<int>(row.second.size());
//        for (const auto& edge : row.second) {
//            totalWeight += edge.second;
//            if (edge.second > stats.maxEdgeWeight) {
//                stats.maxEdgeWeight = edge.second;
//            }
//            if (edge.second < stats.minEdgeWeight) {
//                stats.minEdgeWeight = edge.second;
//            }
//        }
//    }
//
//    if (stats.minEdgeWeight == std::numeric_limits<double>::max()) {
//        stats.minEdgeWeight = 0.0;
//    }
//
//    stats.sccCount = static_cast<int>(sccComponents.size());
//    stats.averageEdgeWeight = (stats.edges > 0) ? totalWeight / stats.edges : 0.0;
//
//    // Вычисляем плотность графа (для ориентированного графа)
//    if (stats.vertices > 1) {
//        int maxPossibleEdges = stats.vertices * (stats.vertices - 1);
//        stats.graphDensity = (maxPossibleEdges > 0) ?
//            static_cast<double>(stats.edges) / maxPossibleEdges : 0.0;
//    }
//    else {
//        stats.graphDensity = 0.0;
//    }
//
//    return stats;
//}
//
//// НОВЫЕ МЕТОДЫ РЕАЛИЗАЦИИ:
//
//std::vector<wchar_t> GraphAnalyzer::getVertices() const {
//    std::vector<wchar_t> vertices;
//    for (const auto& vertex : adjacencyMatrix) {
//        vertices.push_back(vertex.first);
//    }
//    return vertices;
//}
//
//std::vector<std::tuple<wchar_t, wchar_t, double>> GraphAnalyzer::getEdges() const {
//    std::vector<std::tuple<wchar_t, wchar_t, double>> edges;
//
//    for (const auto& row : adjacencyMatrix) {
//        wchar_t from = row.first;
//        for (const auto& edge : row.second) {
//            edges.emplace_back(from, edge.first, edge.second);
//        }
//    }
//
//    return edges;
//}
//
//std::vector<std::pair<std::wstring, double>> GraphAnalyzer::getTopBigrams(int n) const {
//    std::vector<std::pair<std::wstring, double>> topBigrams;
//
//    // Копируем и сортируем биграммы по частоте
//    std::vector<std::pair<std::wstring, double>> sortedBigrams(
//        bigramData.begin(), bigramData.end());
//
//    std::sort(sortedBigrams.begin(), sortedBigrams.end(),
//        [](const auto& a, const auto& b) {
//            return a.second > b.second;
//        });
//
//    // Берем топ N
//    size_t limit = std::min(static_cast<size_t>(n), sortedBigrams.size());
//    for (size_t i = 0; i < limit; ++i) {
//        topBigrams.push_back(sortedBigrams[i]);
//    }
//
//    return topBigrams;
//}
//
//std::string GraphAnalyzer::toJSON() const {
//    std::stringstream json;
//    json << "{\n";
//
//    // Вершины
//    json << "  \"nodes\": [\n";
//    auto vertices = getVertices();
//    for (size_t i = 0; i < vertices.size(); ++i) {
//        json << "    { \"id\": \"" << static_cast<char>(vertices[i])
//            << "\", \"label\": \"" << static_cast<char>(vertices[i]) << "\" }";
//        if (i < vertices.size() - 1) json << ",";
//        json << "\n";
//    }
//    json << "  ],\n";
//
//    // Ребра
//    json << "  \"edges\": [\n";
//    auto edges = getEdges();
//    for (size_t i = 0; i < edges.size(); ++i) {
//        wchar_t from = std::get<0>(edges[i]);
//        wchar_t to = std::get<1>(edges[i]);
//        double weight = std::get<2>(edges[i]);
//
//        json << "    { \"from\": \"" << static_cast<char>(from)
//            << "\", \"to\": \"" << static_cast<char>(to)
//            << "\", \"weight\": " << std::fixed << std::setprecision(3) << weight << " }";
//        if (i < edges.size() - 1) json << ",";
//        json << "\n";
//    }
//    json << "  ],\n";
//
//    // Статистика
//    auto stats = getGraphStats();
//    json << "  \"statistics\": {\n";
//    json << "    \"vertices\": " << stats.vertices << ",\n";
//    json << "    \"edges\": " << stats.edges << ",\n";
//    json << "    \"sccCount\": " << stats.sccCount << ",\n";
//    json << "    \"averageEdgeWeight\": " << std::fixed << std::setprecision(3)
//        << stats.averageEdgeWeight << ",\n";
//    json << "    \"graphDensity\": " << std::fixed << std::setprecision(3)
//        << stats.graphDensity << ",\n";
//    json << "    \"maxEdgeWeight\": " << std::fixed << std::setprecision(3)
//        << stats.maxEdgeWeight << ",\n";
//    json << "    \"minEdgeWeight\": " << std::fixed << std::setprecision(3)
//        << stats.minEdgeWeight << "\n";
//    json << "  }\n";
//
//    json << "}";
//    return json.str();
//}
//
//std::string GraphAnalyzer::toDOT() const {
//    std::stringstream dot;
//    dot << "digraph BigramGraph {\n";
//    dot << "    rankdir=LR;\n";
//    dot << "    node [shape=circle, style=filled, fillcolor=\"lightblue\"];\n";
//    dot << "    edge [fontsize=10];\n\n";
//
//    // Добавляем вершины
//    for (const auto& vertex : adjacencyMatrix) {
//        dot << "    \"" << static_cast<char>(vertex.first) << "\";\n";
//    }
//
//    dot << "\n";
//
//    // Добавляем ребра
//    for (const auto& row : adjacencyMatrix) {
//        wchar_t from = row.first;
//        for (const auto& edge : row.second) {
//            wchar_t to = edge.first;
//            double weight = edge.second;
//
//            // Определяем толщину линии в зависимости от веса
//            std::string penwidth = "1.0";
//            if (weight > 5.0) penwidth = "3.0";
//            else if (weight > 2.0) penwidth = "2.0";
//            else if (weight > 1.0) penwidth = "1.5";
//
//            dot << "    \"" << static_cast<char>(from)
//                << "\" -> \"" << static_cast<char>(to)
//                << "\" [label=\"" << std::fixed << std::setprecision(2) << weight << "%\", "
//                << "penwidth=" << penwidth << "];\n";
//        }
//    }
//
//    dot << "}\n";
//    return dot.str();
//}
//
//std::map<wchar_t, double> GraphAnalyzer::getVertexCentrality() const {
//    std::map<wchar_t, double> centrality;
//
//    for (const auto& vertex : adjacencyMatrix) {
//        wchar_t v = vertex.first;
//        double degree = 0.0;
//
//        // Исходящая степень (out-degree)
//        for (const auto& edge : vertex.second) {
//            degree += edge.second;
//        }
//
//        // Входящая степень (in-degree)
//        for (const auto& row : adjacencyMatrix) {
//            if (row.first != v && row.second.find(v) != row.second.end()) {
//                degree += row.second.at(v);
//            }
//        }
//
//        centrality[v] = degree;
//    }
//
//    return centrality;
//}
//
//void GraphAnalyzer::clear() {
//    adjacencyMatrix.clear();
//    sccComponents.clear();
//    bigramData.clear();
//}
//
//// Существующие методы (оставляем без изменений):
//
//// Реализация алгоритма Тарьяна
//void GraphAnalyzer::tarjanSCC() {
//    std::map<wchar_t, int> index;
//    std::map<wchar_t, int> lowlink;
//    std::vector<wchar_t> stack;
//    std::map<wchar_t, bool> onStack;
//    int idx = 0;
//
//    // Инициализация
//    for (const auto& vertex : adjacencyMatrix) {
//        wchar_t v = vertex.first;
//        index[v] = -1;
//        lowlink[v] = -1;
//        onStack[v] = false;
//    }
//
//    // Запуск DFS для каждой вершины
//    for (const auto& vertex : adjacencyMatrix) {
//        wchar_t v = vertex.first;
//        if (index[v] == -1) {
//            strongConnect(v, index, lowlink, stack, onStack, idx);
//        }
//    }
//}
//
//void GraphAnalyzer::strongConnect(wchar_t v,
//    std::map<wchar_t, int>& index,
//    std::map<wchar_t, int>& lowlink,
//    std::vector<wchar_t>& stack,
//    std::map<wchar_t, bool>& onStack,
//    int& idx) {
//    index[v] = idx;
//    lowlink[v] = idx;
//    idx++;
//    stack.push_back(v);
//    onStack[v] = true;
//
//    // Рассматриваем соседей
//    if (adjacencyMatrix.find(v) != adjacencyMatrix.end()) {
//        for (const auto& neighbor : adjacencyMatrix.at(v)) {
//            wchar_t w = neighbor.first;
//
//            if (index[w] == -1) {
//                // w еще не посещена
//                strongConnect(w, index, lowlink, stack, onStack, idx);
//                lowlink[v] = std::min(lowlink[v], lowlink[w]);
//            }
//            else if (onStack[w]) {
//                // w в стеке - обратное ребро
//                lowlink[v] = std::min(lowlink[v], index[w]);
//            }
//        }
//    }
//
//    // Если v - корень компоненты
//    if (lowlink[v] == index[v]) {
//        std::vector<wchar_t> component;
//        wchar_t w;
//
//        do {
//            w = stack.back();
//            stack.pop_back();
//            onStack[w] = false;
//            component.push_back(w);
//        } while (w != v);
//
//        sccComponents.push_back(component);
//    }
//}
//
//// Сравнение профилей SCC (косминусная мера)
//double GraphAnalyzer::compareSCCProfiles(const std::vector<int>& profile1,
//    const std::vector<int>& profile2) {
//    // Нормализуем профили до одинаковой длины
//    size_t maxSize = std::max(profile1.size(), profile2.size());
//    std::vector<double> v1(maxSize, 0.0);
//    std::vector<double> v2(maxSize, 0.0);
//
//    for (size_t i = 0; i < profile1.size(); ++i) {
//        v1[i] = static_cast<double>(profile1[i]);
//    }
//
//    for (size_t i = 0; i < profile2.size(); ++i) {
//        v2[i] = static_cast<double>(profile2[i]);
//    }
//
//    // Вычисляем косинусное сходство
//    double dot = 0.0;
//    double norm1 = 0.0;
//    double norm2 = 0.0;
//
//    for (size_t i = 0; i < maxSize; ++i) {
//        dot += v1[i] * v2[i];
//        norm1 += v1[i] * v1[i];
//        norm2 += v2[i] * v2[i];
//    }
//
//    if (norm1 == 0.0 || norm2 == 0.0) {
//        return 0.0;
//    }
//
//    return dot / (sqrt(norm1) * sqrt(norm2));
//}



//#include "GraphAnalyzer_w.h"
//#include <algorithm>
//#include <cmath>
//#include <stack>
//#include <limits>
//
//GraphAnalyzer::GraphAnalyzer() {}
//
//void GraphAnalyzer::buildGraphFromBigrams(const std::map<std::wstring, double>& bigrams) {
//    adjacencyMatrix.clear();
//    sccComponents.clear();
//
//    // Заполняем матрицу смежности
//    for (const auto& bigram : bigrams) {
//        if (bigram.first.length() == 2) {
//            wchar_t from = bigram.first[0];
//            wchar_t to = bigram.first[1];
//            double weight = bigram.second;
//
//            adjacencyMatrix[from][to] = weight;
//        }
//    }
//
//    // Находим компоненты сильной связности
//    tarjanSCC();
//}
//
//std::vector<std::vector<wchar_t>> GraphAnalyzer::getStronglyConnectedComponents() const {
//    return sccComponents;
//}
//
//std::vector<int> GraphAnalyzer::getSCCProfile() const {
//    std::vector<int> profile;
//    for (const auto& component : sccComponents) {
//        profile.push_back(static_cast<int>(component.size()));
//    }
//    // Сортируем по убыванию
//    std::sort(profile.rbegin(), profile.rend());
//    return profile;
//}
//
//GraphStats GraphAnalyzer::getGraphStats() const {
//    GraphStats stats;
//    stats.vertices = static_cast<int>(adjacencyMatrix.size());
//
//    // Подсчитываем ребра
//    stats.edges = 0;
//    double totalWeight = 0.0;
//
//    for (const auto& row : adjacencyMatrix) {
//        stats.edges += static_cast<int>(row.second.size());
//        for (const auto& edge : row.second) {
//            totalWeight += edge.second;
//        }
//    }
//
//    stats.sccCount = static_cast<int>(sccComponents.size());
//    stats.averageEdgeWeight = (stats.edges > 0) ? totalWeight / stats.edges : 0.0;
//
//    return stats;
//}
//
//// Реализация алгоритма Тарьяна
//void GraphAnalyzer::tarjanSCC() {
//    std::map<wchar_t, int> index;
//    std::map<wchar_t, int> lowlink;
//    std::vector<wchar_t> stack;
//    std::map<wchar_t, bool> onStack;
//    int idx = 0;
//
//    // Инициализация
//    for (const auto& vertex : adjacencyMatrix) {
//        wchar_t v = vertex.first;
//        index[v] = -1;
//        lowlink[v] = -1;
//        onStack[v] = false;
//    }
//
//    // Запуск DFS для каждой вершины
//    for (const auto& vertex : adjacencyMatrix) {
//        wchar_t v = vertex.first;
//        if (index[v] == -1) {
//            strongConnect(v, index, lowlink, stack, onStack, idx);
//        }
//    }
//}
//
//void GraphAnalyzer::strongConnect(wchar_t v,
//    std::map<wchar_t, int>& index,
//    std::map<wchar_t, int>& lowlink,
//    std::vector<wchar_t>& stack,
//    std::map<wchar_t, bool>& onStack,
//    int& idx) {
//    index[v] = idx;
//    lowlink[v] = idx;
//    idx++;
//    stack.push_back(v);
//    onStack[v] = true;
//
//    // Рассматриваем соседей
//    if (adjacencyMatrix.find(v) != adjacencyMatrix.end()) {
//        for (const auto& neighbor : adjacencyMatrix.at(v)) {
//            wchar_t w = neighbor.first;
//
//            if (index[w] == -1) {
//                // w еще не посещена
//                strongConnect(w, index, lowlink, stack, onStack, idx);
//                lowlink[v] = std::min(lowlink[v], lowlink[w]);
//            }
//            else if (onStack[w]) {
//                // w в стеке - обратное ребро
//                lowlink[v] = std::min(lowlink[v], index[w]);
//            }
//        }
//    }
//
//    // Если v - корень компоненты
//    if (lowlink[v] == index[v]) {
//        std::vector<wchar_t> component;
//        wchar_t w;
//
//        do {
//            w = stack.back();
//            stack.pop_back();
//            onStack[w] = false;
//            component.push_back(w);
//        } while (w != v);
//
//        sccComponents.push_back(component);
//    }
//}
//
//// Сравнение профилей SCC (косминусная мера)
//double GraphAnalyzer::compareSCCProfiles(const std::vector<int>& profile1,
//    const std::vector<int>& profile2) {
//    // Нормализуем профили до одинаковой длины
//    size_t maxSize = std::max(profile1.size(), profile2.size());
//    std::vector<double> v1(maxSize, 0.0);
//    std::vector<double> v2(maxSize, 0.0);
//
//    for (size_t i = 0; i < profile1.size(); ++i) {
//        v1[i] = static_cast<double>(profile1[i]);
//    }
//
//    for (size_t i = 0; i < profile2.size(); ++i) {
//        v2[i] = static_cast<double>(profile2[i]);
//    }
//
//    // Вычисляем косинусное сходство
//    double dot = 0.0;
//    double norm1 = 0.0;
//    double norm2 = 0.0;
//
//    for (size_t i = 0; i < maxSize; ++i) {
//        dot += v1[i] * v2[i];
//        norm1 += v1[i] * v1[i];
//        norm2 += v2[i] * v2[i];
//    }
//
//    if (norm1 == 0.0 || norm2 == 0.0) {
//        return 0.0;
//    }
//
//    return dot / (sqrt(norm1) * sqrt(norm2));
//}




//#include "GraphAnalyzer_w.h"
//#include <algorithm>
//#include <numeric>
//#include <sstream>
//#include <iomanip>
//
//GraphAnalyzer::GraphAnalyzer() : currentIndex(0) {}
//
//void GraphAnalyzer::buildGraphFromBigrams(const std::map<std::wstring, double>& bigrams) {
//    // Очищаем предыдущий граф
//    adjacencyList.clear();
//    vertices.clear();
//    SCCs.clear();
//
//    std::set<wchar_t> uniqueVertices;
//
//    // Строим граф из биграмм
//    for (const auto& bigram : bigrams) {
//        const std::wstring& bg = bigram.first;
//        double weight = bigram.second;
//
//        if (bg.length() == 2) {
//            wchar_t from = bg[0];
//            wchar_t to = bg[1];
//
//            // Добавляем вершины
//            uniqueVertices.insert(from);
//            uniqueVertices.insert(to);
//
//            // Добавляем ребро с весом
//            adjacencyList[from][to] = weight;
//        }
//    }
//
//    // Сохраняем вершины
//    vertices.assign(uniqueVertices.begin(), uniqueVertices.end());
//
//    // Находим компоненты сильной связности
//    findSCCs();
//}
//
//// Алгоритм Тарьяна для поиска КСС
//void GraphAnalyzer::tarjanDFS(wchar_t v) {
//    index[v] = currentIndex;
//    lowlink[v] = currentIndex;
//    currentIndex++;
//    S.push(v);
//    onStack[v] = true;
//
//    // Обрабатываем соседей
//    if (adjacencyList.find(v) != adjacencyList.end()) {
//        for (const auto& neighbor : adjacencyList.at(v)) {
//            wchar_t w = neighbor.first;
//
//            if (index.find(w) == index.end()) {
//                // w еще не посещена
//                tarjanDFS(w);
//                lowlink[v] = std::min(lowlink[v], lowlink[w]);
//            }
//            else if (onStack[w]) {
//                // w в стеке -> обратное ребро
//                lowlink[v] = std::min(lowlink[v], index[w]);
//            }
//        }
//    }
//
//    // Если v - корень компоненты
//    if (lowlink[v] == index[v]) {
//        std::vector<wchar_t> component;
//        wchar_t w;
//
//        do {
//            w = S.top();
//            S.pop();
//            onStack[w] = false;
//            component.push_back(w);
//        } while (w != v);
//
//        SCCs.push_back(component);
//    }
//}
//
//void GraphAnalyzer::findSCCs() {
//    // Сбрасываем данные
//    index.clear();
//    lowlink.clear();
//    onStack.clear();
//    SCCs.clear();
//    currentIndex = 0;
//
//    // Очищаем стек
//    while (!S.empty()) S.pop();
//
//    // Запускаем DFS для каждой непосещенной вершины
//    for (wchar_t v : vertices) {
//        if (index.find(v) == index.end()) {
//            tarjanDFS(v);
//        }
//    }
//
//    // Сортируем компоненты по размеру (от большего к меньшему)
//    std::sort(SCCs.begin(), SCCs.end(),
//        [](const std::vector<wchar_t>& a, const std::vector<wchar_t>& b) {
//            return a.size() > b.size();
//        });
//}
//
//std::vector<std::vector<wchar_t>> GraphAnalyzer::getStronglyConnectedComponents() {
//    return SCCs;
//}
//
//std::vector<int> GraphAnalyzer::getSCCProfile() {
//    std::vector<int> profile;
//    for (const auto& component : SCCs) {
//        profile.push_back(static_cast<int>(component.size()));
//    }
//    return profile;
//}
//
//// Генерация DOT для Graphviz
//std::string GraphAnalyzer::getGraphvizDot() const {
//    std::stringstream ss;
//    ss << "digraph G {\n";
//    ss << "  rankdir=LR;\n";
//    ss << "  node [shape=circle, style=filled, fillcolor=lightblue];\n";
//    ss << "  edge [fontsize=10];\n\n";
//
//    // Добавляем вершины
//    for (wchar_t v : vertices) {
//        ss << "  \"" << static_cast<char>(v) << "\";\n";
//    }
//    ss << "\n";
//
//    // Добавляем ребра с весами
//    for (const auto& from_pair : adjacencyList) {
//        wchar_t from = from_pair.first;
//        for (const auto& to_pair : from_pair.second) {
//            wchar_t to = to_pair.first;
//            double weight = to_pair.second;
//
//            ss << "  \"" << static_cast<char>(from) << "\" -> \""
//                << static_cast<char>(to) << "\" [label=\""
//                << std::fixed << std::setprecision(2) << weight << "%\"];\n";
//        }
//    }
//
//    ss << "}\n";
//    return ss.str();
//}
//
//// DOT с выделенными SCC
//std::string GraphAnalyzer::getSCCGraphvizDot() const {
//    std::stringstream ss;
//    ss << "digraph G {\n";
//    ss << "  rankdir=LR;\n";
//    ss << "  node [shape=circle];\n\n";
//
//    // Цвета для разных компонент
//    std::vector<std::string> colors = {
//        "lightblue", "lightgreen", "lightpink", "lightyellow",
//        "lightcoral", "lightsalmon", "lightseagreen"
//    };
//
//    // Рисуем компоненты
//    for (size_t i = 0; i < SCCs.size(); ++i) {
//        std::string color = colors[i % colors.size()];
//
//        ss << "  subgraph cluster_" << i << " {\n";
//        ss << "    style=filled;\n";
//        ss << "    color=" << color << ";\n";
//        ss << "    label=\"SCC " << (i + 1) << " (size=" << SCCs[i].size() << ")\";\n";
//
//        for (wchar_t v : SCCs[i]) {
//            ss << "    \"" << static_cast<char>(v) << "\";\n";
//        }
//        ss << "  }\n\n";
//    }
//
//    // Рисуем ребра между компонентами
//    ss << "  // Межкомпонентные связи\n";
//    for (const auto& from_pair : adjacencyList) {
//        wchar_t from = from_pair.first;
//        for (const auto& to_pair : from_pair.second) {
//            wchar_t to = to_pair.first;
//            double weight = to_pair.second;
//
//            // Находим компоненты вершин
//            int fromComp = -1, toComp = -1;
//            for (size_t i = 0; i < SCCs.size(); ++i) {
//                if (std::find(SCCs[i].begin(), SCCs[i].end(), from) != SCCs[i].end()) {
//                    fromComp = i;
//                }
//                if (std::find(SCCs[i].begin(), SCCs[i].end(), to) != SCCs[i].end()) {
//                    toComp = i;
//                }
//            }
//
//            // Если ребро между разными компонентами
//            if (fromComp != -1 && toComp != -1 && fromComp != toComp) {
//                ss << "  \"" << static_cast<char>(from) << "\" -> \""
//                    << static_cast<char>(to) << "\" [style=dashed, color=gray];\n";
//            }
//        }
//    }
//
//    ss << "}\n";
//    return ss.str();
//}
//
//GraphAnalyzer::GraphStats GraphAnalyzer::getGraphStats() const {
//    GraphStats stats;
//    stats.vertices = static_cast<int>(vertices.size());
//
//    // Считаем ребра
//    stats.edges = 0;
//    double totalWeight = 0.0;
//    for (const auto& from_pair : adjacencyList) {
//        stats.edges += static_cast<int>(from_pair.second.size());
//        for (const auto& to_pair : from_pair.second) {
//            totalWeight += to_pair.second;
//        }
//    }
//
//    // Находим вершину с максимальной степенью
//    stats.mostConnectedVertex = L'\0';
//    int maxDegree = -1;
//    for (wchar_t v : vertices) {
//        int outDegree = 0;
//        if (adjacencyList.find(v) != adjacencyList.end()) {
//            outDegree = static_cast<int>(adjacencyList.at(v).size());
//        }
//
//        if (outDegree > maxDegree) {
//            maxDegree = outDegree;
//            stats.mostConnectedVertex = v;
//        }
//    }
//
//    // Статистика SCC
//    stats.sccCount = static_cast<int>(SCCs.size());
//    for (const auto& component : SCCs) {
//        stats.sccSizes.push_back(static_cast<int>(component.size()));
//    }
//
//    // Средний вес ребра
//    stats.averageEdgeWeight = (stats.edges > 0) ? totalWeight / stats.edges : 0.0;
//
//    return stats;
//}
//
//double GraphAnalyzer::compareSCCProfiles(const std::vector<int>& profile1,
//    const std::vector<int>& profile2) {
//    // Нормализуем профили до одинаковой длины
//    size_t maxLen = std::max(profile1.size(), profile2.size());
//    std::vector<double> v1(maxLen, 0.0);
//    std::vector<double> v2(maxLen, 0.0);
//
//    for (size_t i = 0; i < maxLen; ++i) {
//        v1[i] = (i < profile1.size()) ? static_cast<double>(profile1[i]) : 0.0;
//        v2[i] = (i < profile2.size()) ? static_cast<double>(profile2[i]) : 0.0;
//    }
//
//    // Вычисляем косинусную близость
//    double dot = 0.0, norm1 = 0.0, norm2 = 0.0;
//    for (size_t i = 0; i < maxLen; ++i) {
//        dot += v1[i] * v2[i];
//        norm1 += v1[i] * v1[i];
//        norm2 += v2[i] * v2[i];
//    }
//
//    if (norm1 == 0.0 || norm2 == 0.0) return 0.0;
//    return dot / (std::sqrt(norm1) * std::sqrt(norm2));
//}
