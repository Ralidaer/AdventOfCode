#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <map>
#include <filesystem>
#include <stdexcept>

/**
 * @struct Point
 * @brief Represents a 2D point with integer coordinates.
 */
struct Point
{
    int x; /// X coordinate
    int y; /// Y coordinate

    bool operator<(const Point &other) const
    {
        if (x != other.x)
            return x < other.x;
        return y < other.y;
    }

    bool operator==(const Point &other) const
    {
        return x == other.x && y == other.y;
    }
};

/**
 * @brief Read 2D points from input file.
 *
 * Reads a file containing 2D coordinates in format "x,y" (one per line).
 *
 * @param file_path Path to the input file
 * @return Vector of Point structures
 * @throws std::runtime_error if the file doesn't exist or cannot be opened
 */
[[nodiscard]] std::vector<Point> read_input(const std::filesystem::path &file_path)
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

    std::vector<Point> redTiles;
    redTiles.reserve(100);

    std::string line;
    while (std::getline(file, line))
    {
        int x, y;
        char comma;
        std::stringstream ss(line);
        ss >> x >> comma >> y;
        redTiles.push_back({x, y});
    }

    redTiles.shrink_to_fit();
    return redTiles;
}

/**
 * @brief Calculate maximum rectangle area using red tiles as corners.
 *
 * @param redTiles Vector of red tile positions
 * @return Maximum area found
 */
[[nodiscard]] long long solve(const std::vector<Point> &redTiles)
{

    long long maxArea = 0;

    // Try all pairs of red tiles as opposite corners
    for (size_t i = 0; i < redTiles.size(); i++)
    {
        for (size_t j = i + 1; j < redTiles.size(); j++)
        {
            const Point p1 = redTiles[i];
            const Point p2 = redTiles[j];

            // Calculate rectangle area
            // The tiles form opposite corners, so we need different x and y coordinates
            if (p1.x != p2.x && p1.y != p2.y)
            {
                long long width = std::abs(p2.x - p1.x) + 1;
                long long height = std::abs(p2.y - p1.y) + 1;
                long long area = width * height;
                maxArea = std::max(maxArea, area);
            }
        }
    }

    return maxArea;
}

/**
 * @brief Solve Advent of Code 2025 Day 9 Part 1.
 *
 * Finds the maximum rectangle area using red tiles as opposite corners.
 *
 * @param file_path Path to the input file
 * @return Maximum rectangle area
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] long long advent_of_code_2025_day9_part1(const std::filesystem::path &file_path)
{
    const auto redTiles = read_input(file_path);
    return solve(redTiles);
}

/**
 * @brief Calculate maximum rectangle area within polygon boundaries.
 *
 * @param redTiles Vector of red tile positions forming a polygon
 * @return Maximum area found within polygon
 */
