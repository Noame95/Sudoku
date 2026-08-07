import subprocess
from pathlib import Path


class Bridge:
    NO_SOLUTION = "NO_SOLUTION"

    @staticmethod
    def board_to_string(board):
        board_string = ""
        for row in board:
            for number in row:
                board_string += str(number) + " "
            board_string += "\n"
        return board_string

    @staticmethod
    def string_to_board(board_string):
        numbers = board_string.split()
        solved_board = []
        current_row = []
        for number in numbers:
            current_row.append(int(number))
            if len(current_row) == 9:
                solved_board.append(current_row)
                current_row = []
        return solved_board

    @staticmethod
    def run_cpp(cpp_input):
        current_folder = Path(__file__).resolve().parent
        cpp_program = (
            current_folder.parent
            / "cpp"
            / "cmake-build-debug-visual-studio"
            / "cpp.exe"
        )
        if not cpp_program.exists():
            raise FileNotFoundError
        result = subprocess.run(
            [cpp_program],
            input=cpp_input,
            text=True,
            capture_output=True,
            timeout=10
        )
        output = result.stdout.strip()
        if result.returncode != 0:
            raise RuntimeError(output or result.stderr)
        return output

    @staticmethod
    def solve_sudoku(board):
        board_string = Bridge.board_to_string(board)
        output = Bridge.run_cpp(f"solve\n{board_string}")
        if output == Bridge.NO_SOLUTION:
            return None
        return Bridge.string_to_board(output)

    @staticmethod
    def generate_sudoku(difficulty):
        cpp_input = f"generate\n{difficulty}\n"
        output = Bridge.run_cpp(cpp_input)
        return Bridge.string_to_board(output)