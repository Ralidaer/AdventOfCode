"""
Advent of Code 2025 - Day 2 Solution

Part 1: Calculates the sum of all invalid IDs within given ranges.
        An ID is invalid if it consists of two identical halves (e.g., 1212, 5555).
Part 2: Calculates the sum of all IDs with repeating patterns.
        An ID has a repeating pattern if it consists of identical segments 
        repeated at least twice (e.g., 123123, 77777, 454545).
"""

from pathlib import Path
from typing import List, Optional
from dataclasses import dataclass


@dataclass
class Range:
    """Represents a numeric range with start and end values."""
    start: int
    end: int
    
    @staticmethod
    def from_string(range_str: str) -> Optional['Range']:
        """
        Create a Range from a string in "start-end" format.
        
        Parses a string containing two numbers separated by a dash.
        Handles negative numbers and trims whitespace. The pattern cannot
        start with zero (e.g., "01-10" is invalid).
        
        Args:
            range_str: The string to parse
            
        Returns:
            Range if parsing succeeded, None otherwise
        """
        range_str = range_str.strip()
        
        # Search for dash from position 1 (may have negative number)
        dash_pos = range_str.find('-', 1)
        if dash_pos == -1:
            return None
        
        start_str = range_str[:dash_pos]
        end_str = range_str[dash_pos + 1:]
        
        try:
            start = int(start_str)
            end = int(end_str)
        except ValueError:
            return None
        
        if start > end:
            return None
        
        return Range(start, end)


def read_input(file_path: Path) -> List[Range]:
    """
    Read and parse ranges from an input file.
    
    Reads a file containing comma-separated ranges in the format
    "start1-end1,start2-end2,...". Invalid ranges are silently skipped.
    
    Args:
        file_path: Path to the input file
        
    Returns:
        List of parsed Range objects
        
    Raises:
        FileNotFoundError: If the file doesn't exist
        RuntimeError: If the file cannot be opened
    """
    if not file_path.exists():
        raise FileNotFoundError(f"File does not exist: {file_path}")
    
    try:
        with open(file_path, 'r') as file:
            content = file.read()
    except Exception as e:
        raise RuntimeError(f"Cannot open file: {file_path}") from e
    
    path_ranges = []
    
    # Parse comma-separated ranges
    range_strings = content.split(',')
    for range_str in range_strings:
        range_obj = Range.from_string(range_str)
        if range_obj:
            path_ranges.append(range_obj)
    
    return path_ranges


def has_repeating_halves(id_num: int) -> bool:
    """
    Check if an ID is invalid (consists of two identical halves).
    
    An ID is considered invalid if it has an even number of digits and
    the first half is identical to the second half. The first half cannot
    start with zero.
    
    Args:
        id_num: The ID to check
        
    Returns:
        True if the ID has repeating halves, False otherwise
    """
    str_id = str(id_num)
    length = len(str_id)
    
    # Must have an even number of digits
    if length % 2 != 0:
        return False
    
    half = length // 2
    first_half = str_id[:half]
    second_half = str_id[half:]
    
    # First half cannot start with zero
    if first_half[0] == '0':
        return False
    
    return first_half == second_half


def advent_of_code_2025_day2_part1(file_path: Path) -> int:
    """
    Solve Advent of Code 2025 Day 2 Part 1.
    
    Calculates the sum of all invalid IDs within the given ranges.
    An ID is invalid if it consists of two identical halves (e.g., 1212, 5555).
    
    Args:
        file_path: Path to the input file containing ranges
        
    Returns:
        The sum of all invalid IDs
    """
    ranges = read_input(file_path)
    total_sum = 0
    
    for range_obj in ranges:
        for id_num in range(range_obj.start, range_obj.end + 1):
            if has_repeating_halves(id_num):
                total_sum += id_num
    
    return total_sum


def has_repeating_pattern(id_num: int) -> bool:
    """
    Check if an ID consists of a repeating pattern (at least 2 times).
    
    An ID has a repeating pattern if it can be divided into identical segments
    repeated at least twice. For example: 123123 (pattern: 123, repeated 2 times),
    77777 (pattern: 7, repeated 5 times). The pattern cannot start with zero.
    
    Args:
        id_num: The ID to check
        
    Returns:
        True if the ID contains a repeating pattern, False otherwise
    """
    str_id = str(id_num)
    length = len(str_id)
    
    # Check all possible pattern lengths (from 1 to length/2)
    for pattern_len in range(1, length // 2 + 1):
        # Check if number length is a multiple of pattern length
        if length % pattern_len != 0:
            continue
        
        pattern = str_id[:pattern_len]
        
        # Pattern cannot start with zero
        if pattern[0] == '0':
            continue
        
        repetitions = length // pattern_len
        
        # Must be repeated at least 2 times
        if repetitions < 2:
            continue
        
        # Check if entire number consists of repetitions of this pattern
        is_repeated = True
        for i in range(1, repetitions):
            segment = str_id[i * pattern_len:(i + 1) * pattern_len]
            if segment != pattern:
                is_repeated = False
                break
        
        if is_repeated:
            return True
    
    return False


def advent_of_code_2025_day2_part2(file_path: Path) -> int:
    """
    Solve Advent of Code 2025 Day 2 Part 2.
    
    Calculates the sum of all IDs with repeating patterns within the given ranges.
    An ID has a repeating pattern if it consists of identical segments repeated
    at least twice (e.g., 123123, 77777, 454545).
    
    Args:
        file_path: Path to the input file containing ranges
        
    Returns:
        The sum of all IDs with repeating patterns
    """
    ranges = read_input(file_path)
    total_sum = 0
    
    for range_obj in ranges:
        for id_num in range(range_obj.start, range_obj.end + 1):
            if has_repeating_pattern(id_num):
                total_sum += id_num
    
    return total_sum


def main():
    """Main entry point of the program."""
    try:
        # Get the directory where this script is located
        script_dir = Path(__file__).parent
        example_file = script_dir / "input_example.txt"
        input_file = script_dir / "input.txt"
        
        print("=== input_example.txt ===")
        print("=== Part 1 ===")
        result1_example = advent_of_code_2025_day2_part1(example_file)
        print(f"Sum of invalid IDs: {result1_example}")
        
        print("=== Part 2 ===")
        result2_example = advent_of_code_2025_day2_part2(example_file)
        print(f"Sum of pattern IDs: {result2_example}")
        
        print("\n=== input.txt ===")
        print("=== Part 1 ===")
        result1 = advent_of_code_2025_day2_part1(input_file)
        print(f"Sum of invalid IDs: {result1}")
        
        print("=== Part 2 ===")
        result2 = advent_of_code_2025_day2_part2(input_file)
        print(f"Sum of pattern IDs: {result2}")
        
    except Exception as e:
        print(f"Error: {e}")
        return 1
    
    return 0


if __name__ == "__main__":
    exit(main())