[[nodiscard]] long long solve_part2(const std::vector<Point> &redTiles)
{

    // Helper function: check if a point is inside the polygon using ray casting
    auto isInsideOrOnPolygon = [&](int px, int py) -> bool
    {
        // First check if on boundary (green tiles - connections between red tiles)
        for (size_t k = 0; k < redTiles.size(); k++)
        {
            Point p1 = redTiles[k];
            Point p2 = redTiles[(k + 1) % redTiles.size()];

            // Check if point is on line segment between p1 and p2
            if (p1.x == p2.x && p1.x == px)
            {
                int minY = std::min(p1.y, p2.y);
                int maxY = std::max(p1.y, p2.y);
                if (py >= minY && py <= maxY)
                    return true;
            }
            else if (p1.y == p2.y && p1.y == py)
            {
                int minX = std::min(p1.x, p2.x);
                int maxX = std::max(p1.x, p2.x);
                if (px >= minX && px <= maxX)
                    return true;
            }
        }

        // Check if inside polygon using ray casting
        int intersections = 0;
        for (size_t k = 0; k < redTiles.size(); k++)
        {
            Point e1 = redTiles[k];
            Point e2 = redTiles[(k + 1) % redTiles.size()];

            if ((e1.y > py) != (e2.y > py))
            {
                double xIntersect = e1.x + (double)(e2.x - e1.x) * (py - e1.y) / (e2.y - e1.y);
                if (px < xIntersect)
                {
                    intersections++;
                }
            }
        }
        return (intersections % 2) == 1;
    };

    // Helper: check if rectangle edge intersects polygon edge
    auto rectangleValid = [&](int minX, int maxX, int minY, int maxY) -> bool
    {
        // Check corners first
        if (!isInsideOrOnPolygon(minX, minY) ||
            !isInsideOrOnPolygon(maxX, minY) ||
            !isInsideOrOnPolygon(minX, maxY) ||
            !isInsideOrOnPolygon(maxX, maxY))
        {
            return false;
        }

        // Sample edge points to ensure edges stay inside
        // Sample ~10 points per edge or fewer if edge is short
        int samplesPerEdge = 10;

        // Top and bottom edges
        int stepX = std::max(1, (maxX - minX) / samplesPerEdge);
        for (int x = minX; x <= maxX; x += stepX)
        {
            if (!isInsideOrOnPolygon(x, minY) || !isInsideOrOnPolygon(x, maxY))
            {
                return false;
            }
        }
        // Make sure we check the last point
        if (!isInsideOrOnPolygon(maxX, minY) || !isInsideOrOnPolygon(maxX, maxY))
        {
            return false;
        }

        // Left and right edges
        int stepY = std::max(1, (maxY - minY) / samplesPerEdge);
        for (int y = minY; y <= maxY; y += stepY)
        {
            if (!isInsideOrOnPolygon(minX, y) || !isInsideOrOnPolygon(maxX, y))
            {
                return false;
            }
        }
        // Make sure we check the last point
        if (!isInsideOrOnPolygon(minX, maxY) || !isInsideOrOnPolygon(maxX, maxY))
        {
            return false;
        }

        // Check if any polygon edge crosses through rectangle interior
        for (size_t k = 0; k < redTiles.size(); k++)
        {
            Point p1 = redTiles[k];
            Point p2 = redTiles[(k + 1) % redTiles.size()];

            int edgeMinX = std::min(p1.x, p2.x);
            int edgeMaxX = std::max(p1.x, p2.x);
            int edgeMinY = std::min(p1.y, p2.y);
            int edgeMaxY = std::max(p1.y, p2.y);

            // If edge passes through rectangle interior, it's bad
            if (p1.x == p2.x)
            { // Vertical edge
                int x = p1.x;
                if (x > minX && x < maxX)
                {
                    if (edgeMinY < maxY && edgeMaxY > minY)
                    {
                        int crossMinY = std::max(edgeMinY, minY);
                        int crossMaxY = std::min(edgeMaxY, maxY);
                        if (crossMinY < crossMaxY)
                        {
                            return false;
                        }
                    }
                }
            }
            else if (p1.y == p2.y)
            { // Horizontal edge
                int y = p1.y;
                if (y > minY && y < maxY)
                {
                    if (edgeMinX < maxX && edgeMaxX > minX)
                    {
                        int crossMinX = std::max(edgeMinX, minX);
                        int crossMaxX = std::min(edgeMaxX, maxX);
                        if (crossMinX < crossMaxX)
                        {
                            return false;
                        }
                    }
                }
            }
        }

        return true;
    };

    long long maxArea = 0;

    // For each pair of red tiles as opposite corners
    for (size_t i = 0; i < redTiles.size(); i++)
    {
        for (size_t j = i + 1; j < redTiles.size(); j++)
        {
            const Point p1 = redTiles[i];
            const Point p2 = redTiles[j];

            if (p1.x == p2.x || p1.y == p2.y)
                continue;

            const int minRectX = std::min(p1.x, p2.x);
            const int maxRectX = std::max(p1.x, p2.x);
            const int minRectY = std::min(p1.y, p2.y);
            const int maxRectY = std::max(p1.y, p2.y);

            // Check if rectangle is valid (all points inside or on polygon)
            if (!rectangleValid(minRectX, maxRectX, minRectY, maxRectY))
            {
                continue;
            }

            const long long width = static_cast<long long>(maxRectX) - minRectX + 1;
            const long long height = static_cast<long long>(maxRectY) - minRectY + 1;
            const long long area = width * height;
            maxArea = std::max(maxArea, area);
        }
    }

    return maxArea;
}

/**
 * @brief Solve Advent of Code 2025 Day 9 Part 2.
 *
 * Finds the maximum rectangle area that fits entirely within the polygon
 * formed by red tiles.
 *
 * @param file_path Path to the input file
 * @return Maximum rectangle area within polygon
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] long long advent_of_code_2025_day9_part2(const std::filesystem::path &file_path)
{
    const auto redTiles = read_input(file_path);
    return solve_part2(redTiles);
}

/**
 * @brief Main entry point of the program.
 *
 * Executes both parts of the Advent of Code 2025 Day 9 challenge on both
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
        const auto result1_example = advent_of_code_2025_day9_part1(example_file);
        std::cout << "Maximum rectangle area: " << result1_example << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2_example = advent_of_code_2025_day9_part2(example_file);
        std::cout << "Maximum rectangle area within polygon: " << result2_example << std::endl;

        std::cout << "\n=== input.txt ===" << std::endl;
        std::cout << "=== Part 1 ===" << std::endl;
        const auto result1 = advent_of_code_2025_day9_part1(input_file);
        std::cout << "Maximum rectangle area: " << result1 << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2 = advent_of_code_2025_day9_part2(input_file);
        std::cout << "Maximum rectangle area within polygon: " << result2 << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}