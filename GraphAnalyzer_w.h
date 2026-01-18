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

