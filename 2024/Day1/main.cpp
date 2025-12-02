#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>

using namespace std;

void readInput(const string &filename, vector<int> &left, vector<int> &right)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Nie można otworzyć pliku: " << filename << endl;
        return;
    }

    int leftNum, rightNum;
    while (file >> leftNum >> rightNum)
    {
        left.push_back(leftNum);
        right.push_back(rightNum);
    }

    file.close();
}

void advent_of_code_2024_day1_part1()
{
    vector<int> left, right;

    readInput("C:\\DATA\\Courses\\AdventOfCode\\2024\\Day1\\input.txt", left, right);

    cout << "Wczytano " << left.size() << " par liczb" << endl;
    cout << "Pierwsze 5 par:" << endl;
    for (int i = 0; i < min(5, (int)left.size()); i++)
    {
        cout << left[i] << " " << right[i] << endl;
    }

    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());

    cout << "Pierwsze 5 par:" << endl;
    for (int i = 0; i < min(5, (int)left.size()); i++)
    {
        cout << left[i] << " " << right[i] << endl;
    }

    cout << "left size - " << left.size() << " right size - " << right.size() << endl;

    long long distance_sum = 0;
    for (size_t i = 0; i < left.size(); i++)
    {
        distance_sum += abs(left[i] - right[i]);
    }

    cout << "Suma odległości: " << distance_sum << endl;
}

void advent_of_code_2024_day1_part2()
{
    vector<int> left, right;

    readInput("C:\\DATA\\Courses\\AdventOfCode\\2024\\Day1\\input.txt", left, right);

    // Tworzymy mapy z ilością wystąpień każdej liczby
    map<int, int> left_count;
    map<int, int> right_count;

    for (int num : left)
    {
        left_count[num]++;
    }

    for (int num : right)
    {
        right_count[num]++;
    }

    // Wyświetlamy przykładowe wystąpienia
    cout << "Przykładowe wystąpienia w lewej kolumnie:" << endl;
    int count = 0;
    for (auto &pair : left_count)
    {
        cout << "Liczba " << pair.first << " występuje " << pair.second << " razy" << endl;
        if (++count >= 5)
            break;
    }

    cout << "\nPrzykładowe wystąpienia w prawej kolumnie:" << endl;
    count = 0;
    for (auto &pair : right_count)
    {
        cout << "Liczba " << pair.first << " występuje " << pair.second << " razy" << endl;
        if (++count >= 5)
            break;
    }

    long long similarity_sum = 0;
    for (auto &left_num : left)
    {
        if (right_count[left_num] > 0)
        {
            similarity_sum += left_num * right_count[left_num];
        }
    }

    cout << "Suma podobieństw: " << similarity_sum << endl;
}

int main()
{
    advent_of_code_2024_day1_part1();

    advent_of_code_2024_day1_part2();

    return 0;
}