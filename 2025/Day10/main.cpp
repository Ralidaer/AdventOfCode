#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <filesystem>
#include <stdexcept>

/**
 * @struct Machine
 * @brief Represents a machine with indicator lights for Part 1.
 */
struct Machine
{
    std::vector<int> target;               /// Target state of lights
    std::vector<std::vector<int>> buttons; /// Each button's toggle pattern
};

/**
 * @struct MachinePart2
 * @brief Represents a machine with joltage requirements for Part 2.
 */
struct MachinePart2
{
    std::vector<int> joltageReq;           /// Joltage requirements
    std::vector<std::vector<int>> buttons; /// Each button's effect pattern
};

/**
 * @brief Parse a line of input for Part 1.
 *
 * @param line Input line to parse
 * @return Machine structure
 */
[[nodiscard]] Machine parse_line(const std::string &line)
{
    Machine machine;

    // Parse target state [.##.]
    size_t start = line.find('[');
    size_t end = line.find(']');
    std::string targetStr = line.substr(start + 1, end - start - 1);

    for (char c : targetStr)
    {
        machine.target.push_back(c == '#' ? 1 : 0);
    }

    const int numLights = static_cast<int>(machine.target.size());

    // Parse buttons (x,y,z)
    size_t pos = end + 1;
    while (pos < line.size())
    {
        size_t openParen = line.find('(', pos);
        if (openParen == std::string::npos)
            break;

        size_t closeParen = line.find(')', openParen);
        if (closeParen == std::string::npos)
            break;

        // Check if this is the joltage requirements (after {)
        size_t bracePos = line.find('{', pos);
        if (bracePos != std::string::npos && openParen > bracePos)
            break;

        const std::string buttonStr = line.substr(openParen + 1, closeParen - openParen - 1);

        std::vector<int> button(numLights, 0);
        std::stringstream ss(buttonStr);
        std::string num;

        while (std::getline(ss, num, ','))
        {
            const int idx = std::stoi(num);
            if (idx < numLights)
            {
                button[idx] = 1;
            }
        }

        machine.buttons.push_back(button);
        pos = closeParen + 1;
    }

    return machine;
}

/**
 * @brief Parse a line of input for Part 2.
 *
 * @param line Input line to parse
 * @return MachinePart2 structure
 */
[[nodiscard]] MachinePart2 parse_line_part2(const std::string &line)
{
    MachinePart2 machine;

    // Parse joltage requirements {3,5,4,7}
    size_t start = line.find('{');
    size_t end = line.find('}');
    std::string joltageStr = line.substr(start + 1, end - start - 1);

    std::stringstream ss(joltageStr);
    std::string num;
    while (std::getline(ss, num, ','))
    {
        machine.joltageReq.push_back(std::stoi(num));
    }

    const int numCounters = static_cast<int>(machine.joltageReq.size());

    // Parse buttons (x,y,z)
    size_t pos = line.find(']') + 1;
    while (pos < start)
    {
        size_t openParen = line.find('(', pos);
        if (openParen == std::string::npos || openParen >= start)
            break;

        size_t closeParen = line.find(')', openParen);
        if (closeParen == std::string::npos)
            break;

        const std::string buttonStr = line.substr(openParen + 1, closeParen - openParen - 1);

        std::vector<int> button(numCounters, 0);
        std::stringstream ss2(buttonStr);
        std::string num2;

        while (std::getline(ss2, num2, ','))
        {
            const int idx = std::stoi(num2);
            if (idx < numCounters)
            {
                button[idx] = 1;
            }
        }

        machine.buttons.push_back(button);
        pos = closeParen + 1;
    }

    return machine;
}

/**
 * @brief Read input data from file for Part 1.
 *
 * @param file_path Path to the input file
 * @return Vector of Machine structures
 * @throws std::runtime_error if the file doesn't exist or cannot be opened
 */
[[nodiscard]] std::vector<Machine> read_input(const std::filesystem::path &file_path)
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

    std::vector<Machine> machines;
    machines.reserve(50);

    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty())
        {
            machines.push_back(parse_line(line));
        }
    }

    machines.shrink_to_fit();
    return machines;
}

/**
 * @brief Read input data from file for Part 2.
 *
 * @param file_path Path to the input file
 * @return Vector of MachinePart2 structures
 * @throws std::runtime_error if the file doesn't exist or cannot be opened
 */
[[nodiscard]] std::vector<MachinePart2> read_input_part2(const std::filesystem::path &file_path)
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

    std::vector<MachinePart2> machines;
    machines.reserve(50);

    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty())
        {
            machines.push_back(parse_line_part2(line));
        }
    }

    machines.shrink_to_fit();
    return machines;
}

