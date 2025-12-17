#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <functional>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

/**
 * @class BigInt
 * @brief Simple big integer class for addition only.
 */
class BigInt
{
private:
    std::string digits;

public:
    BigInt(unsigned long long num = 0)
    {
        if (num == 0)
        {
            digits = "0";
        }
        else
        {
            while (num > 0)
            {
                digits = char('0' + num % 10) + digits;
                num /= 10;
            }
        }
    }

    BigInt(const std::string &s) : digits(s) {}

    BigInt operator+(const BigInt &other) const
    {
        std::string result;
        int carry = 0;
        int i = digits.length() - 1;
        int j = other.digits.length() - 1;

        while (i >= 0 || j >= 0 || carry)
        {
            int sum = carry;
            if (i >= 0)
                sum += digits[i--] - '0';
            if (j >= 0)
                sum += other.digits[j--] - '0';

            result = char('0' + sum % 10) + result;
            carry = sum / 10;
        }

        return BigInt(result);
    }

    std::string toString() const
    {
        return digits;
    }
};

/**
 * @struct Beam
 * @brief Represents a tachyon beam position in the grid.
 */
struct Beam
{
    int row; /// Current row position
    int col; /// Current column position
};

/**
 * @brief Read grid from an input file.
 *
 * Reads a file containing a grid of characters.
 * Each line represents one row of the grid.
 *
 * @param file_path Path to the input file
 * @return Vector of strings representing the grid
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

    std::vector<std::string> grid;
    grid.reserve(50);

    std::string line;
    while (std::getline(file, line))
    {
        grid.push_back(std::move(line));
    }

    grid.shrink_to_fit();
    return grid;
}

/**
 * @brief Simulate tachyon beam through the grid.
 *
 * Tracks beam splits as the beam moves through the grid.
 * Beam splits when it hits a '^' character.
 *
 * @param grid The grid to simulate
 * @return Number of beam splits
 */
[[nodiscard]] int simulate_tachyon_beam(const std::vector<std::string> &grid)
{

    if (grid.empty())
        return 0;

    // Find the starting position (S)
    const auto it = std::find(grid[0].begin(), grid[0].end(), 'S');
    if (it == grid[0].end())
        return 0;

    const int start_col = static_cast<int>(std::distance(grid[0].begin(), it));

    int split_count = 0;
    std::queue<Beam> beams;
    beams.push({0, start_col});

    // Track which beams we've already processed to avoid infinite loops
    std::set<std::pair<int, int>> processed;

    while (!beams.empty())
    {
        Beam current = beams.front();
        beams.pop();

        // Move the beam downward until it hits a splitter or exits the grid
        while (current.row < grid.size())
        {
            // Check if we've already processed a beam at this position
            auto key = std::make_pair(current.row, current.col);
            if (processed.count(key) > 0)
            {
                break;
            }
            processed.insert(key);

            // Check what's at the current position
            if (current.row < grid.size() && current.col >= 0 && current.col < grid[current.row].length())
            {
                if (grid[current.row][current.col] == '^')
                {
                    // Hit a splitter - create two new beams
                    split_count++;

                    // Left beam
                    if (current.col - 1 >= 0)
                    {
                        beams.push({current.row + 1, current.col - 1});
                    }

                    // Right beam
                    if (current.col + 1 < grid[current.row].length())
                    {
                        beams.push({current.row + 1, current.col + 1});
                    }

                    break; // Stop this beam
                }
            }

            // Move down
            current.row++;
        }
    }

    return split_count;
}

/**
 * @brief Count quantum timelines through the grid.
 *
 * Uses memoization to count all possible paths through the grid.
 * Each beam split creates a new timeline.
 *
 * @param grid The grid to analyze
 * @return Number of quantum timelines
 */
[[nodiscard]] BigInt count_quantum_timelines(const std::vector<std::string> &grid)
{
    if (grid.empty())
        return BigInt(0);

    // Find the starting position (S)
    const auto it = std::find(grid[0].begin(), grid[0].end(), 'S');
    if (it == grid[0].end())
        return BigInt(0);

    const int start_col = static_cast<int>(std::distance(grid[0].begin(), it));

    // Memoization cache: position -> number of timelines from that position
    std::map<std::pair<int, int>, BigInt> memo;

    std::function<BigInt(int, int)> dfs;
    dfs = [&](int row, int col) -> BigInt
    {
        // Check if we exited the grid - this completes one timeline
        if (row >= grid.size() || col < 0 || col >= grid[0].length())
        {
            return BigInt(1);
        }

        // Check memoization cache
        auto pos = std::make_pair(row, col);
        if (memo.find(pos) != memo.end())
        {
            return memo[pos];
        }

        BigInt count(0);

        // Check if we hit a splitter
        if (grid[row][col] == '^')
        {
            // Split into two paths (left and right)
            count = dfs(row + 1, col - 1) + dfs(row + 1, col + 1);
        }
        else
        {
            // Continue moving down
            count = dfs(row + 1, col);
        }

        memo[pos] = count;
        return count;
    };

    return dfs(0, start_col);
}

/**
 * @brief Solve Advent of Code 2025 Day 7 Part 1.
 *
 * Simulates tachyon beam through the grid and counts beam splits.
 *
 * @param file_path Path to the input file
 * @return Number of beam splits
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] int advent_of_code_2025_day7_part1(const std::filesystem::path &file_path)
{
    const auto grid = read_input(file_path);
    return simulate_tachyon_beam(grid);
}

/**
 * @brief Solve Advent of Code 2025 Day 7 Part 2.
 *
 * Counts all possible quantum timelines through the grid.
 *
 * @param file_path Path to the input file
 * @return Number of quantum timelines as BigInt
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] BigInt advent_of_code_2025_day7_part2(const std::filesystem::path &file_path)
{
    const auto grid = read_input(file_path);
    return count_quantum_timelines(grid);
}

/**
 * @brief Main entry point of the program.
 *
 * Executes both parts of the Advent of Code 2025 Day 7 challenge on both
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
        const auto result1_example = advent_of_code_2025_day7_part1(example_file);
        std::cout << "Total beam splits: " << result1_example << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2_example = advent_of_code_2025_day7_part2(example_file);
        std::cout << "Total quantum timelines: " << result2_example.toString() << std::endl;

        std::cout << "\n=== input.txt ===" << std::endl;
        std::cout << "=== Part 1 ===" << std::endl;
        const auto result1 = advent_of_code_2025_day7_part1(input_file);
        std::cout << "Total beam splits: " << result1 << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2 = advent_of_code_2025_day7_part2(input_file);
        std::cout << "Total quantum timelines: " << result2.toString() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}