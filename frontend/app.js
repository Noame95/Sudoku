console.log("app.js loaded");
const API_URL = "https://sudoku-solver-game-generator.onrender.com";
const WEBSOCKET_URL = "wss://sudoku-solver-game-generator.onrender.com/ws";

const socket = new WebSocket(WEBSOCKET_URL);
socket.onopen = function () {console.log("WebSocket connected");};
socket.onmessage = function (event) {
    const data = JSON.parse(event.data);
    console.log("WebSocket message:", data);
    if (data.type === CONTROLLER) {handleControllerCommand(data.value);}
    if (data.type === KEYPAD) {handleKeypadKey(data.value);}
};
socket.onclose = function () {console.log("WebSocket disconnected");};
socket.onerror = function (error) {console.log("WebSocket error:", error);};

const board = document.getElementById("sudoku-board");
const clearButton = document.getElementById("clear-button");
const solveBoard = document.getElementById("solve-button");

const easyButton = document.getElementById("easy-button");
const mediumButton = document.getElementById("medium-button");
const hardButton = document.getElementById("hard-button");

const BOARD_SIZE = 9;
let gameSolution = null;
const SOLVED_CELL_CLASS = "solved-cell";
const FIXED_CELL_CLASS = "fixed-cell";
const CORRECT_CELL_CLASS = "correct-cell";
const WRONG_CELL_CLASS = "wrong-cell";
const SELECTED_CELL_CLASS = "controller-selected";
const CORRECT_EFFECT_DELAY = 300;
const DELAY_CELL_EFFECT = 10;
const click = "click"


const POST_METHOD = "POST";
const CONTENT_TYPE = "Content-Type";
const JSON_TYPE = "application/json";
const SOLVE_API = "solve"
const GENERATE_API = "generate";

const EMPTY = "";
const REMOVE = "*";
const CLEAR = "#";
const A_SOLVE = "A";
const B_EASY = "B";
const C_MEDIUM = "C";
const D_HARD = "D";
const SOLVER_MODE = "solver";
const GAME_MODE = "game";

const CONTROLLER = "controller";
const LEFT = "left";
const RIGHT = "right";
const DOWN = "down";
const UP = "up";

const KEYPAD = "keypad";

let currentMode = SOLVER_MODE;
const EASY = "easy";
const MEDIUM = "medium";
const HARD = "hard";
let diff = EMPTY;

let selectedRow = 0;
let selectedColumn = 0;


function handleControllerCommand(command) {
    if (command === EMPTY) {return;}
    if (command === RIGHT && selectedColumn < 8) {selectedColumn++;}
    if (command === LEFT && selectedColumn > 0) {selectedColumn--;}
    if (command === UP && selectedRow > 0) {selectedRow--;}
    if (command === DOWN && selectedRow < 8) {selectedRow++;}
    console.log(selectedRow, selectedColumn);
    updateControllerSelection();
}

async function handleKeypadKey(key) {
    if (key === EMPTY || key === undefined) return;
    if (key === CLEAR) {clearPuzzle(); return;}
    if (key === A_SOLVE) {await handleSolution(); return;}
    if (key === B_EASY) {await gameBoard(EASY); return;}
    if (key === C_MEDIUM) {await gameBoard(MEDIUM); return;}
    if (key === D_HARD) {await gameBoard(HARD); return;}
    const selectedCell = document.querySelector(`[data-row="${selectedRow}"][data-column="${selectedColumn}"]`);
    if (selectedCell === null || selectedCell.disabled) return;
    if (key === REMOVE) {
        selectedCell.value = EMPTY;
        selectedCell.dispatchEvent(new Event("input"));
        return;
    } selectedCell.value = key;
    selectedCell.dispatchEvent(new Event("input"));
}

function updateControllerSelection() {
    const oldSelectedCell = document.querySelector(`.${SELECTED_CELL_CLASS}`);
    if (oldSelectedCell !== null) {
        oldSelectedCell.classList.remove(SELECTED_CELL_CLASS);
        oldSelectedCell.style.backgroundColor = "";
        oldSelectedCell.style.boxShadow = "";
    } const selectedCell = document.querySelector(`[data-row="${selectedRow}"][data-column="${selectedColumn}"]`);
    if (selectedCell !== null) {
        selectedCell.classList.add(SELECTED_CELL_CLASS);
        selectedCell.style.backgroundColor = "yellow";
        selectedCell.style.boxShadow = "inset 0 0 0 4px red";
    } console.log("Selected cell:", selectedCell);
    console.log("Classes:", selectedCell?.className);
}


function createBoard() {
    for (let row = 0; row < BOARD_SIZE; row++) {
        const newRow = document.createElement("div");
        for (let column = 0; column < BOARD_SIZE; column++) {
            const cell = document.createElement("input");
            cell.dataset.row = row;
            cell.dataset.column = column;
            cell.type = "number";
            cell.min = "1";
            cell.max = BOARD_SIZE.toString();
            cell.addEventListener("input", async function () {
            if (cell.value > BOARD_SIZE || cell.value < 1 || cell.value % 1 !== 0) {cell.value = EMPTY;}
            await checkGameCell(cell);
        });
            newRow.appendChild(cell);
        } board.appendChild(newRow);
    } console.log(board);
}

