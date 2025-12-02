#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <string_view>
#include <optional>
#include <expected>

struct Instruction
{
    char direction;
    int steps;

    Instruction(char dir, int st) : direction(dir), steps(st) {}
};

[[nodiscard]] std::optional<std::vector<Instruction>> readInput(std::string_view filename)
{
    std::ifstream file(filename.data());
    if (!file.is_open())
    {
        std::cerr << "Can't open file!!! - " << filename << std::endl;
        return std::nullopt;
    }

    std::vector<Instruction> instructions;
    std::string line;

    while (file >> line)
    {
        if (line.length() > 1)
        {
            char direction = line[0];

            try
            {
                int steps = std::stoi(line.substr(1));

                if (steps < 0)
                {
                    std::cerr << "Negative steps value: " << steps << std::endl;
                    continue;
                }

                instructions.emplace_back(direction, steps);
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << "Invalid number format: " << line << std::endl;
                continue;
            }
            catch (const std::out_of_range &e)
            {
                std::cerr << "Number out of range: " << line << std::endl;
                continue;
            }
        }
    }

    return instructions;
}

[[nodiscard]] unsigned int advent_of_code_2025_day1_part1(const std::vector<Instruction> &instructions)
{
    constexpr unsigned int kDialRange = 99U;
    constexpr unsigned int kStartingPosition = 50U;
    constexpr unsigned int kDialSize = kDialRange + 1;

    unsigned int position = kStartingPosition;
    unsigned int zero_counter = 0;

    std::cout << "Starting position - " << position << std::endl;

    for (const auto &[direction, steps] : instructions)
    {
        const auto unsigned_steps = static_cast<unsigned int>(steps);
        const auto effective_steps = unsigned_steps % kDialSize;

        switch (direction)
        {
        case 'R':
            position = (position + effective_steps) % kDialSize;
            break;
        case 'L':
            position = (position + kDialSize - effective_steps) % kDialSize;
            break;
        default:
            std::cerr << "Unknown instruction: " << direction << std::endl;
            continue;
        }

        if (position == 0)
        {
            ++zero_counter;
        }
        /*
        std::cout << "Current instruction - " << direction << steps
                  << ", position after instruction - " << position
                  << std::endl;
        */
    }

    std::cout << "Total zero count - " << zero_counter << std::endl;
    return zero_counter;
}

[[nodiscard]] unsigned int advent_of_code_2025_day1_part2(const std::vector<Instruction> &instructions)
{
    constexpr unsigned int kDialRange = 99U;
    constexpr unsigned int kStartingPosition = 50U;
    constexpr int kDialSize = kDialRange + 1;

    int position = kStartingPosition;
    unsigned int zero_counter = 0;

    std::cout << "Starting position - " << position << std::endl;

    for (const auto &[direction, steps] : instructions)
    {
        unsigned int zero_hits = 0;
        int old_position = position;

        if (direction == 'R')
        {
            int end_pos = old_position + steps;

            int first_zero = ((old_position / kDialSize) + 1) * kDialSize;
            if (first_zero <= end_pos)
            {
                zero_hits = ((end_pos - first_zero) / kDialSize) + 1;
            }

            position = end_pos % kDialSize;
        }
        else if (direction == 'L')
        {
            int end_pos_raw = old_position - steps;

            int first_zero_in_range = (old_position / kDialSize) * kDialSize;
            if (first_zero_in_range == old_position)
            {
                first_zero_in_range -= kDialSize;
            }

            if (first_zero_in_range >= end_pos_raw)
            {
                zero_hits = ((first_zero_in_range - end_pos_raw) / kDialSize) + 1;
            }

            int end_pos_mod = end_pos_raw % kDialSize;
            if (end_pos_mod < 0)
            {
                end_pos_mod += kDialSize;
            }
            position = end_pos_mod;
        }
        else
        {
            std::cerr << "Unknown instruction: " << direction << std::endl;
            continue;
        }

        zero_counter += zero_hits;

        /*
        std::cout << "Current instruction - " << direction << steps
                  << ", position after instruction - " << position
                  << ", zero hits - " << zero_hits
                  << std::endl;
        */
    }

    std::cout << "Total zero count (method 0x434C49434B) - " << zero_counter << std::endl;
    return zero_counter;
}

int main()
{
    if (auto instructions = readInput("C:\\DATA\\Courses\\AdventOfCode\\2025\\Day1\\input.txt"))
    {
        std::cout << "=== Part 1 ===" << std::endl;
        const auto result1 = advent_of_code_2025_day1_part1(*instructions);

        std::cout << "\n=== Part 2 (method 0x434C49434B) ===" << std::endl;
        const auto result2 = advent_of_code_2025_day1_part2(*instructions);
        std::cout << "\n=== RESULTS ===" << std::endl;
        std::cout << "Part 1: " << result1 << std::endl;
        std::cout << "Part 2: " << result2 << std::endl;

        return 0;
    }
    else
    {
        std::cerr << "Failed to read input file" << std::endl;
        return 1;
    }
}