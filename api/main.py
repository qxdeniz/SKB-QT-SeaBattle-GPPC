import grpc
from concurrent import futures
import proto.game_service_pb2 as pb2
import proto.game_service_pb2_grpc as pb2_grpc
from adapters.repositories.memory_repo import GameRepository
from use_cases.gameActions import gameActions


class GameServiceServicer(pb2_grpc.GameServiceServicer):
    def __init__(self):
        self.game_repo = GameRepository()
        self.game_actions = gameActions(self.game_repo)

    def CreateGame(self, request, context):
        try:
            game_id = self.game_actions.create(request.format)
            return pb2.GameResponse(game_id=game_id)
        except Exception as e:
            context.set_code(grpc.StatusCode.INTERNAL)
            context.set_details(f"Error creating game: {e}")
            return pb2.GameResponse()  

    def AvailableGames(self, request, context):
        """Получить список доступных игр."""
        try:
            games = self.game_repo.get_available_games() 
            game_list = [pb2.GameInfo(id=game["id"], name=game["name"]) for game in games]
            return pb2.GameList(games=game_list)
        except Exception as e:
            context.set_code(grpc.StatusCode.INTERNAL)
            context.set_details(f"Error fetching available games: {e}")
            return pb2.GameList()  

    def StartGame(self, request, context):
        """Начать игру."""
        try:
            game_id = request.game_id
            success = self.game_actions.start(game_id)  
            if success:
                return pb2.GameMessage(message="Game started successfully.")
            else:
                context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
                context.set_details("Failed to start the game.")
                return pb2.GameMessage(message="Game start failed.")
        except Exception as e:
            context.set_code(grpc.StatusCode.INTERNAL)
            context.set_details(f"Error starting game: {e}")
            return pb2.GameMessage()

    def GetPlayerField(self, request, context):
        try:
            player_field = self.game_repo.get_game(request.player_id)  
            return pb2.PlayerFieldResponse(field=player_field)
        except Exception as e:
            context.set_code(grpc.StatusCode.INTERNAL)
            context.set_details(f"Error fetching player field: {e}")
            return pb2.PlayerFieldResponse()

    def PlaceShips(self, request, context):
        try:
            success = self.game_actions.place_ships(
                request.game_id, request.player_id, request.ship_positions
            )
            if success:
                return pb2.GameMessage(message="Ships placed successfully.")
            else:
                context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
                context.set_details("Failed to place ships.")
                return pb2.GameMessage(message="Failed to place ships.")
        except Exception as e:
            context.set_code(grpc.StatusCode.INTERNAL)
            context.set_details(f"Error placing ships: {e}")
            return pb2.GameMessage()

    def Attack(self, request, context):
        """Совершить атаку."""
        try:
            result = self.game_actions.attack(
                request.game_id, request.attacker_id, request.target_position
            )
            return pb2.AttackResponse(
                success=result["success"],
                hit=result["hit"],
                sunk=result.get("sunk", False)
            )
        except Exception as e:
            context.set_code(grpc.StatusCode.INTERNAL)
            context.set_details(f"Error performing attack: {e}")
            return pb2.AttackResponse()


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    pb2_grpc.add_GameServiceServicer_to_server(GameServiceServicer(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    print("Сервер запущен на порту 50051...")
    server.wait_for_termination()


if __name__ == "__main__":
    serve()