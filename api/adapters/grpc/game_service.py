import proto.game_service_pb2
import proto.game_service_pb2_grpc
from use_cases.gameActions import gameActions

class GameServiceServicer(proto.game_service_pb2_grpc.GameServiceServicer):
    def __init__(self, gameManager):
        self.gameManager = gameManager

    async def CreateGame(self, request, context):
        game_id = self.gameManager.create(request.format)
        return proto.game_service_pb2.GameResponse(game_id=game_id)