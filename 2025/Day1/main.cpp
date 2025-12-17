#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <string_view>
#include <optional>
#include <expected>
#include <filesystem>
#include <stdexcept>
#include <charconv>

/**
 * @enum Direction
 * @brief Represents the direction of movement on a dial.
 * The dial can be rotated either to the right (clockwise) or to the left (counter-clockwise).
 */
enum class Direction : char
{
    Right = 'R', /// Move right (clockwise)
    Left = 'L'   /// Move left (counter-clockwise)
};

/**
 * @struct Instruction
 * @brief Represents a single movement instruction with direction and step count.
 *
 * An instruction consists of a direction (Right or Left) and the number of steps
 * to move in that direction on the dial.
 */
struct Instruction
{
    Direction direction; /// Direction to move
    int steps;           /// Number of steps to move

    /**
     * @brief Construct an instruction with a Direction enum and steps.
     * @param dir The direction to move
     * @param st The number of steps to move
     */
    constexpr Instruction(Direction dir, int st) noexcept : direction(dir), steps(st) {}

    /**
     * @brief Construct an instruction from a character and steps.
     * @param dir The direction character ('R' or 'L')
     * @param st The number of steps to move
     * @throws std::invalid_argument if the direction character is invalid
     */
    explicit Instruction(char dir, int st) : direction(parse_direction(dir)), steps(st) {}

    /**
     * @brief Parse a character into a Direction enum value.
     * @param dir The character to parse ('R' or 'L')
     * @return The corresponding Direction enum value
     * @throws std::invalid_argument if the character is not 'R' or 'L'
     */
    [[nodiscard]] static constexpr Direction parse_direction(char dir)
    {
        switch (dir)
        {
        case 'R':
            return Direction::Right;
        case 'L':
            return Direction::Left;
        default:
            throw std::invalid_argument(std::string("Invalid direction: ") + dir);
        }
    }
};

/**
 * @brief Read and parse instructions from an input file.
 *
 * Reads a file containing movement instructions, where each line consists of
 * a direction character ('R' or 'L') followed by a number of steps.
 *
 * @param file_path Path to the input file
 * @return Vector of parsed Instruction objects
 * @throws std::runtime_error if the file doesn't exist, cannot be opened,
 *         or contains invalid instruction format
 */
[[nodiscard]] std::vector<Instruction> read_input(const std::filesystem::path &file_path)
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

    std::vector<Instruction> instructions;

    const auto file_size = std::filesystem::file_size(file_path);
    constexpr size_t kAvgBytesPerInstruction = 5;
    instructions.reserve(file_size / kAvgBytesPerInstruction);

    std::string line;
    size_t line_number = 0;

    while (file >> line)
    {
        ++line_number;

        if (line.empty())
        {
            continue;
        }

        if (line.length() < 2)
        {
            throw std::runtime_error("Invalid instruction format at line " +
                                     std::to_string(line_number) + ": " + line);
        }

        const char direction_char = line[0];
        const std::string_view number_part(line.data() + 1, line.length() - 1);

        int steps = 0;
        const auto [ptr, ec] = std::from_chars(number_part.data(),
                                               number_part.data() + number_part.size(),
                                               steps);

        if (ec == std::errc::invalid_argument)
        {
            throw std::runtime_error("Invalid number format at line " +
                                     std::to_string(line_number) + ": " + line);
        }
        if (ec == std::errc::result_out_of_range)
        {
            throw std::runtime_error("Number out of range at line " +
                                     std::to_string(line_number) + ": " + line);
        }
        if (steps < 0)
        {
            throw std::runtime_error("Negative steps value at line " +
                                     std::to_string(line_number) + ": " + line);
        }

        try
        {
            instructions.emplace_back(direction_char, steps);
        }
        catch (const std::invalid_argument &e)
        {
            throw std::runtime_error(std::string("At line ") +
                                     std::to_string(line_number) + ": " + e.what());
        }
    }

    instructions.shrink_to_fit();
    return instructions;
}

/**
 * @brief Solve Advent of Code 2025 Day 1 Part 1.
 *
 * Simulates movement on a circular dial with positions 0-99. Starting at position 50,
 * executes all instructions and counts how many times the position lands exactly on 0.
 * The dial wraps around (position 0 follows position 99 and vice versa).
 *
 * @param file_path Path to the input file containing instructions
 * @return The number of times the position lands on 0
 * @throws std::runtime_error if the file cannot be read or contains invalid data
 */
