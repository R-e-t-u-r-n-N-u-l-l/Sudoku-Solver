#include <vector>
#include <string>
#include <stdlib.h>

#include "olcConsoleGameEngine.h"

struct Square {
	int digit;
	int possibilities;
	bool possible[9];

	Square(int digit) {
		this->digit = digit;
		possibilities = 9;

		for (int i = 0; i < 9; i++)
			possible[i] = true;
	}

	bool nextPossibility() {
		for (int i = digit; i < 9; i++) {
			if (possible[i]) {
				digit = i + 1;
				return true;
			}
		}

		return false;
	}
};

class Frame : public olcConsoleGameEngine {

private:
	std::vector<Square> m_originalBoard;
	std::vector<Square> m_btSquares;
	std::vector<Square> m_squares;
	bool m_backtracking = false;
	int m_index = 0;
	int m_solved = 0;

	void drawGrid(const std::vector<Square>& squares) {
		for (int i = 0; i < 9; i++) {
			if (i > 0) {
				DrawLine(ScreenWidth() / 9 * i + i / 3 - 1, 0, ScreenWidth() / 9 * i + i / 3 - 1, ScreenHeight());
				DrawLine(0, ScreenHeight() / 9 * i + i / 3 - 1, ScreenWidth(), ScreenHeight() / 9 * i + i / 3 - 1);

				if (i == 3 || i == 6) {
					DrawLine(ScreenWidth() / 9 * i + i / 3 - 2, 0, ScreenWidth() / 9 * i + i / 3 - 2, ScreenHeight());
					DrawLine(0, ScreenHeight() / 9 * i + i / 3 - 2, ScreenWidth(), ScreenHeight() / 9 * i + i / 3 - 2);
				}
			}

			for (int j = 0; j < 9; j++) {
				if (squares[i + j * 9].digit != 0)
					DrawDigit(ScreenWidth() / 9 * i + ScreenWidth() / 18 - 3 + i / 3, ScreenHeight() / 9 * j + ScreenHeight() / 18 - 5 + j / 3, squares[i + j * 9].digit);
				else {
					/*for (int k = 0; k < 9; k++) {
						if (squares[i + j * 9].possible[k])
							Draw(ScreenWidth() / 9 * i + ScreenWidth() / 18 + i / 3 + k - 8, ScreenHeight() / 9 * j + ScreenHeight() / 18 + j / 3);
						else
							Draw(ScreenWidth() / 9 * i + ScreenWidth() / 18 + i / 3 + k - 8, ScreenHeight() / 9 * j + ScreenHeight() / 18 + j / 3, PIXEL_HALF);
					}*/
				}
			}
		}
	}

