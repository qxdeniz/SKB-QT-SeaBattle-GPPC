#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <grpcpp/grpcpp.h>
#include "game_service.pb.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

class GameManager {
public:
    explicit GameManager(const QString &server_address);

    QString createGame(const QString &gameFormat);
    void startGame(const QString &gameId, const QString &gameType);
    QJsonDocument sendCoordinates(const QJsonObject &coordinates);
    void submitShips(const QJsonArray &shipsData, const QString &gameId);
    QJsonArray FetchAvailableGames();

private:
    std::unique_ptr<GameService::Stub> stub_;
    std::string server_address_;

};

#endif // GAME_MANAGER_H
