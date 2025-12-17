#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>

/**
 * @struct Problem
 * @brief Represents a mathematical problem with numbers and an operation.
 */
struct Problem
{
    std::vector<long long> numbers; ///< Numbers in the problem
    char operation;                 ///< Operation to perform ('*' or '+')
};

/**
 * @brief Read worksheet problems from input file (horizontal reading).
 *
 * Reads problems arranged horizontally, parsing each column as a problem.
 *
 * @param file_path Path to the input file
 * @return Vector of Problem structures
 * @throws std::runtime_error if the file doesn't exist or cannot be opened
 */
[[nodiscard]] std::vector<Problem> read_input(const std::filesystem::path &file_path)
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
    std::string line;

    // Read all lines
    while (std::getline(file, line))
    {
        lines.push_back(line);
    }

    if (lines.empty())
        return {};

    // Find the width of the worksheet
    int max_width = 0;
    for (const auto &l : lines)
    {
        max_width = std::max(max_width, (int)l.length());
    }

    // Parse problems column by column
    std::vector<Problem> problems;

    for (int col = 0; col < max_width; ++col)
    {
        // Check if this column starts a problem (has non-space content)
        bool has_content = false;
        for (int row = 0; row < lines.size(); ++row)
        {
            if (col < lines[row].length() && lines[row][col] != ' ')
            {
                has_content = true;
                break;
            }
        }

        if (!has_content)
            continue;

        // Find the extent of this problem (until we hit a column of all spaces)
        int end_col = col;
        while (end_col < max_width)
        {
            bool all_spaces = true;
            for (int row = 0; row < lines.size(); ++row)
            {
                if (end_col < lines[row].length() && lines[row][end_col] != ' ')
                {
                    all_spaces = false;
                    break;
                }
            }
            if (all_spaces)
                break;
            end_col++;
        }

        // Extract this problem
        Problem prob;
        std::string number_str = "";

        for (int row = 0; row < lines.size(); ++row)
        {
            std::string segment = "";
            for (int c = col; c < end_col && c < lines[row].length(); ++c)
            {
                segment += lines[row][c];
            }

            // Trim segment
            size_t start = segment.find_first_not_of(' ');
            size_t end = segment.find_last_not_of(' ');
            if (start != std::string::npos)
            {
                segment = segment.substr(start, end - start + 1);
            }
            else
            {
                segment = "";
            }

            if (!segment.empty())
            {
                if (segment == "*" || segment == "+")
                {
                    prob.operation = segment[0];
                }
                else
                {
                    prob.numbers.push_back(std::stoll(segment));
                }
            }
        }

        if (!prob.numbers.empty())
        {
            problems.push_back(prob);
        }

        col = end_col; // Skip to next problem
    }

    return problems;
}

/**
 * @brief Read worksheet problems from input file (vertical reading).
 *
 * Reads problems arranged horizontally, but reads digits vertically.
 * Each column represents a digit read from top to bottom.
 *
 * @param file_path Path to the input file
 * @return Vector of Problem structures
 * @throws std::runtime_error if the file doesn't exist or cannot be opened
 */
[[nodiscard]] std::vector<Problem> read_input_vertical(const std::filesystem::path &file_path)
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
    std::string line;

    while (std::getline(file, line))
    {
        lines.push_back(line);
    }

    if (lines.empty())
        return {};

    int max_width = 0;
    for (const auto &l : lines)
    {
        max_width = std::max(max_width, (int)l.length());
    }

    std::vector<Problem> problems;

    for (int col = 0; col < max_width; ++col)
    {
        bool has_content = false;
        for (int row = 0; row < lines.size(); ++row)
        {
            if (col < lines[row].length() && lines[row][col] != ' ')
            {
                has_content = true;
                break;
            }
        }

        if (!has_content)
            continue;

        int end_col = col;
        while (end_col < max_width)
        {
            bool all_spaces = true;
            for (int row = 0; row < lines.size(); ++row)
            {
                if (end_col < lines[row].length() && lines[row][end_col] != ' ')
                {
                    all_spaces = false;
                    break;
                }
            }
            if (all_spaces)
                break;
            end_col++;
        }

        Problem prob;

        for (int c = end_col - 1; c >= col; --c)
        {
            std::string digit_str = "";
            char operation_char = ' ';

            for (int row = 0; row < lines.size(); ++row)
            {
                if (c < lines[row].length() && lines[row][c] != ' ')
                {
                    char ch = lines[row][c];
                    if (ch == '*' || ch == '+')
                    {
                        operation_char = ch;
                    }
                    else
                    {
                        digit_str += ch;
                    }
                }
            }

            if (!digit_str.empty())
            {
                long long number = std::stoll(digit_str);
                prob.numbers.push_back(number);
            }

            if (operation_char != ' ')
            {
                prob.operation = operation_char;
            }
        }

        if (!prob.numbers.empty())
        {
            problems.push_back(prob);
        }

        col = end_col;
    }

    return problems;
}

/**
 * @brief Solve Advent of Code 2025 Day 6 Part 1.
 *
 * Reads worksheet problems horizontally and calculates results.
 *
 * @param file_path Path to the input file
 * @return Grand total of all problem results
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] long long advent_of_code_2025_day6_part1(const std::filesystem::path &file_path)
{
    const auto problems = read_input(file_path);

    long long grand_total = 0;

    for (const auto &prob : problems)
    {
        long long result = prob.numbers[0];

        for (int i = 1; i < prob.numbers.size(); ++i)
        {
            if (prob.operation == '*')
            {
                result *= prob.numbers[i];
            }
            else if (prob.operation == '+')
            {
                result += prob.numbers[i];
            }
        }

        grand_total += result;
    }

    return grand_total;
}

/**
 * @brief Solve Advent of Code 2025 Day 6 Part 2.
 *
 * Reads worksheet problems with vertical digit reading and calculates results.
 *
 * @param file_path Path to the input file
 * @return Grand total of all problem results
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] long long advent_of_code_2025_day6_part2(const std::filesystem::path &file_path)
{
    const auto problems = read_input_vertical(file_path);

    long long grand_total = 0;

    for (const auto &prob : problems)
    {
        long long result = prob.numbers[0];

        for (int i = 1; i < prob.numbers.size(); ++i)
        {
            if (prob.operation == '*')
            {
                result *= prob.numbers[i];
            }
            else if (prob.operation == '+')
            {
                result += prob.numbers[i];
            }
        }

        grand_total += result;
    }

    return grand_total;
}

/**
 * @brief Main entry point of the program.
 *
 * Executes both parts of the Advent of Code 2025 Day 6 challenge.
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
        std::cout << "=== Part 1: Horizontal Reading ===" << std::endl;
        const auto result1_example = advent_of_code_2025_day6_part1(example_file);
        std::cout << "Grand Total: " << result1_example << std::endl;

        std::cout << "=== Part 2: Vertical Reading ===" << std::endl;
        std::cout << "=== input_example.txt ===" << std::endl;
        const auto result2_example = advent_of_code_2025_day6_part2(example_file);
        std::cout << "Grand Total: " << result2_example << std::endl;

        std::cout << "\n=== input.txt ===" << std::endl;
        std::cout << "=== Part 1: Horizontal Reading ===" << std::endl;
        const auto result1 = advent_of_code_2025_day6_part1(input_file);
        std::cout << "Grand Total: " << result1 << std::endl;

        std::cout << "=== Part 2: Vertical Reading ===" << std::endl;
        const auto result2 = advent_of_code_2025_day6_part2(input_file);
        std::cout << "Grand Total: " << result2 << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}