#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <optional>
#include <filesystem>
#include <string_view>
#include <algorithm>
#include <charconv>
#include <stdexcept>

/**
 * @struct Range
 * @brief Represents a numeric range with start and end values.
 *
 * A range consists of two long long integers representing the start and end
 * of an inclusive range. The start value must be less than or equal to the end value.
 */
struct Range
{
    long long start; /// Start of the range (inclusive)
    long long end;   /// End of the range (inclusive)

    /**
     * @brief Create a Range from a string in "start-end" format.
     *
     * Parses a string containing two numbers separated by a dash.
     * Handles negative numbers and trims whitespace. The pattern cannot
     * start with zero (e.g., "01-10" is invalid).
     *
     * @param range_str The string to parse
     * @return Range if parsing succeeded, std::nullopt otherwise
     */
    [[nodiscard]] static constexpr std::optional<Range> from_string(std::string_view range_str)
    {
        // Remove whitespace from the beginning and end
        while (!range_str.empty() && std::isspace(static_cast<unsigned char>(range_str.front())))
        {
            range_str.remove_prefix(1);
        }
        while (!range_str.empty() && std::isspace(static_cast<unsigned char>(range_str.back())))
        {
            range_str.remove_suffix(1);
        }

        const auto dash_pos = range_str.find('-', 1); // Search from position 1 (may have negative number)
        if (dash_pos == std::string_view::npos)
            return std::nullopt;

        const auto start_str = range_str.substr(0, dash_pos);
        const auto end_str = range_str.substr(dash_pos + 1);

        long long start = 0;
        long long end = 0;

        // Use std::from_chars for better performance
        auto [ptr1, ec1] = std::from_chars(start_str.data(), start_str.data() + start_str.size(), start);
        if (ec1 != std::errc{})
            return std::nullopt;

        auto [ptr2, ec2] = std::from_chars(end_str.data(), end_str.data() + end_str.size(), end);
        if (ec2 != std::errc{})
            return std::nullopt;

        if (start > end)
            return std::nullopt;

        return Range{start, end};
    }
};

/**
 * @brief Read and parse ranges from an input file.
 *
 * Reads a file containing comma-separated ranges in the format
 * "start1-end1,start2-end2,...". Invalid ranges are silently skipped.
 *
 * @param file_path Path to the input file
 * @return Vector of parsed Range objects
 * @throws std::runtime_error if the file doesn't exist or cannot be opened
 */
[[nodiscard]] std::vector<Range> read_input(const std::filesystem::path &file_path)
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

    std::vector<Range> path_ranges;

    // Read entire file content
    std::ostringstream buffer;
    buffer << file.rdbuf();
    const std::string content = buffer.str();

    std::stringstream ss{content};
    std::string range_str;

    // Parse comma-separated ranges
    while (std::getline(ss, range_str, ','))
    {
        const auto range = Range::from_string(range_str);
        if (range)
        {
            path_ranges.emplace_back(*range);
        }
    }

    path_ranges.shrink_to_fit();
    return path_ranges;
}

/**
 * @brief Check if an ID is invalid (consists of two identical halves).
 *
 * An ID is considered invalid if it has an even number of digits and
 * the first half is identical to the second half. The first half cannot
 * start with zero.
 *
 * @param id The ID to check
 * @return true if the ID has repeating halves, false otherwise
 */
[[nodiscard]] bool has_repeating_halves(const long long id) noexcept
{
    const auto str = std::to_string(id);
    const auto len = str.length();

    // Must have an even number of digits
    if (len % 2 != 0)
        return false;

    const auto half = len / 2;
    const std::string_view first_half(str.data(), half);
    const std::string_view second_half(str.data() + half, half);

    // First half cannot start with zero
    if (first_half.front() == '0')
        return false;

    return first_half == second_half;
}

