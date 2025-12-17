#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <filesystem>
#include <stdexcept>
#include <utility>

/**
 * @brief Read grid from an input file.
 *
 * Reads a file containing a grid (one line per row).
 * Empty lines are skipped.
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
    grid.reserve(100);

    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty())
        {
            grid.push_back(std::move(line));
        }
    }

    grid.shrink_to_fit();
    return grid;
}

/**
 * @brief Solve Advent of Code 2025 Day 4 Part 1.
 *
 * Counts accessible rolls (marked with '@') in the grid.
 * A roll is accessible if it has fewer than 4 adjacent rolls.
 *
 * @param file_path Path to the input file containing the grid
 * @return Number of accessible rolls
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] int advent_of_code_2025_day4_part1(const std::filesystem::path &file_path)
{
    const auto grid = read_input(file_path);

    if (grid.empty())
    {
        return 0;
    }

    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());

    // Define 8 directions: N, NE, E, SE, S, SW, W, NW
    constexpr std::array<int, 8> dx = {-1, -1, 0, 1, 1, 1, 0, -1};
    constexpr std::array<int, 8> dy = {0, 1, 1, 1, 0, -1, -1, -1};

    int accessibleCount = 0;

    // Check each position
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            // Only check positions with '@'
            if (grid[i][j] == '@')
            {
                int adjacentRolls = 0;

                // Check all 8 adjacent positions
                for (int dir = 0; dir < 8; ++dir)
                {
                    int ni = i + dx[dir];
                    int nj = j + dy[dir];

                    // Check if position is within bounds
                    if (ni >= 0 && ni < rows && nj >= 0 && nj < cols)
                    {
                        if (grid[ni][nj] == '@')
                        {
                            adjacentRolls++;
                        }
                    }
                }

                // A roll is accessible if there are fewer than 4 adjacent rolls
                if (adjacentRolls < 4)
                {
                    accessibleCount++;
                }
            }
        }
    }

    return accessibleCount;
}

/**
 * @brief Solve Advent of Code 2025 Day 4 Part 2.
 *
 * Counts total number of accessible rolls that can be removed.
 * Keeps removing accessible rolls until no more can be removed.
 * A roll is accessible if it has fewer than 4 adjacent rolls.
 *
 * @param file_path Path to the input file containing the grid
 * @return Total number of rolls removed
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] int advent_of_code_2025_day4_part2(const std::filesystem::path &file_path)
{
    auto grid = read_input(file_path);

    if (grid.empty())
    {
        return 0;
    }

    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());

    // Define 8 directions: N, NE, E, SE, S, SW, W, NW
    constexpr std::array<int, 8> dx = {-1, -1, 0, 1, 1, 1, 0, -1};
    constexpr std::array<int, 8> dy = {0, 1, 1, 1, 0, -1, -1, -1};

    int totalRemoved = 0;
    bool changed = true;

    // Keep removing accessible rolls until no more can be removed
    while (changed)
    {
        changed = false;
        std::vector<std::pair<int, int>> toRemove;

        // Find all accessible rolls in current iteration
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                // Only check positions with '@'
                if (grid[i][j] == '@')
                {
                    int adjacentRolls = 0;

                    // Check all 8 adjacent positions
                    for (int dir = 0; dir < 8; ++dir)
                    {
                        int ni = i + dx[dir];
                        int nj = j + dy[dir];

                        // Check if position is within bounds
                        if (ni >= 0 && ni < rows && nj >= 0 && nj < cols)
                        {
                            if (grid[ni][nj] == '@')
                            {
                                adjacentRolls++;
                            }
                        }
                    }

                    // A roll is accessible if there are fewer than 4 adjacent rolls
                    if (adjacentRolls < 4)
                    {
                        toRemove.push_back({i, j});
                    }
                }
            }
        }

        // Remove all accessible rolls
        if (!toRemove.empty())
        {
            for (const auto &pos : toRemove)
            {
                grid[pos.first][pos.second] = '.';
                totalRemoved++;
            }
            changed = true;
        }
    }

    return totalRemoved;
}

/**
 * @brief Main entry point of the program.
 *
 * Executes both parts of the Advent of Code 2025 Day 4 challenge on both
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
        const auto result1_example = advent_of_code_2025_day4_part1(example_file);
        std::cout << "Accessible rolls: " << result1_example << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2_example = advent_of_code_2025_day4_part2(example_file);
        std::cout << "Total rolls removed: " << result2_example << std::endl;

        std::cout << "\n=== input.txt ===" << std::endl;
        std::cout << "=== Part 1 ===" << std::endl;
        const auto result1 = advent_of_code_2025_day4_part1(input_file);
        std::cout << "Accessible rolls: " << result1 << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2 = advent_of_code_2025_day4_part2(input_file);
        std::cout << "Total rolls removed: " << result2 << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}