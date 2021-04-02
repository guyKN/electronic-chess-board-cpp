import chessboard
try:
    chessboard.initGpio()
    prevBoard = chessboard.readBoard()
    chessboard.writeLeds(prevBoard, prevBoard)
    while True:
        board = chessboard.readBoard()
        if board != prevBoard:
            chessboard.writeLeds(board, board)
            prevBoard = board
finally:
    chessboard.cleanupGpio()
