from entities import Game


class AttackUseCase:
    def __init__(self, game_repo):
        self.game_repo = game_repo

    def execute(self, game_id: str, player: str,
                row: int, col: int):
        
        game = self.game_repo.get_game(game_id)
        if not game:
            return {"error": "Game not founded"}
        
        opponent = "player1" if player == "player2" else "player2"

        if [row, col] in game.players[opponent]["ships"]:
            game.players[opponent]["hits"].append([row, col])
            result = "hit"
        else:
            game.players[opponent]["misses"].append([row, col])
            result = "miss"

        victory = game.check_victory(opponent)
        return {
            "result": result,
            "victory": victory,
            "row": row,
            "col": col,
            "winner": player if victory else None
        }