	bool solveGrid() {
		bool solveable = false;

		checkGrid();

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (m_squares[j + i * 9].digit != 0)
					continue;

				if (m_squares[j + i * 9].possibilities == 1) {
					for (int k = 0; k < 9; k++) {
						if (m_squares[j + i * 9].possible[k]) {
							m_squares[j + i * 9].digit = k + 1;
							m_solved++;
							solveable = true;
						}
					}
				}
			}
		}

		return solveable;
	}

	void checkGrid() {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				//Row and column check
				for (int k = 0; k < 9; k++) {
					if (m_squares[j + i * 9].digit == 0)
						continue;

					if (m_squares[k + i * 9].digit == 0)
						m_squares[k + i * 9].possible[m_squares[j + i * 9].digit - 1] = false;

					if (m_squares[j + k * 9].digit == 0)
						m_squares[j + k * 9].possible[m_squares[j + i * 9].digit - 1] = false;
				}

				//Box check
				int x = int(j / 3) * 3;
				int y = int(i / 3) * 3;

				for (int k = y; k < y + 3; k++) {
					for (int l = x; l < x + 3; l++) {
						if (m_squares[j + i * 9].digit == 0)
							continue;

						if (m_squares[l + k * 9].digit == 0)
							m_squares[l + k * 9].possible[m_squares[j + i * 9].digit - 1] = false;
					}
				}

				//Check possibilities
				m_squares[j + i * 9].possibilities = 0;
				for (int k = 0; k < 9; k++)
					if (m_squares[j + i * 9].possible[k])
						m_squares[j + i * 9].possibilities++;
			}
		}
	}

	bool valid(const std::vector<Square>& squares, int index) {
		if (squares[index].digit < 0 || squares[index].digit > 9)
			return false;

		int i = floor(index / 9);
		int j = index % 9;

		//Row and column check
		for (int k = 0; k < 9; k++) {
			if (squares[j + i * 9].digit == 0)
				continue;

			if ((squares[k + i * 9].digit == squares[j + i * 9].digit && k + i * 9 != j + i * 9) || 
				(squares[j + k * 9].digit == squares[j + i * 9].digit && j + k * 9 != j + i * 9))
				return false;
		}

		//Box check
		int x = floor(j / 3) * 3;
		int y = floor(i / 3) * 3;

		for (int k = y; k < y + 3; k++) {
			for (int l = x; l < x + 3; l++) {
				if (squares[j + i * 9].digit == 0 || j + i * 9 == l + k * 9)
					continue;

				if (squares[j + i * 9].digit == squares[l + k * 9].digit)
					return false;
			}
		}

		return true;
	}

	void backtrack() {
		if (m_btSquares.empty())
			m_btSquares = m_squares;

		// Slow visible solve
		// Takes a while to solve hard sudoku's
		// It has to draw the grid every frame

		if (m_index < 9 * 9) {
			while (m_squares[m_index].digit != 0 && m_index < 9 * 9 - 1) m_index++;

			if (!m_btSquares[m_index].nextPossibility()) {
				m_btSquares[m_index].digit = 0;
				m_index--;
				while (m_squares[m_index].digit != 0) m_index--;
				return;
			}

			if (valid(m_btSquares, m_index)) m_index++;
		}

		// Fast insvisible solve
		// It solves every sudoku almost instantly
		// Only draws the grid when it's finished

		/*while (m_index < 9 * 9) {
			if (m_squares[m_index].digit != 0) {
				m_index++;
				continue;
			}

			if (!m_btSquares[m_index].nextPossibility()) {
				m_btSquares[m_index].digit = 0;
				m_index--;
				while (m_squares[m_index].digit != 0) m_index--;
				continue;
			}

			if (valid(m_btSquares, m_index)) m_index++;
		}*/
	}

	bool advancedSolve() {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {

				if (m_squares[j + i * 9].digit != 0)
					continue;

				// Unique row check
				for (int k = 0; k < 9; k++) {
					if (m_squares[j + i * 9].possible[k]) {
						bool unique = true;
						for (int l = 0; l < 9; l++) {
							if (l + i * 9 != j + i * 9 && m_squares[l + i * 9].digit == 0 && m_squares[l + i * 9].possible[k]) {
								unique = false;
								break;
							}
						}

						if (unique) {
							for (int l = 0; l < 9; l++)
								m_squares[j + i * 9].possible[l] = false;
							m_squares[j + i * 9].possibilities = 1;
							m_squares[j + i * 9].possible[k] = true;

							return true;
						}
					}
				}

				// Unique column check
				for (int k = 0; k < 9; k++) {
					if (m_squares[j + i * 9].possible[k]) {
						bool unique = true;
						for (int l = 0; l < 9; l++) {
							if (j + l * 9 != j + i * 9 && m_squares[j + l * 9].digit == 0 && m_squares[j + l * 9].possible[k]) {
								unique = false;
								break;
							}
						}

						if (unique) {
							for (int l = 0; l < 9; l++)
								m_squares[j + i * 9].possible[l] = false;
							m_squares[j + i * 9].possibilities = 1;
							m_squares[j + i * 9].possible[k] = true;

							return true;
						}
					}
				}

				// Unique box check
				int x = floor(j / 3) * 3;
				int y = floor(i / 3) * 3;

				for (int o = 0; o < 9; o++) {
					if (m_squares[j + i * 9].possible[o]) {
						bool unique = true;
						for (int k = y; k < y + 3; k++) {
							for (int l = x; l < x + 3; l++) {
								if (l + k * 9 != j + i * 9 && m_squares[l + k * 9].digit == 0 && m_squares[l + k * 9].possible[o]) {
									unique = false;
									break;
								}
							}
						}

						if (unique) {
							for (int k = 0; k < 9; k++)
								m_squares[j + i * 9].possible[k] = false;
							m_squares[j + i * 9].possibilities = 1;
							m_squares[j + i * 9].possible[o] = true;

							return true;
						}
					}
				}
			}
		}

		return false;
	}

	std::vector<std::string> split(const std::string& str, char c) {
		std::vector<std::string> array;
		std::string element = "";

		for (unsigned i = 0; i < str.length(); i++) {
			if (str[i] != c)
				element += str[i];
			else if (str[i] == c && element != "") {
				array.push_back(element);
				element = "";
			}
		} if (element != "")
			array.push_back(element);

		return array;
	}

	void replace(std::string& str, const std::string& original, const std::string& regex) {
		size_t start = 0;
		while ((start = str.find(original, start)) != std::string::npos) {
			str.replace(start, original.length(), regex);
			start += regex.length();
		}
	}

	std::vector<Square> parseGrid(std::string grid) {
		std::vector<Square> board;

		replace(grid, "  ", " ");
		auto elements = split(grid, ' ');

		for (std::string s : elements)
			board.push_back(std::stoi(s));

		return board;
	}