[[nodiscard]] unsigned int advent_of_code_2025_day1_part1(const std::filesystem::path &file_path)
{
    const auto instructions = read_input(file_path);

    constexpr unsigned int kDialRange = 99U;
    constexpr unsigned int kStartingPosition = 50U;
    constexpr unsigned int kDialSize = kDialRange + 1U;

    unsigned int position = kStartingPosition;
    unsigned int zero_counter = 0U;

    for (const auto &[direction, steps] : instructions)
    {
        const auto unsigned_steps = static_cast<unsigned int>(steps);
        const auto effective_steps = unsigned_steps % kDialSize;

        switch (direction)
        {
        case Direction::Right:
            position = (position + effective_steps) % kDialSize;
            break;
        case Direction::Left:
            position = (position + kDialSize - effective_steps) % kDialSize;
            break;
        }

        if (position == 0U)
        {
            ++zero_counter;
        }
    }

    return zero_counter;
}

/**
 * @brief Solve Advent of Code 2025 Day 1 Part 2.
 *
 * Similar to Part 1, but counts how many times position 0 is crossed during each
 * movement, not just when landing on it. For example, moving from position 98 to
 * position 2 crosses 0 once (98->99->0->1->2).
 *
 * Uses an infinite number line representation where position can be negative,
 * then maps back to the 0-99 dial range. This allows for easier calculation
 * of zero crossings during movement.
 *
 * @param file_path Path to the input file containing instructions
 * @return Total number of times position 0 is crossed
 * @throws std::runtime_error if the file cannot be read or contains invalid data
 */
[[nodiscard]] unsigned int advent_of_code_2025_day1_part2(const std::filesystem::path &file_path)
{
    const auto instructions = read_input(file_path);

    constexpr unsigned int kDialRange = 99U;
    constexpr unsigned int kStartingPosition = 50U;
    constexpr int kDialSize = static_cast<int>(kDialRange + 1U);

    int position = static_cast<int>(kStartingPosition);
    unsigned int zero_counter = 0U;

    for (const auto &[direction, steps] : instructions)
    {
        unsigned int zero_hits = 0U;
        const int old_position = position;

        switch (direction)
        {
        case Direction::Right:
        {
            const int end_pos = old_position + steps;
            const int first_zero = ((old_position / kDialSize) + 1) * kDialSize;

            if (first_zero <= end_pos)
            {
                zero_hits = static_cast<unsigned int>(((end_pos - first_zero) / kDialSize) + 1);
            }

            position = end_pos % kDialSize;
            break;
        }
        case Direction::Left:
        {
            const int end_pos_raw = old_position - steps;
            int first_zero_in_range = (old_position / kDialSize) * kDialSize;

            if (first_zero_in_range == old_position)
            {
                first_zero_in_range -= kDialSize;
            }

            if (first_zero_in_range >= end_pos_raw)
            {
                zero_hits = static_cast<unsigned int>(((first_zero_in_range - end_pos_raw) / kDialSize) + 1);
            }

            int end_pos_mod = end_pos_raw % kDialSize;
            if (end_pos_mod < 0)
            {
                end_pos_mod += kDialSize;
            }
            position = end_pos_mod;
            break;
        }
        }

        zero_counter += zero_hits;
    }

    return zero_counter;
}

/**
 * @brief Main entry point of the program.
 *
 * Executes both parts of the Advent of Code 2025 Day 1 challenge on both
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
        const auto result1_example = advent_of_code_2025_day1_part1(example_file);
        std::cout << "Total zero count - " << result1_example << std::endl;

        std::cout << "=== Part 2 (method 0x434C49434B) ===" << std::endl;
        const auto result2_example = advent_of_code_2025_day1_part2(example_file);
        std::cout << "Total zero count - " << result2_example << std::endl;

        std::cout << "\n=== input.txt ===" << std::endl;
        std::cout << "=== Part 1 ===" << std::endl;
        const auto result1 = advent_of_code_2025_day1_part1(input_file);
        std::cout << "Total zero count - " << result1 << std::endl;

        std::cout << "=== Part 2 (method 0x434C49434B) ===" << std::endl;
        const auto result2 = advent_of_code_2025_day1_part2(input_file);
        std::cout << "Total zero count - " << result2 << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}