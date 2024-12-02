from typing import List, Dict

class Game:
    def __init__(self, game_id: str, format: str):
        self.game_id = game_id
        self.players = {
            "player1": {"ships": [], "hits": [], "misses": []},
            "player2": {"ships": [], "hits": [], "misses": []} if format == "pvp" else {"ai": True, "ships": [], "hits": [], "misses": []}

        }
    
    def place_ships(self, player: str, ships: List[List[int]]):
        self.players[player]["ships"] = ships

    def check_victory(self, player: str) -> bool:
        player_ships = self.players[player]["ships"]
        hits = self.players[player]["hits"]
        return all(ship in hits for ship in player_ships)