createBoard();
updateControllerSelection();

function getPuzzleFromBoard() {
    const puzzle = [];
    for (let row = 0; row < BOARD_SIZE; row++) {
        const currentRow = [];
        for (let column = 0; column < BOARD_SIZE; column++) {
            const cell = board.children[row].children[column];
            if (cell.value === "") {
                currentRow.push(0);
            } else {
                currentRow.push(Number(cell.value));
            }
        }
        puzzle.push(currentRow);
    } return puzzle;
}

function clearPuzzle() {
    console.log("Clear clicked");
    for (let row = 0; row < BOARD_SIZE; row++) {
        for (let column = 0; column < BOARD_SIZE; column++) {
            const cell = board.children[row].children[column];
            cell.value = EMPTY;
            cell.disabled = false;
            cell.classList.remove(
                WRONG_CELL_CLASS,
                CORRECT_CELL_CLASS,
                FIXED_CELL_CLASS
            );
        }
    } currentMode = SOLVER_MODE;
    diff = EMPTY;
    gameSolution = null;
}

function isGameComplete() {
    for (let row = 0; row < BOARD_SIZE; row++) {
        for (let column = 0; column < BOARD_SIZE; column++) {
            const cell = board.children[row].children[column];
            if (cell.value === EMPTY) {return false;}
        }
    } return true;
}

function sleep(milliseconds) {
    return new Promise(resolve => setTimeout(resolve, milliseconds));
}

async function fillBoard(solvedBoard) {
    for (let row = 0; row < BOARD_SIZE; row++) {
        for (let column = 0; column < BOARD_SIZE; column++) {
            const cell = board.children[row].children[column];
            const wasEmpty = cell.value === EMPTY;
            cell.value = solvedBoard[row][column];
            if (wasEmpty) {
                cell.classList.remove(SOLVED_CELL_CLASS); // even if it doesn't exist, it wouldn't care and pass.
                void cell.offsetWidth;
                cell.classList.add(SOLVED_CELL_CLASS);
                await sleep(DELAY_CELL_EFFECT);
            }
        }
    }
}

function sudokuRequest(action, value) {
    if (action === SOLVE_API) {
        return fetch(`${API_URL}/${SOLVE_API}`, {
            method: POST_METHOD,
            headers: {[CONTENT_TYPE]: JSON_TYPE},
            body: JSON.stringify({board: value})
        });
    } if (action === GENERATE_API) {
        return fetch(`${API_URL}/${GENERATE_API}/${value}`, {
            method: POST_METHOD
        });
    }
}

async function handleSolution() {
    try {
        solveBoard.disabled = true;
        console.log("Solve clicked");
        const puzzle = getPuzzleFromBoard();
        console.log("Puzzle being sent:", puzzle);
        const response = await sudokuRequest(SOLVE_API, puzzle);
        console.log(response.status);
        const data = await response.json();
        console.log(data);
        await fillBoard(data.board);

    } catch (error) {
        console.error("Failed to solve puzzle:", error);
    } finally {
        solveBoard.disabled = false;
    }
}

async function checkGameCell(cell) {
    if (currentMode !== GAME_MODE || gameSolution === null) {return;}
    const row = Number(cell.dataset.row);
    const column = Number(cell.dataset.column);
    cell.classList.remove(CORRECT_CELL_CLASS, WRONG_CELL_CLASS);
    if (cell.value === EMPTY) {return;}
    if (Number(cell.value) !== gameSolution[row][column]) {
        cell.classList.add(WRONG_CELL_CLASS);
        playErrorSound();
        return;
    }
    cell.classList.add(CORRECT_CELL_CLASS);
    playSuccessSound();
    await sleep(CORRECT_EFFECT_DELAY);
    cell.classList.remove(CORRECT_CELL_CLASS);
    if (isGameComplete()) {
    playVictorySound();
    console.log("Victory!");
}
}

async function gameBoard(difficulty) {
    currentMode = GAME_MODE;
    diff = difficulty;
    console.log("Current difficulty:", difficulty);
    const response = await sudokuRequest(GENERATE_API, difficulty);
    if (!response.ok) {throw new Error("Failed to generate puzzle");}
    const data = await response.json();
    const puzzle = data.board;
    const solutionResponse = await sudokuRequest(SOLVE_API, puzzle);
    if (!solutionResponse.ok) {throw new Error("Failed to solve generated puzzle");}
    const solutionData = await solutionResponse.json();
    gameSolution = solutionData.board;
    for (let row = 0; row < BOARD_SIZE; row++) {
        for (let column = 0; column < BOARD_SIZE; column++) {
            const cell = board.children[row].children[column];
            const value = puzzle[row][column];
            if (value === 0) {
                cell.value = EMPTY;
                cell.disabled = false;
                cell.classList.remove(FIXED_CELL_CLASS);
            } else {
                cell.value = value;
                cell.disabled = true;
                cell.classList.add(FIXED_CELL_CLASS);
            }
        }
    }
}


clearButton.addEventListener(click, () => {clearPuzzle();});
solveBoard.addEventListener(click, handleSolution);
easyButton.addEventListener(click, () => gameBoard(EASY));
mediumButton.addEventListener(click, () => gameBoard(MEDIUM));
hardButton.addEventListener(click, () => gameBoard(HARD));