/**
 * @brief Solve system of linear equations over GF(2) using Gaussian elimination.
 *
 * Tries all combinations of free variables to find minimum presses.
 *
 * @param matrix Button toggle patterns
 * @param target Target state
 * @return Minimum number of button presses, or -1 if no solution
 */
int solveGF2(std::vector<std::vector<int>> &matrix, std::vector<int> &target)
{
    int rows = target.size();
    int cols = matrix.size();

    if (cols == 0)
        return -1;

    // Create augmented matrix
    std::vector<std::vector<int>> aug(rows, std::vector<int>(cols + 1));
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            aug[i][j] = matrix[j][i]; // Transpose: buttons are columns
        }
        aug[i][cols] = target[i];
    }

    // Keep track of pivot columns
    std::vector<int> pivotCol(rows, -1);
    std::vector<bool> isPivotCol(cols, false);

    // Gaussian elimination
    int pivot = 0;
    for (int col = 0; col < cols && pivot < rows; col++)
    {
        // Find pivot
        int pivotRow = -1;
        for (int row = pivot; row < rows; row++)
        {
            if (aug[row][col] == 1)
            {
                pivotRow = row;
                break;
            }
        }

        if (pivotRow == -1)
            continue;

        // Swap rows
        if (pivotRow != pivot)
        {
            std::swap(aug[pivot], aug[pivotRow]);
        }

        pivotCol[pivot] = col;
        isPivotCol[col] = true;

        // Eliminate
        for (int row = 0; row < rows; row++)
        {
            if (row != pivot && aug[row][col] == 1)
            {
                for (int c = 0; c <= cols; c++)
                {
                    aug[row][c] ^= aug[pivot][c]; // XOR for GF(2)
                }
            }
        }
        pivot++;
    }

    // Check for inconsistency
    for (int row = pivot; row < rows; row++)
    {
        if (aug[row][cols] == 1)
        {
            return -1; // No solution
        }
    }

    // Find free variables
    std::vector<int> freeVars;
    for (int col = 0; col < cols; col++)
    {
        if (!isPivotCol[col])
        {
            freeVars.push_back(col);
        }
    }

    // Try all combinations of free variables
    int minPresses = cols + 1;
    int numFree = freeVars.size();

    for (int mask = 0; mask < (1 << numFree); mask++)
    {
        std::vector<int> solution(cols, 0);

        // Set free variables
        for (int i = 0; i < numFree; i++)
        {
            solution[freeVars[i]] = (mask >> i) & 1;
        }

        // Back substitution for pivot variables
        for (int row = pivot - 1; row >= 0; row--)
        {
            int col = pivotCol[row];
            if (col == -1)
                continue;

            int val = aug[row][cols];
            for (int c = col + 1; c < cols; c++)
            {
                val ^= (aug[row][c] * solution[c]);
            }
            solution[col] = val;
        }

        // Count presses
        int count = 0;
        for (int val : solution)
        {
            count += val;
        }

        minPresses = std::min(minPresses, count);
    }

    return minPresses;
}

/**
 * @brief Solve a machine puzzle for Part 1.
 *
 * @param machine The machine to solve
 * @return Minimum number of button presses
 */
[[nodiscard]] int solveMachine(const Machine &machine)
{
    std::vector<std::vector<int>> buttons = machine.buttons;
    std::vector<int> target = machine.target;

    return solveGF2(buttons, target);
}

/**
 * @brief Solve Advent of Code 2025 Day 10 Part 1.
 *
 * Solves all machines and calculates total button presses needed.
 *
 * @param file_path Path to the input file
 * @return Total minimum button presses
 * @throws std::runtime_error if the file cannot be read or no solution found
 */
[[nodiscard]] int advent_of_code_2025_day10_part1(const std::filesystem::path &file_path)
{
    const auto machines = read_input(file_path);
    int totalPresses = 0;

    for (size_t i = 0; i < machines.size(); i++)
    {
        const Machine &machine = machines[i];
        int presses = solveMachine(machine);

        if (presses == -1)
        {
            throw std::runtime_error("No solution found for machine " + std::to_string(i + 1));
        }

        totalPresses += presses;
    }

    return totalPresses;
}

