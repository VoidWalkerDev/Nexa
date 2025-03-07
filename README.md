# Axiom - A Simple Chess Engine

<p align="center">
  <img src="https://github.com/user-attachments/assets/148aa313-3302-49f0-bf01-8598ba8de4ef" alt="Axiom Logo" width="200">
</p>

<p align="center">
  <a href="https://www.gnu.org/licenses/agpl-3.0">
    <img src="https://img.shields.io/badge/License-AGPL%20v3-blue.svg" alt="License">
  </a>
</p>

Welcome to Axiom, a basic chess engine crafted purely through AI-assisted development. This project serves as a demonstration of basic chess engine principles and the potential of AI in software creation.

## Purpose

The primary purpose of Axiom is to:

* **Educational Demonstration:** Illustrate the fundamental concepts involved in building a chess engine.
* **AI-Assisted Development:** Explore the capabilities of AI in generating and structuring code.
* **Open-Source Contribution:** Provide a foundation for further development and learning within the open-source community.

This engine is a starting point, intentionally simple to allow for easy understanding and modification. It is not intended for high-level competitive play in its current state.

## Features

* Basic board representation and initialization.
* Pseudo-legal move generation for all standard chess pieces (pawn, knight, bishop, rook, queen, king).
* Implementation of en passant and castling rules.
* Simple FEN position setup.
* Negamax search algorithm for move selection.
* UCI protocol support for interfacing with chess GUIs.
* Basic material-based evaluation function.

## Limitations

* The search algorithm is basic and can be easily outmaneuvered.
* Alpha-beta pruning is not implemented, leading to slow search times.
* Check detection and checkmate/stalemate detection are incomplete.
* The evaluation function is simplistic (material only).
* FEN parsing is limited, ignoring castling rights and en passant fields.
* Pawn promotion auto promotes to queen.

## Installation

1.  Clone the repository:

    ```bash
    git clone [repository URL]
    ```

2.  Compile the source code using a C++ compiler (e.g., g++):

    ```bash
    g++ main.cpp -o axiom
    ```

3.  Run the executable:

    ```bash
    ./axiom
    ```

## Usage

Axiom utilizes the Universal Chess Interface (UCI) protocol. To play against it, use a UCI-compatible chess GUI.

Example UCI commands:

* `uci`: Initialize UCI communication.
* `isready`: Check if the engine is ready.
* `ucinewgame`: Start a new game.
* `position startpos moves e2e4 e7e5`: Set the position and make moves.
* `go`: Start the engine's search.
* `quit`: Exit the engine.

## License

Axiom is licensed under the AGPL-3.0 license. This ensures that any modifications or distributions of this software must also be open source. See the `LICENSE` file for more details.

## Contributions

Contributions are welcome! Feel free to fork the repository, make improvements, and submit pull requests.

## Future Enhancements

* Implement alpha-beta pruning for improved search efficiency.
* Enhance the evaluation function with positional and tactical considerations.
* Add full check detection and checkmate/stalemate detection.
* Improve FEN parsing.
* Add user selectable pawn promotion.
* Implement better search algorithms.

## Acknowledgements

This project was developed by the devs with the assistance of AI, demonstrating its potential in software development.