public:
	virtual bool OnUserCreate() {
		srand(time(NULL));

		m_sAppName = L"Sudoku";

		std::string grid;

		grid =
			"0 0 0  0 0 0  0 0 0 "
			"0 0 0  0 0 0  0 0 0 "
			"0 0 0  0 0 0  0 0 0 "

			"0 0 0  0 0 0  0 0 0 "
			"0 0 0  0 0 0  0 0 0 "
			"0 0 0  0 0 0  0 0 0 "

			"0 0 0  0 0 0  0 0 0 "
			"0 0 0  0 0 0  0 0 0 "
			"0 0 0  0 0 0  0 0 0 ";

		grid =
			"8 0 0  0 0 0  0 0 0 "
			"0 0 3  6 0 0  0 0 0 "
			"0 7 0  0 9 0  2 0 0 "

			"0 5 0  0 0 7  0 0 0 "
			"0 0 0  0 4 5  7 0 0 "
			"0 0 0  1 0 0  0 3 0 "

			"0 0 1  0 0 0  0 6 8 "
			"0 0 8  5 0 0  0 1 0 "
			"0 9 0  0 0 0  4 0 0 ";

		grid =
			"0 6 0  1 0 4  0 5 0 "
			"0 0 8  3 0 5  6 0 0 "
			"2 0 0  0 0 0  0 0 1 "

			"8 0 0  4 0 7  0 0 6 "
			"0 0 6  0 0 0  3 0 0 "
			"7 0 0  9 0 1  0 0 4 "

			"5 0 0  0 0 0  0 0 2 "
			"0 0 7  2 0 6  9 0 0 "
			"0 4 0  5 0 8  0 7 0 ";

		grid =
			"0 0 0  8 0 1  0 0 0 "
			"0 0 0  0 0 0  4 3 0 "
			"5 0 0  0 0 0  0 0 0 "

			"0 0 0  0 7 0  8 0 0 "
			"0 0 0  0 0 0  1 0 0 "
			"0 2 0  0 3 0  0 0 0 "

			"6 0 0  0 0 0  0 7 5 "
			"0 0 3  4 0 0  0 0 0 "
			"0 0 0  2 0 0  6 0 0 ";

		grid =
			"0 9 0  0 1 6  0 5 0 "
			"0 0 5  8 0 0  0 0 0 "
			"6 0 0  0 0 0  0 3 8 "

			"0 0 0  0 0 0  0 0 7 "
			"4 0 0  5 0 8  0 0 3 "
			"9 0 0  0 0 0  0 0 0 "

			"5 2 0  0 0 0  0 0 4 "
			"0 0 0  0 0 9  8 0 0 "
			"0 7 0  1 2 0  0 9 0 ";

		m_originalBoard = parseGrid(grid);

		for (int i = 0; i < 9 * 9; i++)
			if (m_originalBoard[i].digit != 0)
				m_solved++;

		m_squares = m_originalBoard;

		return true;
	}

	virtual bool OnUserUpdate(float eTime) {
		Clear();

		if (!m_backtracking && m_solved < 9 * 9 - 1) {
			if (!solveGrid()) {
				if (!advancedSolve()) {
					m_backtracking = true;
				}
			}
		}
		
		if (m_backtracking)
			backtrack();

		drawGrid(m_backtracking && !m_btSquares.empty() ? m_btSquares : m_squares);

		return true;
	}
};

int main() {
	Frame frame;
	frame.ConstructConsole(145, 145, 4, 4);
	frame.Start();

	return 0;
}