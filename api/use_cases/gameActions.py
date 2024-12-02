import uuid
from adapters.repositories.memory_repo import GameRepository

class gameActions:
    def __init__(self, game_repo: GameRepository):
        self.game_repo = game_repo

    def create(self, format: str) -> str:
        game_id = str(uuid.uuid4())
        self.game_repo.create_game(game_id, format)
        return game_id
    
    def delete(self, game_id: str):
        try:
            self.game_repo.delete_game(game_id)
            return True
        except Exception as e:
            return False