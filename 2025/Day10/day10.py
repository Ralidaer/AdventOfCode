import re
from itertools import combinations
import numpy as np
from scipy.optimize import linprog
import pulp

def parse_line(line):
    """Parse a single machine line to extract lights, buttons, and joltage."""
    # Extract indicator lights pattern
    lights_match = re.search(r'\[(.*?)\]', line)
    lights_str = lights_match.group(1)
    target_lights = [1 if c == '#' else 0 for c in lights_str]
    
    # Extract button configurations
    buttons = []
    button_matches = re.findall(r'\(([0-9,]+)\)', line)
    for button_str in button_matches:
        button_indices = [int(x) for x in button_str.split(',')]
        buttons.append(button_indices)
    
    # Extract joltage requirements
    joltage_match = re.search(r'\{([0-9,]+)\}', line)
    target_joltage = []
    if joltage_match:
        joltage_str = joltage_match.group(1)
        target_joltage = [int(x) for x in joltage_str.split(',')]
    
    return target_lights, buttons, target_joltage

def find_min_presses(target, buttons):
    """
    Find minimum number of button presses to achieve target configuration.
    This is a lights-out puzzle - we need to find which buttons to press
    (and how many times) to flip the right lights.
    
    Since each button toggles (XOR operation), pressing it twice = not pressing.
    So we only care about pressing each button 0 or 1 times.
    This becomes a system of linear equations over GF(2).
    """
    n_lights = len(target)
    n_buttons = len(buttons)
    
    # Try all combinations of buttons (2^n_buttons possibilities)
    # For small n_buttons this is feasible
    min_presses = float('inf')
    
    for mask in range(1 << n_buttons):
        # Calculate resulting light configuration
        lights = [0] * n_lights
        presses = 0
        
        for i in range(n_buttons):
            if mask & (1 << i):
                presses += 1
                for light_idx in buttons[i]:
                    if light_idx < n_lights:
                        lights[light_idx] ^= 1
        
        if lights == target:
            min_presses = min(min_presses, presses)
    
    return min_presses if min_presses != float('inf') else 0

def advent_of_code_2025_day10_part1(filename):
    """Solve the factory initialization problem - Part 1."""
    total_presses = 0
    
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            
            target, buttons, _ = parse_line(line)
            min_presses = find_min_presses(target, buttons)
            print(f"Machine: {line[:30]}... -> Min presses: {min_presses}")
            total_presses += min_presses
    
    return total_presses

def find_min_presses_joltage(target_joltage, buttons):
    """
    Find minimum number of button presses using Integer Linear Programming.
    This guarantees optimal solution.
    """
    n_counters = len(target_joltage)
    n_buttons = len(buttons)
    
    if n_counters == 0:
        return 0
    
    # Use ILP solver for all cases - it's fast and gives optimal solution
    return find_min_presses_joltage_lp(target_joltage, buttons)

def find_min_presses_joltage_lp(target_joltage, buttons):
    """
    Use Integer Linear Programming to find minimum button presses.
    This guarantees optimal solution unlike regular LP with rounding.
    """
    n_counters = len(target_joltage)
    n_buttons = len(buttons)
    
    # Create the problem
    prob = pulp.LpProblem("MinButtonPresses", pulp.LpMinimize)
    
    # Decision variables: number of times to press each button (non-negative integers)
    button_vars = [pulp.LpVariable(f"button_{i}", lowBound=0, cat='Integer') for i in range(n_buttons)]
    
    # Objective: minimize total button presses
    prob += pulp.lpSum(button_vars)
    
    # Constraints: for each counter, sum of button presses affecting it must equal target
    for counter_idx in range(n_counters):
        constraint = []
        for btn_idx in range(n_buttons):
            if counter_idx in buttons[btn_idx]:
                constraint.append(button_vars[btn_idx])
        
        if constraint:
            prob += pulp.lpSum(constraint) == target_joltage[counter_idx]
    
    # Solve
    prob.solve(pulp.PULP_CBC_CMD(msg=0))
    
    # Check if solution found
    if prob.status == pulp.LpStatusOptimal:
        total = sum(int(var.varValue) for var in button_vars)
        return total
    
    # If ILP failed, fall back to exhaustive for very small cases
    if n_buttons <= 8:
        return find_min_presses_joltage_exhaustive(target_joltage, buttons)
    
    return 0

