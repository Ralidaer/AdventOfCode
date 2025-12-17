#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <string_view>
#include <stdexcept>

/**
 * @brief Read battery banks from an input file.
 *
 * Reads a file containing battery banks (one per line).
 * Each line represents a series of battery joltage digits.
 *
 * @param file_path Path to the input file
 * @return Vector of battery bank strings
 * @throws std::runtime_error if the file doesn't exist or cannot be opened
 */
[[nodiscard]] std::vector<std::string> read_input(const std::filesystem::path &file_path)
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

    std::vector<std::string> lines;
    lines.reserve(200); // Reserve space based on typical input size

    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty())
        {
            lines.push_back(std::move(line));
        }
    }

    lines.shrink_to_fit();
    return lines;
}

/**
 * @brief Find maximum joltage by selecting any 2 batteries from a bank.
 *
 * Checks all possible pairs of two batteries and finds the maximum
 * joltage value that can be formed by concatenating their digits.
 *
 * @param bank String of battery joltage digits
 * @return Maximum joltage value from selecting 2 batteries
 */
[[nodiscard]] constexpr int find_max_joltage(std::string_view bank) noexcept
{
    int max_joltage = 0;
    const auto len = bank.length();

    // Check all possible pairs of two batteries
    for (std::size_t i = 0; i < len; ++i)
    {
        for (std::size_t j = i + 1; j < len; ++j)
        {
            // Create a number from two selected digits
            const int joltage = (bank[i] - '0') * 10 + (bank[j] - '0');
            max_joltage = std::max(max_joltage, joltage);
        }
    }

    return max_joltage;
}

/**
 * @brief Find maximum joltage by selecting exactly 12 batteries from a bank.
 *
 * Uses a greedy algorithm to find the largest number that can be formed
 * by selecting 12 batteries in order (maintaining their relative positions).
 *
 * @param bank String of battery joltage digits
 * @return Maximum joltage value from selecting 12 batteries in order
 */
[[nodiscard]] long long find_max_joltage_12_batteries(std::string_view bank)
{
    constexpr std::size_t kBatteriesToSelect = 12;
    const auto len = bank.length();
    std::string result;
    result.reserve(kBatteriesToSelect);

    // Greedy algorithm: for each position in result, select the largest possible digit
    std::size_t start_pos = 0;
    for (std::size_t i = 0; i < kBatteriesToSelect; ++i)
    {
        // How many digits do we still need to select after this position?
        const auto remaining = kBatteriesToSelect - i - 1;

        // Find the largest digit in range that allows selecting remaining digits later
        char max_digit = '0';
        std::size_t max_pos = start_pos;

        // Search from start_pos to position that leaves enough digits
        for (std::size_t j = start_pos; j <= len - remaining - 1; ++j)
        {
            if (bank[j] > max_digit)
            {
                max_digit = bank[j];
                max_pos = j;
            }
        }

        result += max_digit;
        start_pos = max_pos + 1;
    }

    return std::stoll(result);
}

/**
 * @brief Solve Advent of Code 2025 Day 3 Part 1.
 *
 * Calculates the sum of maximum joltages from each battery bank,
 * where each bank contributes its maximum 2-battery joltage.
 *
 * @param file_path Path to the input file containing battery banks
 * @return Sum of all maximum 2-battery joltages
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] long long advent_of_code_2025_day3_part1(const std::filesystem::path &file_path)
{
    const auto banks = read_input(file_path);
    long long total_joltage = 0;

    for (const auto &bank : banks)
    {
        total_joltage += find_max_joltage(bank);
    }

    return total_joltage;
}

/**
 * @brief Solve Advent of Code 2025 Day 3 Part 2.
 *
 * Calculates the sum of maximum joltages from each battery bank,
 * where each bank contributes its maximum 12-battery joltage.
 *
 * @param file_path Path to the input file containing battery banks
 * @return Sum of all maximum 12-battery joltages
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] long long advent_of_code_2025_day3_part2(const std::filesystem::path &file_path)
{
    const auto banks = read_input(file_path);
    long long total_joltage = 0;

    for (const auto &bank : banks)
    {
        total_joltage += find_max_joltage_12_batteries(bank);
    }

    return total_joltage;
}

/**
 * @brief Main entry point of the program.
 *
 * Executes both parts of the Advent of Code 2025 Day 3 challenge on both
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
        const auto result1_example = advent_of_code_2025_day3_part1(example_file);
        std::cout << "Max joltage sum: " << result1_example << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2_example = advent_of_code_2025_day3_part2(example_file);
        std::cout << "Max joltage sum (12 batteries): " << result2_example << std::endl;

        std::cout << "\n=== input.txt ===" << std::endl;
        std::cout << "=== Part 1 ===" << std::endl;
        const auto result1 = advent_of_code_2025_day3_part1(input_file);
        std::cout << "Max joltage sum: " << result1 << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2 = advent_of_code_2025_day3_part2(input_file);
        std::cout << "Max joltage sum (12 batteries): " << result2 << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}