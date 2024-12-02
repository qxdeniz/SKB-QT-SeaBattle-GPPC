#include "game_manager.h"
#include <grpcpp/grpcpp.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <stdexcept>
#include <iostream>
#include <string>
#include <memory>

GameManager::GameManager(const QString &server_address)
    : server_address_(server_address.toStdString()),
    stub_(GameService::NewStub(grpc::CreateChannel(server_address.toStdString(), grpc::InsecureChannelCredentials()))) {


    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(server_address_, grpc::InsecureChannelCredentials());

    stub_ = GameService::NewStub(channel);
}


QString GameManager::createGame(const QString &gameFormat) {

    GameRequest request;
    request.set_format(gameFormat);

    GameResponse response;
    grpc::ClientContext context;

    grpc::Status status = stub_->CreateGame(&context, request, &response);

    if (!status.ok()) {
        throw std::runtime_error("Failed to create game: " + status.error_message());
    }


    return QString::fromStdString(response.game_id());
}

void GameManager::startGame(const QString &gameId, const QString &gameType) {

    GameRequest request;
    request.set_game_id(gameId);
   // request.set_type(gameType);

    GameResponse response;
    grpc::ClientContext context;


    grpc::Status status = stub_->StartGame(&context, request, &response);

    if (!status.ok()) {
        throw std::runtime_error("Failed to start game: " + status.error_message());
    }
}

QJsonDocument GameManager::sendCoordinates(const QJsonObject &coordinates) {

    AttackRequest request;
    request.set_row(coordinates["row"].toInt());
    request.set_col(coordinates["col"].toInt());
    request.set_game_id(coordinates["game_id"].toString().toStdString());
    request.set_player(coordinates["player"].toString().toStdString());

    AttackResponse response;
    grpc::ClientContext context;

    grpc::Status status = stub_->SendCoordinates(&context, request, &response);

    if (!status.ok()) {
        throw std::runtime_error("Failed to send coordinates: " + status.error_message());
    }

    QJsonObject jsonResponse;
    jsonResponse["row"] = response.row();
    jsonResponse["col"] = response.col();
    jsonResponse["result"] = QString::fromStdString(response.result());
    jsonResponse["victory"] = response.victory();

    return QJsonDocument(jsonResponse);
}

void GameManager::submitShips(const QJsonArray &shipsData, const QString &gameId) {
    PlaceShipsRequest request;
    request.set_game_id(gameId.toStdString());

    Ships ships;
    for (const auto &ship : shipsData) {
        QJsonObject shipObject = ship.toObject();
        Ship *newShip = ships.add_ships();
        newShip->set_row(shipObject["row"].toInt());
        newShip->set_col(shipObject["col"].toInt());
    }

    request.mutable_ships()->CopyFrom(ships);

    grpc::ClientContext context;

    grpc::Status status = stub_->SubmitShips(&context, request);

    if (!status.ok()) {
        throw std::runtime_error("Failed to submit ships: " + status.error_message());
    }
}

QJsonArray GameManager::FetchAvailableGames() {
    GameList response;
    grpc::ClientContext context;


    grpc::Status status = stub_->FetchAvailableGames(&context, &response);

    if (!status.ok()) {
        throw std::runtime_error("Failed to fetch available games: " + status.error_message());
    }
    QJsonArray gamesArray;
    for (int i = 0; i < response.games_size(); ++i) {
        gamesArray.append(QString::fromStdString(response.games(i)));
    }

    return gamesArray;
}
