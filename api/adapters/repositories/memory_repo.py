from entities.models import Game

class GameRepository:

    def __init__(self):
        self.games = {}

    def create_game(self, game_id: str, format: str):
        self.games[game_id] = Game(game_id, format)

    def get_game(self, game_id: str) -> Game:
        return self.games.get(game_id)

    def delete_game(self, game_id: str):
        if game_id in self.games:
            del self.games[game_id]
        else:
            raise Exception(f"Game with ID {game_id} not found")