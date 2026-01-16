#ifndef GRAPH_ANALYZER_H
#define GRAPH_ANALYZER_H

#include <vector>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <tuple>

// Структура для статистики графа
struct GraphStats {
    int numVertices = 0;
    int numEdges = 0;
    double averageDegree = 0.0;
    double density = 0.0;
    int maxComponentSize = 0;
    int numComponents = 0;
    double averageEdgeWeight = 0.0;

    // Конструктор по умолчанию
    GraphStats() = default;

    // Конструктор с параметрами
    GraphStats(int v, int e, double avgDeg, double dens, int maxComp, int numComp, double avgWeight = 0.0)
        : numVertices(v), numEdges(e), averageDegree(avgDeg),
        density(dens), maxComponentSize(maxComp), numComponents(numComp),
        averageEdgeWeight(avgWeight) {}  // <-- ДОБАВЬТЕ averageEdgeWeight
};

class GraphAnalyzer {
private:
    std::map<wchar_t, std::map<wchar_t, double>> adjacencyList;
    std::set<wchar_t> vertices;
    std::map<std::wstring, double> bigramFrequencies;

    // Для алгоритма Тарьяна
    std::map<wchar_t, int> index;
    std::map<wchar_t, int> lowlink;
    std::stack<wchar_t> tarjanStack;
    std::vector<wchar_t> stackContained;
    std::vector<std::vector<wchar_t>> sccs;
    int currentIndex = 0;

    // Вспомогательные методы для алгоритма Тарьяна
    void strongConnect(wchar_t v);

public:
    // Конструктор
    GraphAnalyzer();

    // Основные методы
    void buildGraphFromBigrams(const std::map<std::wstring, double>& bigrams);
    std::vector<std::vector<wchar_t>> getStronglyConnectedComponents();
    std::vector<int> getSCCProfile();
    GraphStats getGraphStats() const;

    // Методы для доступа к данным
    std::set<wchar_t> getAllLetters() const;
    std::map<std::wstring, double> getAllBigramsWithFrequencies() const;
    std::vector<std::tuple<wchar_t, wchar_t, double>> getAllEdgesWithWeights() const;

    // Статические методы для сравнения
    static double compareSCCProfiles(const std::vector<int>& profile1,
        const std::vector<int>& profile2);

    // Утилиты
    std::string getAdjacencyMatrix() const;
    void printGraphInfo() const;

    // Вспомогательные методы конвертации
    static std::string wcharToString(wchar_t wc);
    static std::string wstringToUtf8(const std::wstring& wstr);

private:
    // Очистка данных SCC
    void clearSCCData();
};

#endif // GRAPH_ANALYZER_H




//// GraphAnalyzer_w.h
//#ifndef GRAPH_ANALYZER_W_H
//#define GRAPH_ANALYZER_W_H
//
//#include <vector>
//#include <map>
//#include <string>
//#include <wchar.h>
//
//struct GraphStats {
//    int vertices;
//    int edges;
//    int sccCount;
//    double averageEdgeWeight;
//    double graphDensity;  // Добавим плотность графа
//    double maxEdgeWeight; // Максимальный вес ребра
//    double minEdgeWeight; // Минимальный вес ребра
//};
//
//class GraphAnalyzer {
//public:
//    // Конструктор
//    GraphAnalyzer();
//
//    // Построение графа из биграмм
//    void buildGraphFromBigrams(const std::map<std::wstring, double>& bigrams);
//
//    // Получение компонент сильной связности
//    std::vector<std::vector<wchar_t>> getStronglyConnectedComponents() const;
//
//    // Получение профиля SCC (размеры компонент)
//    std::vector<int> getSCCProfile() const;
//
//    // Получение статистики графа
//    GraphStats getGraphStats() const;
//
//    // Сравнение профилей SCC (статический метод)
//    static double compareSCCProfiles(const std::vector<int>& profile1,
//        const std::vector<int>& profile2);
//
//    // НОВЫЕ МЕТОДЫ для экспорта и анализа:
//
//    // Получение уникальных букв (вершин)
//    std::vector<wchar_t> getVertices() const;
//
//    // Получение ребер в формате для визуализации
//    std::vector<std::tuple<wchar_t, wchar_t, double>> getEdges() const;
//
//    // Получение топ N биграмм
//    std::vector<std::pair<std::wstring, double>> getTopBigrams(int n) const;
//
//    // Экспорт в JSON для визуализации
//    std::string toJSON() const;
//
//    // Экспорт в DOT формат (Graphviz)
//    std::string toDOT() const;
//
//    // Получение центральности вершин (степень)
//    std::map<wchar_t, double> getVertexCentrality() const;
//
//    // Очистка графа
//    void clear();
//
//private:
//    std::map<wchar_t, std::map<wchar_t, double>> adjacencyMatrix;
//    std::vector<std::vector<wchar_t>> sccComponents;
//    std::map<std::wstring, double> bigramData;  // Храним исходные данные биграмм
//
//    // Алгоритм Тарьяна для поиска SCC
//    void tarjanSCC();
//    void strongConnect(wchar_t v,
//        std::map<wchar_t, int>& index,
//        std::map<wchar_t, int>& lowlink,
//        std::vector<wchar_t>& stack,
//        std::map<wchar_t, bool>& onStack,
//        int& idx);
//};
//
//#endif // GRAPH_ANALYZER_W_H