// Improved backtracking solver with better pruning and constraint propagation
bool backtrackOptimized(const std::vector<std::vector<int>> &A, const std::vector<int> &b, std::vector<long long> &x,
                        std::vector<long long> &remaining, int buttonIdx, long long currentCost, long long &bestCost)
{
    int numButtons = A.size();
    int numCounters = b.size();

    // Prune if current cost already exceeds best
    if (currentCost >= bestCost)
        return false;

    // Base case: all buttons assigned
    if (buttonIdx == numButtons)
    {
        // Check if all counters are satisfied
        for (int i = 0; i < numCounters; i++)
        {
            if (remaining[i] != 0)
                return false;
        }
        bestCost = currentCost;
        return true;
    }

    // Calculate smart bound for current button
    long long maxNeeded = 0;
    bool affectsAny = false;

    for (int i = 0; i < numCounters; i++)
    {
        if (A[buttonIdx][i] == 1 && remaining[i] > 0)
        {
            affectsAny = true;
            maxNeeded = std::max(maxNeeded, remaining[i]);
        }
    }

    // If this button doesn't help with any remaining requirement, skip it
    if (!affectsAny)
    {
        x[buttonIdx] = 0;
        return backtrackOptimized(A, b, x, remaining, buttonIdx + 1, currentCost, bestCost);
    }

    // Try values from 0 to maxNeeded (but not more than bestCost - currentCost)
    long long maxTries = std::min(maxNeeded, bestCost - currentCost - 1);

    // Try in reverse order (greedy: try larger values first for this button)
    for (long long val = maxTries; val >= 0; val--)
    {
        x[buttonIdx] = val;

        // Update remaining counters
        std::vector<long long> newRemaining = remaining;
        bool valid = true;

        for (int i = 0; i < numCounters; i++)
        {
            if (A[buttonIdx][i] == 1)
            {
                newRemaining[i] -= val;
                // If we go negative, this assignment is invalid
                if (newRemaining[i] < 0)
                {
                    valid = false;
                    break;
                }
            }
        }

        if (valid)
        {
            backtrackOptimized(A, b, x, newRemaining, buttonIdx + 1, currentCost + val, bestCost);
        }
    }

    x[buttonIdx] = 0;
    return bestCost < LLONG_MAX;
}

/**
 * @brief Solve integer linear programming problem.
 *
 * @param A Button effect matrix
 * @param b Joltage requirements
 * @return Minimum cost, or -1 if no solution
 */
[[nodiscard]] long long solveIntegerLinear(const std::vector<std::vector<int>> &A, const std::vector<int> &b)
{
    int numButtons = A.size();
    int numCounters = b.size();

    if (numButtons == 0)
        return -1;

    // Initialize solution vector
    std::vector<long long> x(numButtons, 0);

    // Initialize remaining requirements (what still needs to be satisfied)
    std::vector<long long> remaining(numCounters);
    for (int i = 0; i < numCounters; i++)
    {
        remaining[i] = b[i];
    }

    // Calculate initial upper bound
    long long bestCost = 0;
    for (int i = 0; i < numCounters; i++)
    {
        bestCost += b[i];
    }

    // Run optimized backtracking
    if (backtrackOptimized(A, b, x, remaining, 0, 0, bestCost))
    {
        return bestCost;
    }

    return -1;
}

/**
 * @brief Solve Advent of Code 2025 Day 10 Part 2.
 *
 * Solves all machines using integer linear programming.
 *
 * @param file_path Path to the input file
 * @return Total minimum button presses
 * @throws std::runtime_error if the file cannot be read or no solution found
 */
[[nodiscard]] long long advent_of_code_2025_day10_part2(const std::filesystem::path &file_path)
{
    const auto machines = read_input_part2(file_path);
    long long totalPresses = 0;

    for (size_t i = 0; i < machines.size(); i++)
    {
        const MachinePart2 &machine = machines[i];
        long long presses = solveIntegerLinear(machine.buttons, machine.joltageReq);

        if (presses == -1)
        {
            throw std::runtime_error("No solution found for machine " + std::to_string(i + 1));
        }

        totalPresses += presses;
    }

    return totalPresses;
}

/**
 * @brief Main entry point of the program.
 *
 * Executes both parts of the Advent of Code 2025 Day 10 challenge.
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
        const auto result1_example = advent_of_code_2025_day10_part1(example_file);
        std::cout << "Total minimum button presses: " << result1_example << std::endl;

        std::cout << "=== input_example.txt ===" << std::endl;
        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2_example = advent_of_code_2025_day10_part2(example_file);
        std::cout << "Total minimum button presses: " << result2_example << std::endl;

        std::cout << "\n=== input.txt ===" << std::endl;
        std::cout << "=== Part 1 ===" << std::endl;
        const auto result1 = advent_of_code_2025_day10_part1(input_file);
        std::cout << "Total minimum button presses: " << result1 << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}