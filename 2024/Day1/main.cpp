#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <stdexcept>
#include <filesystem>
#include <utility>
#include <numeric>

struct InputData
{
    std::vector<int> left_column;
    std::vector<int> right_column;

    [[nodiscard]] size_t size() const noexcept { return left_column.size(); }
    [[nodiscard]] bool empty() const noexcept { return left_column.empty(); }
};

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

    InputData data;

    const auto file_size = std::filesystem::file_size(file_path);
    constexpr size_t kAvgBytesPerLine = 10;
    const size_t estimated_lines = file_size / kAvgBytesPerLine;

    data.left_column.reserve(estimated_lines);
    data.right_column.reserve(estimated_lines);

    int left_number, right_number;
    while (file >> left_number >> right_number)
    {
        data.left_column.emplace_back(left_number);
        data.right_column.emplace_back(right_number);
    }

    if (!file.eof())
    {
        throw std::runtime_error("Error reading file: " + file_path.string() +
                                 " - invalid format at line " +
                                 std::to_string(data.left_column.size() + 1));
    }

    data.left_column.shrink_to_fit();
    data.right_column.shrink_to_fit();

    return data;
}

[[nodiscard]] long long advent_of_code_2024_day1_part1(const std::filesystem::path &file_path)
{
    auto data = read_input(file_path);

    std::sort(data.left_column.begin(), data.left_column.end());
    std::sort(data.right_column.begin(), data.right_column.end());

    const long long distance_sum = std::transform_reduce(
        data.left_column.cbegin(), data.left_column.cend(),
        data.right_column.cbegin(),
        0LL,
        std::plus<>{},
        [](const int left, const int right)
        {
            return std::abs(left - right);
        });

    return distance_sum;
}

[[nodiscard]] long long advent_of_code_2024_day1_part2(const std::filesystem::path &file_path)
{
    const auto data = read_input(file_path);

    std::unordered_map<int, int> right_column_count;
    right_column_count.reserve(data.right_column.size());

    for (const auto num : data.right_column)
    {
        ++right_column_count[num];
    }

    long long similarity_sum = 0LL;
    for (const auto left_num : data.left_column)
    {
        if (const auto it = right_column_count.find(left_num); it != right_column_count.end())
        {
            similarity_sum += static_cast<long long>(left_num) * it->second;
        }
    }

    return similarity_sum;
}

int main()
{
    try
    {
        const std::filesystem::path example_file = "input_example.txt";
        const std::filesystem::path input_file = "input.txt";

        std::cout << "=== input_example.txt ===" << std::endl;
        std::cout << "=== Part 1 ===" << std::endl;
        const auto result1_example = advent_of_code_2024_day1_part1(example_file);
        std::cout << "Sum of distances: " << result1_example << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2_example = advent_of_code_2024_day1_part2(example_file);
        std::cout << "Similarity score: " << result2_example << std::endl;

        std::cout << std::endl;

        std::cout << "=== input.txt ===" << std::endl;
        std::cout << "=== Part 1 ===" << std::endl;
        const auto result1 = advent_of_code_2024_day1_part1(input_file);
        std::cout << "Sum of distances: " << result1 << std::endl;

        std::cout << "=== Part 2 ===" << std::endl;
        const auto result2 = advent_of_code_2024_day1_part2(input_file);
        std::cout << "Similarity score: " << result2 << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}