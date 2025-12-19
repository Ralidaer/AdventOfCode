"""
Advent of Code 2025 - Day 1 Solution

Simulates movement on a circular dial with positions 0-99.
Part 1: Counts how many times the position lands exactly on 0.
Part 2: Counts how many times position 0 is crossed during movement.
"""

from enum import Enum
from pathlib import Path
from typing import List, Tuple


class Direction(Enum):
    """Represents the direction of movement on a dial."""
    RIGHT = 'R'  # Move right (clockwise)
    LEFT = 'L'   # Move left (counter-clockwise)


class Instruction:
    """Represents a single movement instruction with direction and step count."""
    
    def __init__(self, direction: str, steps: int):
        """
        Initialize an instruction.
        
        Args:
            direction: Direction character ('R' or 'L')
            steps: Number of steps to move
            
        Raises:
            ValueError: If direction is not 'R' or 'L' or if steps is negative
        """
        if direction not in ('R', 'L'):
            raise ValueError(f"Invalid direction: {direction}")
        if steps < 0:
            raise ValueError(f"Negative steps value: {steps}")
            
        self.direction = Direction(direction)
        self.steps = steps
    
    def __repr__(self) -> str:
        return f"Instruction({self.direction.value}, {self.steps})"


def read_input(file_path: Path) -> List[Instruction]:
    """
    Read and parse instructions from an input file.
    
    Reads a file containing movement instructions, where each line consists of
    a direction character ('R' or 'L') followed by a number of steps.
    
    Args:
        file_path: Path to the input file
        
    Returns:
        List of parsed Instruction objects
        
    Raises:
        FileNotFoundError: If the file doesn't exist
        ValueError: If the file contains invalid instruction format
    """
    if not file_path.exists():
        raise FileNotFoundError(f"File does not exist: {file_path}")
    
    instructions = []
    
    with open(file_path, 'r') as file:
        for line_number, line in enumerate(file, start=1):
            line = line.strip()
            
            if not line:
                continue
            
            if len(line) < 2:
                raise ValueError(
                    f"Invalid instruction format at line {line_number}: {line}"
                )
            
            direction_char = line[0]
            number_part = line[1:]
            
            try:
                steps = int(number_part)
            except ValueError:
                raise ValueError(
                    f"Invalid number format at line {line_number}: {line}"
                )
            
            try:
                instructions.append(Instruction(direction_char, steps))
            except ValueError as e:
                raise ValueError(f"At line {line_number}: {e}")
    
    return instructions


def advent_of_code_2025_day1_part1(file_path: Path) -> int:
    """
    Solve Advent of Code 2025 Day 1 Part 1.
    
    Simulates movement on a circular dial with positions 0-99. Starting at position 50,
    executes all instructions and counts how many times the position lands exactly on 0.
    The dial wraps around (position 0 follows position 99 and vice versa).
    
    Args:
        file_path: Path to the input file containing instructions
        
    Returns:
        The number of times the position lands on 0
    """
    instructions = read_input(file_path)
    
    DIAL_RANGE = 99
    STARTING_POSITION = 50
    DIAL_SIZE = DIAL_RANGE + 1
    
    position = STARTING_POSITION
    zero_counter = 0
    
    for instruction in instructions:
        steps = instruction.steps
        effective_steps = steps % DIAL_SIZE
        
        if instruction.direction == Direction.RIGHT:
            position = (position + effective_steps) % DIAL_SIZE
        else:  # Direction.LEFT
            position = (position - effective_steps) % DIAL_SIZE
        
        if position == 0:
            zero_counter += 1
    
    return zero_counter


def advent_of_code_2025_day1_part2(file_path: Path) -> int:
    """
    Solve Advent of Code 2025 Day 1 Part 2.
    
    Similar to Part 1, but counts how many times position 0 is crossed during each
    movement, not just when landing on it. For example, moving from position 98 to
    position 2 crosses 0 once (98->99->0->1->2).
    
    Uses an infinite number line representation where position can be negative,
    then maps back to the 0-99 dial range. This allows for easier calculation
    of zero crossings during movement.
    
    Args:
        file_path: Path to the input file containing instructions
        
    Returns:
        Total number of times position 0 is crossed
    """
    instructions = read_input(file_path)
    
    DIAL_RANGE = 99
    STARTING_POSITION = 50
    DIAL_SIZE = DIAL_RANGE + 1
    
    position = STARTING_POSITION
    zero_counter = 0
    
    for instruction in instructions:
        zero_hits = 0
        old_position = position
        steps = instruction.steps
        
        if instruction.direction == Direction.RIGHT:
            end_pos = old_position + steps
            first_zero = ((old_position // DIAL_SIZE) + 1) * DIAL_SIZE
            
            if first_zero <= end_pos:
                zero_hits = ((end_pos - first_zero) // DIAL_SIZE) + 1
            
            position = end_pos % DIAL_SIZE
            
        else:  # Direction.LEFT
            end_pos_raw = old_position - steps
            first_zero_in_range = (old_position // DIAL_SIZE) * DIAL_SIZE
            
            if first_zero_in_range == old_position:
                first_zero_in_range -= DIAL_SIZE
            
            if first_zero_in_range >= end_pos_raw:
                zero_hits = ((first_zero_in_range - end_pos_raw) // DIAL_SIZE) + 1
            
            position = end_pos_raw % DIAL_SIZE
        
        zero_counter += zero_hits
    
    return zero_counter


def main():
    """Main entry point of the program."""
    try:
        # Get the directory where this script is located
        script_dir = Path(__file__).parent
        example_file = script_dir / "input_example.txt"
        input_file = script_dir / "input.txt"
        
        print("=== input_example.txt ===")
        print("=== Part 1 ===")
        result1_example = advent_of_code_2025_day1_part1(example_file)
        print(f"Total zero count - {result1_example}")
        
        print("=== Part 2 (method 0x434C49434B) ===")
        result2_example = advent_of_code_2025_day1_part2(example_file)
        print(f"Total zero count - {result2_example}")
        
        print("\n=== input.txt ===")
        print("=== Part 1 ===")
        result1 = advent_of_code_2025_day1_part1(input_file)
        print(f"Total zero count - {result1}")
        
        print("=== Part 2 (method 0x434C49434B) ===")
        result2 = advent_of_code_2025_day1_part2(input_file)
        print(f"Total zero count - {result2}")
        
    except Exception as e:
        print(f"Error: {e}")
        return 1
    
    return 0


if __name__ == "__main__":
    exit(main())