def find_min_presses_joltage_exhaustive(target_joltage, buttons):
    """
    Exhaustive search for minimum button presses.
    Try different combinations up to a reasonable limit.
    """
    from itertools import product
    
    n_counters = len(target_joltage)
    n_buttons = len(buttons)
    max_target = max(target_joltage) if target_joltage else 0
    
    # Upper bound: press buttons individually up to max needed
    max_presses_per_button = max_target * 2  # generous upper bound
    
    # For small examples, we can try different combinations
    # Use BFS-like approach: try increasing total presses until we find solution
    
    for total_presses in range(sum(target_joltage) + 1):
        # Generate all ways to distribute total_presses among buttons
        # This is still exponential but bounded
        if n_buttons > 10:  # Too many buttons for exhaustive
            break
            
        # Try to find any valid distribution
        found = try_distribute_presses(total_presses, n_buttons, buttons, target_joltage)
        if found is not None:
            return total_presses
    
    return 0  # Fallback

def try_distribute_presses(total_presses, n_buttons, buttons, target_joltage):
    """Try to distribute total_presses among buttons to achieve target."""
    from itertools import combinations_with_replacement
    
    n_counters = len(target_joltage)
    
    # Generate all partitions of total_presses into n_buttons non-negative integers
    def generate_partitions(n, k):
        """Generate all ways to partition n into k non-negative integers."""
        if k == 1:
            yield [n]
            return
        for i in range(n + 1):
            for rest in generate_partitions(n - i, k - 1):
                yield [i] + rest
    
    for partition in generate_partitions(total_presses, n_buttons):
        # Check if this partition achieves the target
        counters = [0] * n_counters
        for btn_idx, presses in enumerate(partition):
            for counter_idx in buttons[btn_idx]:
                if counter_idx < n_counters:
                    counters[counter_idx] += presses
        
        if counters == target_joltage:
            return partition
    
    return None

def advent_of_code_2025_day10_part2(filename):
    """Solve the factory initialization problem - Part 2 (Joltage)."""
    total_presses = 0
    
    with open(filename, 'r') as f:
        for line_num, line in enumerate(f, 1):
            line = line.strip()
            if not line:
                continue
            
            _, buttons, target_joltage = parse_line(line)
            min_presses = find_min_presses_joltage(target_joltage, buttons)
            print(f"Machine {line_num}: {line[:30]}... -> Min presses: {min_presses}")
            total_presses += min_presses
    
    return total_presses

if __name__ == "__main__":
    import os
    # Get the directory where this script is located
    script_dir = os.path.dirname(os.path.abspath(__file__))
    example_file = os.path.join(script_dir, "input_example.txt")
    input_file = os.path.join(script_dir, "input.txt")
    
    print("=== input_example.txt ===")
    print("=== Part 1: Indicator Lights ===")
    result_example_part1 = advent_of_code_2025_day10_part1(example_file)
    print(f"\nTotal minimum button presses (Part 1): {result_example_part1}")
    print("\n=== Part 2: Joltage Levels ===")
    result_example_part2 = advent_of_code_2025_day10_part2(example_file)
    print(f"\nTotal minimum button presses (Part 2): {result_example_part2}")

    print("\n=== input.txt ===")
    print("=== Part 1: Indicator Lights ===")
    result_part1 = advent_of_code_2025_day10_part1(input_file)
    print(f"\nTotal minimum button presses (Part 1): {result_part1}")
    print("\n=== Part 2: Joltage Levels ===")
    result_part2 = advent_of_code_2025_day10_part2(input_file)
    print(f"\nTotal minimum button presses (Part 2): {result_part2}")

