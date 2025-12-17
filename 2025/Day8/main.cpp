#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <map>
#include <filesystem>
#include <stdexcept>

/**
 * @struct Point3D
 * @brief Represents a 3D point with integer coordinates.
 */
struct Point3D
{
    int x; /// X coordinate
    int y; /// Y coordinate
    int z; /// Z coordinate
};

// Union-Find (Disjoint Set Union) data structure
class UnionFind
{
private:
    std::vector<int> parent;
    std::vector<int> rank;
    std::vector<int> size;

public:
    UnionFind(int n)
    {
        parent.resize(n);
        rank.resize(n, 0);
        size.resize(n, 1);
        for (int i = 0; i < n; i++)
        {
            parent[i] = i;
        }
    }

    int find(int x)
    {
        if (parent[x] != x)
        {
            parent[x] = find(parent[x]); // Path compression
        }
        return parent[x];
    }

    bool unite(int x, int y)
    {
        int rootX = find(x);
        int rootY = find(y);

        if (rootX == rootY)
        {
            return false; // Already in same set
        }

        // Union by rank
        if (rank[rootX] < rank[rootY])
        {
            parent[rootX] = rootY;
            size[rootY] += size[rootX];
        }
        else if (rank[rootX] > rank[rootY])
        {
            parent[rootY] = rootX;
            size[rootX] += size[rootY];
        }
        else
        {
            parent[rootY] = rootX;
            size[rootX] += size[rootY];
            rank[rootX]++;
        }
        return true;
    }

    int getSize(int x)
    {
        return size[find(x)];
    }

    std::vector<int> getAllSizes()
    {
        std::map<int, int> componentSizes;
        for (int i = 0; i < parent.size(); i++)
        {
            int root = find(i);
            componentSizes[root] = size[root];
        }

        std::vector<int> sizes;
        for (const auto &p : componentSizes)
        {
            sizes.push_back(p.second);
        }
        return sizes;
    }
};

/**
 * @brief Calculate Euclidean distance between two 3D points.
 *
 * @param a First point
 * @param b Second point
 * @return Distance between points
 */
[[nodiscard]] double distance(const Point3D &a, const Point3D &b) noexcept
{
    long long dx = a.x - b.x;
    long long dy = a.y - b.y;
    long long dz = a.z - b.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

/**
 * @brief Read 3D junction points from input file.
 *
 * Reads a file containing 3D coordinates in format "x,y,z" (one per line).
 *
 * @param file_path Path to the input file
 * @return Vector of Point3D structures
 * @throws std::runtime_error if the file doesn't exist or cannot be opened
 */
[[nodiscard]] std::vector<Point3D> read_input(const std::filesystem::path &file_path)
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

    std::vector<Point3D> junctions;
    junctions.reserve(100);

    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        Point3D p;
        char comma;
        ss >> p.x >> comma >> p.y >> comma >> p.z;
        junctions.push_back(p);
    }

    junctions.shrink_to_fit();
    return junctions;
}

/**
 * @brief Solve Advent of Code 2025 Day 8 Part 1.
 *
 * Connects junction boxes with limited connections and calculates
 * the product of the three largest circuits.
 *
 * @param file_path Path to the input file
 * @return Product of three largest circuit sizes
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] long long advent_of_code_2025_day8_part1(const std::filesystem::path &file_path)
{
    const auto junctions = read_input(file_path);
    const int n = static_cast<int>(junctions.size());

    // Create list of all pairs with their distances
    struct Edge
    {
        int i, j;
        double dist;
        bool operator<(const Edge &other) const
        {
            return dist < other.dist;
        }
    };

    std::vector<Edge> edges;
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            double d = distance(junctions[i], junctions[j]);
            edges.push_back({i, j, d});
        }
    }

    // Sort edges by distance
    std::sort(edges.begin(), edges.end());

    // Union-Find to track circuits
    UnionFind uf(n);

    // Connect the closest pairs (10 for example, n-1 for full input to ensure connectivity)
    int connections = (n == 20) ? 10 : 1000; // 20 boxes = example, otherwise full input
    int attempts = 0;

    for (const auto &edge : edges)
    {
        if (attempts >= connections)
            break;

        attempts++;
        uf.unite(edge.i, edge.j);
    }

    // Get all circuit sizes
    std::vector<int> sizes = uf.getAllSizes();
    std::sort(sizes.rbegin(), sizes.rend()); // Sort descending

    // Multiply the three largest
    if (sizes.size() >= 3)
    {
        return static_cast<long long>(sizes[0]) * sizes[1] * sizes[2];
    }
    return 0;
}

/**
 * @brief Solve Advent of Code 2025 Day 8 Part 2.
 *
 * Connects all junction boxes into one circuit and calculates
 * the product of X coordinates of the last connection.
 *
 * @param file_path Path to the input file
 * @return Product of X coordinates of last connection
 * @throws std::runtime_error if the file cannot be read
 */
[[nodiscard]] long long advent_of_code_2025_day8_part2(const std::filesystem::path &file_path)
{
    const auto junctions = read_input(file_path);
    const int n = static_cast<int>(junctions.size());

    // Create list of all pairs with their distances
    struct Edge
    {
        int i, j;
        double dist;
        bool operator<(const Edge &other) const
        {
            return dist < other.dist;
        }
    };

    std::vector<Edge> edges;
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            double d = distance(junctions[i], junctions[j]);
            edges.push_back({i, j, d});
        }
    }

    // Sort edges by distance
    std::sort(edges.begin(), edges.end());

    // Union-Find to track circuits
    UnionFind uf(n);

    // Connect pairs until all are in one circuit
    int lastI = -1, lastJ = -1;

    for (const auto &edge : edges)
    {
        if (uf.unite(edge.i, edge.j))
        {
            lastI = edge.i;
            lastJ = edge.j;

            // Check if all are in one circuit
            if (uf.getSize(0) == n)
            {
                break;
            }
        }
    }

    return static_cast<long long>(junctions[lastI].x) * junctions[lastJ].x;
}

/**
 * @brief Main entry point of the program.
 *
 * Executes both parts of the Advent of Code 2025 Day 8 challenge on both
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
        const auto result1_example = advent_of_code_2025_day8_part1(example_file);
        std::cout << "Product of three largest circuits: " << result1_example << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2_example = advent_of_code_2025_day8_part2(example_file);
        std::cout << "Product of X coordinates: " << result2_example << std::endl;

        std::cout << "\n=== input.txt ===" << std::endl;
        std::cout << "=== Part 1 ===" << std::endl;
        const auto result1 = advent_of_code_2025_day8_part1(input_file);
        std::cout << "Product of three largest circuits: " << result1 << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2 = advent_of_code_2025_day8_part2(input_file);
        std::cout << "Product of X coordinates: " << result2 << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}