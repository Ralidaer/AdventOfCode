#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

/**
 * @struct Shape
 * @brief Represents a present shape pattern.
 */
struct Shape
{
    std::vector<std::string> pattern; ///< Pattern representation

    /**
     * @brief Get width of the shape.
     * @return Width in characters
     */
    [[nodiscard]] int width() const noexcept
    {
        return pattern.empty() ? 0 : static_cast<int>(pattern[0].size());
    }

    /**
     * @brief Get height of the shape.
     * @return Height in rows
     */
    [[nodiscard]] int height() const noexcept
    {
        return static_cast<int>(pattern.size());
    }
};

/**
 * @struct InputData
 * @brief Contains parsed shapes and region specifications.
 */
struct InputData
{
    std::vector<Shape> shapes;                                             ///< Available present shapes
    std::vector<std::pair<std::pair<int, int>, std::vector<int>>> regions; ///< Regions: (width, height), counts
};

/**
 * @brief Read and parse input from file.
 *
 * @param file_path Path to the input file
 * @return InputData structure with shapes and regions
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

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line))
    {
        lines.push_back(line);
    }

    InputData data;
    size_t i = 0;

    // Parse shapes
    while (i < lines.size())
    {
        line = lines[i];

        if (line.empty())
        {
            i++;
            continue;
        }

        // Check if it's a region definition
        if (line.find('x') != std::string::npos && line.find(':') != std::string::npos)
        {
            break;
        }

        // Check if it's a shape definition
        if (line.find(':') != std::string::npos && line.find('x') == std::string::npos)
        {
            i++; // Skip the "N:" line
            Shape shape;

            while (i < lines.size() && !lines[i].empty())
            {
                shape.pattern.push_back(lines[i]);
                i++;
            }

            if (!shape.pattern.empty())
            {
                data.shapes.push_back(shape);
            }
        }
        else
        {
            i++;
        }
    }

    // Parse regions
    while (i < lines.size())
    {
        if (lines[i].empty())
        {
            i++;
            continue;
        }

        if (lines[i].find('x') == std::string::npos)
        {
            i++;
            continue;
        }

        size_t colonPos = lines[i].find(':');
        std::string dimensions = lines[i].substr(0, colonPos);
        std::string countsStr = lines[i].substr(colonPos + 1);

        size_t xPos = dimensions.find('x');
        int width = std::stoi(dimensions.substr(0, xPos));
        int height = std::stoi(dimensions.substr(xPos + 1));

        std::vector<int> presentCounts;
        std::istringstream iss(countsStr);
        int count;
        while (iss >> count)
        {
            presentCounts.push_back(count);
        }

        data.regions.push_back({{width, height}, presentCounts});
        i++;
    }

    return data;
}

/**
 * @brief Generate all rotations and flips of a shape.
 *
 * @param shape Input shape
 * @return Vector of all unique transformations
 */
[[nodiscard]] std::vector<Shape> generate_transformations(const Shape &shape)
{
    std::vector<Shape> result;
    std::set<std::vector<std::string>> unique;

    const auto addIfUnique = [&](const Shape &s)
    {
        if (unique.find(s.pattern) == unique.end())
        {
            unique.insert(s.pattern);
            result.push_back(s);
        }
    };

    // Original
    Shape current = shape;
    addIfUnique(current);

    // 3 rotations
    for (int r = 0; r < 3; ++r)
    {
        Shape rotated;
        const int h = current.height();
        const int w = current.width();
        rotated.pattern.resize(w);
        for (int i = 0; i < w; i++)
        {
            rotated.pattern[i] = std::string(h, '.');
        }

        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                rotated.pattern[j][h - 1 - i] = current.pattern[i][j];
            }
        }
        current = rotated;
        addIfUnique(current);
    }

    // Flip horizontally and repeat rotations
    Shape flipped = shape;
    for (auto &row : flipped.pattern)
    {
        std::reverse(row.begin(), row.end());
    }
    addIfUnique(flipped);

    current = flipped;
    for (int r = 0; r < 3; ++r)
    {
        Shape rotated;
        const int h = current.height();
        const int w = current.width();
        rotated.pattern.resize(w);
        for (int i = 0; i < w; i++)
        {
            rotated.pattern[i] = std::string(h, '.');
        }

        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                rotated.pattern[j][h - 1 - i] = current.pattern[i][j];
            }
        }
        current = rotated;
        addIfUnique(current);
    }

    return result;
}

/**
 * @brief Check if shape can be placed at position.
 *
 * @param grid Current grid state
 * @param shape Shape to place
 * @param row Starting row position
 * @param col Starting column position
 * @return True if placement is valid
 */
