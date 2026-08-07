from fastapi import FastAPI, HTTPException, WebSocket, WebSocketDisconnect
from pydantic import BaseModel
from backend.bridge import Bridge
from fastapi.middleware.cors import CORSMiddleware
from typing import Literal

app = FastAPI()
app.add_middleware(
    CORSMiddleware,
    allow_origins=[
        "http://127.0.0.1:5500",
        "http://localhost:5500",
        "https://sudoku-solver-and-game-generator.onrender.com",
    ],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)
bridge = Bridge()
Difficulty  = Literal["easy", "medium", "hard"] # I defined this for an easier for to filter the difficulties.
KeypadInput = Literal["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "*", "#"]
Joystick = Literal["left", "right", "down", "up", "pressed"]

active_websocket = None

class SudokuRequest(BaseModel):
    board: list[list[int]]

@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    global active_websocket
    await websocket.accept()
    active_websocket = websocket
    print("WebSocket connected!!!!")
    try:
        while True:
            await websocket.receive_text()
    except WebSocketDisconnect:
        if active_websocket is websocket:
            active_websocket = None
        print("WebSocket disconnected!!!!")
@app.get("/")
def home():
    return {"message": "Sudoku API is running"}

def validate_lengths(board: list[list[int]]) -> None:
    if len(board) != 9:
        raise HTTPException(
            status_code=400,
            detail="Board must contain exactly 9 rows"
        )
    for row in board:
        if len(row) != 9:
            raise HTTPException(
                status_code=400,
                detail="Board must contain exactly 9 columns"
            )

def validate_values(board: list[list[int]]) -> None:
    for row in board:
        for number in row:
            if number < 0 or number > 9:
                raise HTTPException(
                    status_code=400,
                    detail="Board values must be between 0 and 9"
                )

@app.post("/solve")
def solve(request: SudokuRequest):
    validate_lengths(request.board)
    validate_values(request.board)
    solved = bridge.solve_sudoku(request.board)
    if solved is None:
        raise HTTPException(
            status_code=422,
            detail="Sudoku has no solution"
        )
    return {"board": solved}

@app.post("/generate/{difficulty}")
def generate(difficulty: Difficulty):
    board = bridge.generate_sudoku(difficulty)
    return {"board": board}


@app.post("/controller/{command}")
async def controller(command: Joystick):
    await send_websocket_message("controller", command)
    return {"command": command}

@app.post("/keypad/{key}")
async def keypad(key: KeypadInput):
    await send_websocket_message("keypad", key)
    return {"key": key}


async def send_websocket_message(message_type: str, value: str):
    global active_websocket
    try:
        if active_websocket is not None:
            await active_websocket.send_json({
                "type": message_type,
                "value": value
            })
    except WebSocketDisconnect:
        active_websocket = None
        print("WebSocket disconnected!!!!")