/**
 * @brief Solve Advent of Code 2025 Day 2 Part 1.
 *
 * Calculates the sum of all invalid IDs within the given ranges.
 * An ID is invalid if it consists of two identical halves (e.g., 1212, 5555).
 *
 * @param file_path Path to the input file containing ranges
 * @return The sum of all invalid IDs
 * @throws std::runtime_error if the file cannot be read or contains invalid data
 */
[[nodiscard]] long long advent_of_code_2025_day2_part1(const std::filesystem::path &file_path)
{
    const auto ranges = read_input(file_path);
    long long sum = 0LL;

    for (const auto &range : ranges)
    {
        for (long long id = range.start; id <= range.end; ++id)
        {
            if (has_repeating_halves(id))
            {
                sum += id;
            }
        }
    }

    return sum;
}

/**
 * @brief Check if an ID consists of a repeating pattern (at least 2 times).
 *
 * An ID has a repeating pattern if it can be divided into identical segments
 * repeated at least twice. For example: 123123 (pattern: 123, repeated 2 times),
 * 77777 (pattern: 7, repeated 5 times). The pattern cannot start with zero.
 *
 * @param id The ID to check
 * @return true if the ID contains a repeating pattern, false otherwise
 */
[[nodiscard]] bool has_repeating_pattern(const long long id) noexcept
{
    const auto str = std::to_string(id);
    const auto len = str.length();

    // Check all possible pattern lengths (from 1 to len/2)
    for (std::size_t pattern_len = 1; pattern_len <= len / 2; ++pattern_len)
    {
        // Check if number length is a multiple of pattern length
        if (len % pattern_len != 0)
            continue;

        const std::string_view pattern(str.data(), pattern_len);

        // Pattern cannot start with zero
        if (pattern.front() == '0')
            continue;

        const auto repetitions = len / pattern_len;

        // Must be repeated at least 2 times
        if (repetitions < 2)
            continue;

        // Check if entire number consists of repetitions of this pattern
        bool is_repeated = true;
        for (std::size_t i = 1; i < repetitions; ++i)
        {
            const std::string_view segment(str.data() + i * pattern_len, pattern_len);
            if (segment != pattern)
            {
                is_repeated = false;
                break;
            }
        }

        if (is_repeated)
            return true;
    }

    return false;
}

/**
 * @brief Solve Advent of Code 2025 Day 2 Part 2.
 *
 * Calculates the sum of all IDs with repeating patterns within the given ranges.
 * An ID has a repeating pattern if it consists of identical segments repeated
 * at least twice (e.g., 123123, 77777, 454545).
 *
 * @param file_path Path to the input file containing ranges
 * @return The sum of all IDs with repeating patterns
 * @throws std::runtime_error if the file cannot be read or contains invalid data
 */
[[nodiscard]] long long advent_of_code_2025_day2_part2(const std::filesystem::path &file_path)
{
    const auto ranges = read_input(file_path);
    long long sum = 0LL;

    for (const auto &range : ranges)
    {
        for (long long id = range.start; id <= range.end; ++id)
        {
            if (has_repeating_pattern(id))
            {
                sum += id;
            }
        }
    }

    return sum;
}

/**
 * @brief Main entry point of the program.
 *
 * Executes both parts of the Advent of Code 2025 Day 2 challenge on both
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
        const auto result1_example = advent_of_code_2025_day2_part1(example_file);
        std::cout << "Sum of invalid IDs: " << result1_example << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2_example = advent_of_code_2025_day2_part2(example_file);
        std::cout << "Sum of pattern IDs: " << result2_example << std::endl;

        std::cout << "\n=== input.txt ===" << std::endl;
        std::cout << "=== Part 1 ===" << std::endl;
        const auto result1 = advent_of_code_2025_day2_part1(input_file);
        std::cout << "Sum of invalid IDs: " << result1 << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2 = advent_of_code_2025_day2_part2(input_file);
        std::cout << "Sum of pattern IDs: " << result2 << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}