[[nodiscard]] bool can_place(const std::vector<std::string> &grid, const Shape &shape, int row, int col)
{
    if (row + shape.height() > static_cast<int>(grid.size()) || col + shape.width() > static_cast<int>(grid[0].size()))
    {
        return false;
    }

    for (int i = 0; i < shape.height(); i++)
    {
        for (int j = 0; j < shape.width(); j++)
        {
            if (shape.pattern[i][j] == '#')
            {
                if (grid[row + i][col + j] != '.')
                {
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * @brief Place a shape on the grid.
 *
 * @param grid Grid to modify
 * @param shape Shape to place
 * @param row Starting row position
 * @param col Starting column position
 * @param marker Character marker for this shape
 */
void place_shape(std::vector<std::string> &grid, const Shape &shape, int row, int col, char marker)
{
    for (int i = 0; i < shape.height(); i++)
    {
        for (int j = 0; j < shape.width(); j++)
        {
            if (shape.pattern[i][j] == '#')
            {
                grid[row + i][col + j] = marker;
            }
        }
    }
}

/**
 * @brief Remove a shape from the grid.
 *
 * @param grid Grid to modify
 * @param shape Shape to remove
 * @param row Starting row position
 * @param col Starting column position
 */
void remove_shape(std::vector<std::string> &grid, const Shape &shape, int row, int col)
{
    for (int i = 0; i < shape.height(); i++)
    {
        for (int j = 0; j < shape.width(); j++)
        {
            if (shape.pattern[i][j] == '#')
            {
                grid[row + i][col + j] = '.';
            }
        }
    }
}

/**
 * @brief Count empty cells in grid.
 *
 * @param grid Grid to analyze
 * @return Number of empty cells
 */
[[nodiscard]] int count_empty(const std::vector<std::string> &grid) noexcept
{
    int count = 0;
    for (const auto &row : grid)
    {
        for (const char c : row)
        {
            if (c == '.')
                count++;
        }
    }
    return count;
}

/**
 * @brief Count total cells needed for remaining presents.
 *
 * @param allTransformations All shape transformations
 * @param presentCounts Count of each present type
 * @param presentIdx Current present index
 * @param countIdx Current count index
 * @return Total cells needed
 */
[[nodiscard]] int count_needed_cells(const std::vector<std::vector<Shape>> &allTransformations,
                                     const std::vector<int> &presentCounts, const int presentIdx, const int countIdx) noexcept
{
    int needed = 0;
    for (size_t p = presentIdx; p < presentCounts.size(); ++p)
    {
        const int startCount = (static_cast<int>(p) == presentIdx) ? countIdx : 0;
        int shapeSize = 0;
        for (const auto &row : allTransformations[p][0].pattern)
        {
            for (const char c : row)
            {
                if (c == '#')
                    shapeSize++;
            }
        }
        needed += (presentCounts[p] - startCount) * shapeSize;
    }
    return needed;
}

/**
 * @brief Backtracking algorithm to place all presents.
 *
 * @param grid Grid to fill
 * @param allTransformations All shape transformations
 * @param presentCounts Count of each present type
 * @param presentIdx Current present index
 * @param countIdx Current count index
 * @return True if all presents can be placed
 */
[[nodiscard]] bool try_place_presents(std::vector<std::string> &grid, const std::vector<std::vector<Shape>> &allTransformations,
                                      const std::vector<int> &presentCounts, int presentIdx, int countIdx)
{
    // All presents placed successfully
    if (presentIdx >= presentCounts.size())
    {
        return true;
    }

    // Skip shapes with 0 count
    if (countIdx >= presentCounts[presentIdx])
    {
        return try_place_presents(grid, allTransformations, presentCounts, presentIdx + 1, 0);
    }

    // Early termination: check if enough space remains
    const int needed = count_needed_cells(allTransformations, presentCounts, presentIdx, countIdx);
    const int available = count_empty(grid);
    if (needed > available)
    {
        return false;
    }

    // Try each transformation of the current shape
    for (const auto &shape : allTransformations[presentIdx])
    {
        // Try each position in the grid
        for (size_t row = 0; row < grid.size(); ++row)
        {
            for (size_t col = 0; col < grid[0].size(); ++col)
            {
                if (can_place(grid, shape, static_cast<int>(row), static_cast<int>(col)))
                {
                    place_shape(grid, shape, static_cast<int>(row), static_cast<int>(col), static_cast<char>('A' + presentIdx));

                    if (try_place_presents(grid, allTransformations, presentCounts,
                                           presentIdx, countIdx + 1))
                    {
                        return true;
                    }

                    remove_shape(grid, shape, static_cast<int>(row), static_cast<int>(col));
                }
            }
        }
    }

    return false;
}

/**
 * @brief Solve Advent of Code 2025 Day 12 Part 1.
 *
 * Counts how many regions can be completely filled with presents.
 *
 * @param file_path Path to the input file
 * @return Number of valid regions
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] int advent_of_code_2025_day12_part1(const std::filesystem::path &file_path)
{
    const auto data = read_input(file_path);

    // Generate all transformations for each shape
    std::vector<std::vector<Shape>> allTransformations;
    allTransformations.reserve(data.shapes.size());
    for (const auto &shape : data.shapes)
    {
        allTransformations.push_back(generate_transformations(shape));
    }

    // Check each region
    int validRegions = 0;
    for (const auto &region : data.regions)
    {
        const int width = region.first.first;
        const int height = region.first.second;
        const auto &presentCounts = region.second;

        std::vector<std::string> grid(height, std::string(width, '.'));

        if (try_place_presents(grid, allTransformations, presentCounts, 0, 0))
        {
            validRegions++;
        }
    }

    return validRegions;
}

/**
 * @brief Main entry point of the program.
 *
 * Executes Advent of Code 2025 Day 12 challenge.
 *
 * @return 0 on success, 1 if an exception is caught
 */
int main()
{
    try
    {
        const std::filesystem::path example_file = "input_example.txt";
        const std::filesystem::path input_file = "input.txt";

        std::cout << "=== Part 1: Present Fitting ===" << std::endl;
        std::cout << "--- input_example.txt ---" << std::endl;
        const auto result1_example = advent_of_code_2025_day12_part1(example_file);
        std::cout << "Valid regions: " << result1_example << std::endl;

        std::cout << "--- input.txt ---" << std::endl;
        const auto result1 = advent_of_code_2025_day12_part1(input_file);
        std::cout << "Valid regions: " << result1 << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}