//#pragma once
//#ifndef GRAPH_ANALYZER_W_H
//#define GRAPH_ANALYZER_W_H
//
//#include <vector>
//#include <map>
//#include <string>
//#include <wchar.h>
//
//struct GraphStats {
//    int vertices;
//    int edges;
//    int sccCount;
//    double averageEdgeWeight;
//};
//
//class GraphAnalyzer {
//public:
//    // Конструктор
//    GraphAnalyzer();
//
//    // Построение графа из биграмм
//    void buildGraphFromBigrams(const std::map<std::wstring, double>& bigrams);
//
//    // Получение компонент сильной связности
//    std::vector<std::vector<wchar_t>> getStronglyConnectedComponents() const;
//
//    // Получение профиля SCC (размеры компонент)
//    std::vector<int> getSCCProfile() const;
//
//    // Получение статистики графа
//    GraphStats getGraphStats() const;
//
//    // Сравнение профилей SCC (статический метод)
//    static double compareSCCProfiles(const std::vector<int>& profile1,
//        const std::vector<int>& profile2);
//
//private:
//    std::map<wchar_t, std::map<wchar_t, double>> adjacencyMatrix;
//    std::vector<std::vector<wchar_t>> sccComponents;
//
//    // Алгоритм Тарьяна для поиска SCC
//    void tarjanSCC();
//    void strongConnect(wchar_t v,
//        std::map<wchar_t, int>& index,
//        std::map<wchar_t, int>& lowlink,
//        std::vector<wchar_t>& stack,
//        std::map<wchar_t, bool>& onStack,
//        int& idx);
//};
//
//#endif // GRAPH_ANALYZER_W_H



//#pragma once
//#ifndef GRAPHANALYZER_W_H
//#define GRAPHANALYZER_W_H
//
//#include <vector>
//#include <map>
//#include <set>
//#include <stack>
//#include <string>
//#include <functional>
//
//class GraphAnalyzer {
//private:
//    // Структура графа
//    std::map<wchar_t, std::map<wchar_t, double>> adjacencyList; // Список смежности
//    std::vector<wchar_t> vertices; // Все вершины (буквы)
//
//    // Алгоритм Тарьяна
//    std::map<wchar_t, int> index;
//    std::map<wchar_t, int> lowlink;
//    std::map<wchar_t, bool> onStack;
//    std::stack<wchar_t> S;
//    int currentIndex;
//    std::vector<std::vector<wchar_t>> SCCs; // Найденные компоненты сильной связности
//
//    // Вспомогательные методы
//    void tarjanDFS(wchar_t v);
//    void findSCCs();
//
//public:
//    GraphAnalyzer();
//
//    // Построение графа из биграмм
//    void buildGraphFromBigrams(const std::map<std::wstring, double>& bigrams);
//
//    // Получение компонент сильной связности
//    std::vector<std::vector<wchar_t>> getStronglyConnectedComponents();
//
//    // Получение профиля SCC (размеры компонент)
//    std::vector<int> getSCCProfile();
//
//    // Визуализация графа в формате DOT (для Graphviz)
//    std::string getGraphvizDot() const;
//
//    // Визуализация SCC в формате DOT
//    std::string getSCCGraphvizDot() const;
//
//    // Статистика по графу
//    struct GraphStats {
//        int vertices;
//        int edges;
//        int sccCount;
//        std::vector<int> sccSizes;
//        wchar_t mostConnectedVertex;
//        double averageEdgeWeight;
//    };
//
//    GraphStats getGraphStats() const;
//
//    // Сравнение SCC профилей
//    static double compareSCCProfiles(const std::vector<int>& profile1,
//        const std::vector<int>& profile2);
//};
//
//#endif // GRAPHANALYZER_W_H