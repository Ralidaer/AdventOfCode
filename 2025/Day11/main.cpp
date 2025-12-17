#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <filesystem>
#include <stdexcept>

/**
 * @brief Read and parse input graph from file.
 *
 * @param file_path Path to the input file
 * @return Map representing the graph (device -> list of outputs)
 * @throws std::runtime_error if the file doesn't exist or cannot be opened
 */
[[nodiscard]] std::map<std::string, std::vector<std::string>> read_input(const std::filesystem::path &file_path)
{
    if (!std::filesystem::exists(file_path))
    {
        throw std::runtime_error("File does not exist: " + file_path.string());
    }

    std::ifstream file(file_path);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file: " + file_path.string());
    }

    std::map<std::string, std::vector<std::string>> graph;
    std::string line;

    while (std::getline(file, line))
    {
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos)
            continue;

        std::string device = line.substr(0, colonPos);
        std::string outputs = line.substr(colonPos + 2);

        std::stringstream ss(outputs);
        std::string output;
        std::vector<std::string> outputList;

        while (ss >> output)
        {
            outputList.push_back(output);
        }

        graph[device] = outputList;
    }

    return graph;
}

/**
 * @brief Count all paths from current node to target node in graph.
 *
 * @param current Current node
 * @param target Target node
 * @param graph Graph representation
 * @return Number of paths from current to target
 */
[[nodiscard]] int count_paths(const std::string &current, const std::string &target,
                              const std::map<std::string, std::vector<std::string>> &graph)
{
    // Jeśli dotarliśmy do celu, znaleźliśmy jedną ścieżkę
    if (current == target)
    {
        return 1;
    }

    // Jeśli węzeł nie ma wyjść, nie ma ścieżki
    if (graph.find(current) == graph.end())
    {
        return 0;
    }

    // Zliczamy ścieżki przez wszystkie wyjścia
    int totalPaths = 0;
    for (const auto &next : graph.at(current))
    {
        totalPaths += count_paths(next, target, graph);
    }

    return totalPaths;
}

/**
 * @brief Solve Advent of Code 2025 Day 11 Part 1.
 *
 * Counts all paths from 'you' to 'out' in the network graph.
 *
 * @param file_path Path to the input file
 * @return Number of paths from 'you' to 'out'
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] int advent_of_code_2025_day11_part1(const std::filesystem::path &file_path)
{
    const auto graph = read_input(file_path);
    const int pathCount = count_paths("you", "out", graph);
    return pathCount;
}

/**
 * @brief Count paths from current to target that visit specific required nodes.
 *
 * Uses memoization to efficiently count paths that must visit both 'dac' and 'fft'.
 *
 * @param current Current node
 * @param target Target node
 * @param visitedDac Whether 'dac' has been visited
 * @param visitedFft Whether 'fft' has been visited
 * @param graph Graph representation
 * @param memo Memoization cache
 * @return Number of valid paths
 */
[[nodiscard]] long long count_paths_with_nodes(const std::string &current, const std::string &target,
                                               bool visitedDac, bool visitedFft,
                                               const std::map<std::string, std::vector<std::string>> &graph,
                                               std::map<std::tuple<std::string, bool, bool>, long long> &memo)
{
    // Jeśli dotarliśmy do celu
    if (current == target)
    {
        // Zwróć 1 tylko jeśli odwiedziliśmy oba wymagane węzły
        return (visitedDac && visitedFft) ? 1 : 0;
    }

    // Sprawdź memoizację
    auto key = std::make_tuple(current, visitedDac, visitedFft);
    if (memo.find(key) != memo.end())
    {
        return memo[key];
    }

    // Jeśli węzeł nie ma wyjść
    if (graph.find(current) == graph.end())
    {
        memo[key] = 0;
        return 0;
    }

    // Zliczamy ścieżki przez wszystkie wyjścia
    long long totalPaths = 0;
    for (const auto &next : graph.at(current))
    {
        const bool newVisitedDac = visitedDac || (next == "dac");
        const bool newVisitedFft = visitedFft || (next == "fft");
        totalPaths += count_paths_with_nodes(next, target, newVisitedDac, newVisitedFft, graph, memo);
    }

    memo[key] = totalPaths;
    return totalPaths;
}

/**
 * @brief Solve Advent of Code 2025 Day 11 Part 2.
 *
 * Counts paths from 'svr' to 'out' that visit both 'dac' and 'fft'.
 *
 * @param file_path Path to the input file
 * @return Number of valid paths from 'svr' to 'out' through 'dac' and 'fft'
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] long long advent_of_code_2025_day11_part2(const std::filesystem::path &file_path)
{
    const auto graph = read_input(file_path);
    std::map<std::tuple<std::string, bool, bool>, long long> memo;
    const bool startVisitedDac = (std::string("svr") == "dac");
    const bool startVisitedFft = (std::string("svr") == "fft");
    const long long totalValidPaths = count_paths_with_nodes("svr", "out", startVisitedDac, startVisitedFft, graph, memo);
    return totalValidPaths;
}

/**
 * @brief Main entry point of the program.
 *
 * Executes both parts of the Advent of Code 2025 Day 11 challenge.
 *
 * @return 0 on success, 1 if an exception is caught
 */
int main()
{
    try
    {
        const std::filesystem::path example_file_part1 = "input_example_part1.txt";
        const std::filesystem::path example_file_part2 = "input_example_part2.txt";
        const std::filesystem::path input_file = "input.txt";

        std::cout << "=== input_example_part1.txt ===" << std::endl;
        std::cout << "=== Part 1 ===" << std::endl;
        const auto result1_example = advent_of_code_2025_day11_part1(example_file_part1);
        std::cout << "Number of paths: " << result1_example << std::endl;

        std::cout << "=== input_example_part2.txt ===" << std::endl;
        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2_example = advent_of_code_2025_day11_part2(example_file_part2);
        std::cout << "Number of valid paths: " << result2_example << std::endl;

        std::cout << "\n=== input.txt ===" << std::endl;
        std::cout << "=== Part 1 ===" << std::endl;
        const auto result1 = advent_of_code_2025_day11_part1(input_file);
        std::cout << "Number of paths: " << result1 << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2 = advent_of_code_2025_day11_part2(input_file);
        std::cout << "Number of valid paths: " << result2 << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}