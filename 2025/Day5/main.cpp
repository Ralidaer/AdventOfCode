#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <utility>

/**
 * @struct Range
 * @brief Represents a numeric range with start and end values.
 */
struct Range
{
    long long start; /// Start of the range (inclusive)
    long long end;   /// End of the range (inclusive)
};

/**
 * @struct InputData
 * @brief Holds the parsed input data for Day 5.
 */
struct InputData
{
    std::vector<Range> freshRanges;      /// Ranges of fresh ingredient IDs
    std::vector<long long> availableIds; /// Available ingredient IDs
};

/**
 * @brief Read input data from a file.
 *
 * Reads a file containing:
 * - First section: ranges of fresh ingredient IDs (format: "start-end")
 * - Empty line separator
 * - Second section: available ingredient IDs (one per line)
 *
 * @param file_path Path to the input file
 * @return InputData structure with parsed ranges and IDs
 * @throws std::runtime_error if the file doesn't exist or cannot be opened
 */
[[nodiscard]] InputData read_input(const std::filesystem::path &file_path)
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

    InputData data;
    data.freshRanges.reserve(50);
    data.availableIds.reserve(100);

    std::string line;
    bool readingRanges = true;

    // Read fresh ingredient ID ranges
    while (std::getline(file, line))
    {
        if (line.empty())
        {
            readingRanges = false;
            continue;
        }

        if (readingRanges)
        {
            // Parse range (e.g., "3-5")
            const size_t dashPos = line.find('-');
            if (dashPos != std::string::npos)
            {
                const long long start = std::stoll(line.substr(0, dashPos));
                const long long end = std::stoll(line.substr(dashPos + 1));
                data.freshRanges.push_back({start, end});
            }
        }
        else
        {
            // Parse available ingredient ID
            data.availableIds.push_back(std::stoll(line));
        }
    }

    data.freshRanges.shrink_to_fit();
    data.availableIds.shrink_to_fit();
    return data;
}

/**
 * @brief Solve Advent of Code 2025 Day 5 Part 1.
 *
 * Counts how many available ingredient IDs are considered fresh
 * (fall within any of the fresh ingredient ID ranges).
 *
 * @param file_path Path to the input file
 * @return Number of fresh ingredient IDs
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] int advent_of_code_2025_day5_part1(const std::filesystem::path &file_path)
{
    const auto data = read_input(file_path);

    // Check which available IDs are fresh
    int freshCount = 0;
    for (const long long id : data.availableIds)
    {
        const bool isFresh = std::any_of(data.freshRanges.begin(), data.freshRanges.end(),
                                          [id](const Range &range)
                                          {
                                              return id >= range.start && id <= range.end;
                                          });
        if (isFresh)
        {
            freshCount++;
        }
    }

    return freshCount;
}

/**
 * @brief Solve Advent of Code 2025 Day 5 Part 2.
 *
 * Calculates the total number of unique ingredient IDs considered fresh
 * by merging overlapping or adjacent ranges.
 *
 * @param file_path Path to the input file
 * @return Total number of fresh ingredient IDs
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] long long advent_of_code_2025_day5_part2(const std::filesystem::path &file_path)
{
    const auto data = read_input(file_path);
    auto freshRanges = data.freshRanges;

    // Merge overlapping ranges to count total unique IDs
    // First, sort ranges by start position
    std::sort(freshRanges.begin(), freshRanges.end(),
              [](const Range &a, const Range &b)
              { return a.start < b.start; });

    // Merge overlapping or adjacent ranges
    std::vector<Range> mergedRanges;
    for (const auto &range : freshRanges)
    {
        if (mergedRanges.empty() || mergedRanges.back().end < range.start - 1)
        {
            // No overlap, add new range
            mergedRanges.push_back(range);
        }
        else
        {
            // Overlap or adjacent, merge
            mergedRanges.back().end = std::max(mergedRanges.back().end, range.end);
        }
    }

    // Count total IDs in merged ranges
    long long totalFreshIds = 0;
    for (const auto &range : mergedRanges)
    {
        totalFreshIds += (range.end - range.start + 1);
    }

    return totalFreshIds;
}

/**
 * @brief Main entry point of the program.
 *
 * Executes both parts of the Advent of Code 2025 Day 5 challenge on both
 * the example input and the actual input files. Prints results to stdout.
 *
 * @return 0 on success, 1 if an exception is caught
 */
int main()
{
    try
    {
        const std::filesystem::path example_file = "input_example.txt";
        const std::filesystem::path input_file = "input.txt";

        std::cout << "=== input_example.txt ===" << std::endl;
        std::cout << "=== Part 1 ===" << std::endl;
        const auto result1_example = advent_of_code_2025_day5_part1(example_file);
        std::cout << "Number of fresh ingredient IDs: " << result1_example << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2_example = advent_of_code_2025_day5_part2(example_file);
        std::cout << "Total ingredient IDs considered fresh: " << result2_example << std::endl;

        std::cout << "\n=== input.txt ===" << std::endl;
        std::cout << "=== Part 1 ===" << std::endl;
        const auto result1 = advent_of_code_2025_day5_part1(input_file);
        std::cout << "Number of fresh ingredient IDs: " << result1 << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2 = advent_of_code_2025_day5_part2(input_file);
        std::cout << "Total ingredient IDs considered fresh: " << result2 << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}