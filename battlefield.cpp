#include "battlefield.h"
#include "game_manager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkReply>
#include <QPushButton>
#include <QComboBox>
#include <QHBoxLayout>

BattleField::BattleField(QWidget *parent, GameManager *gameManager)
    : QWidget(parent), gameManager(gameManager) {

    QVBoxLayout *layout = new QVBoxLayout(this);

    gameFormatCombo = new QComboBox(this);
    gameFormatCombo->addItem("User vs User");
    gameFormatCombo->addItem("User vs Computer");
    layout->addWidget(gameFormatCombo);

    gameListCombo = new QComboBox(this);
    layout->addWidget(gameListCombo);

    QPushButton *createGameButton = new QPushButton("Создать игру", this);
    layout->addWidget(createGameButton);
    connect(createGameButton, &QPushButton::clicked, this, &BattleField::createNewGame);

    QPushButton *selectButton = new QPushButton("Выбрать игру", this);
    layout->addWidget(selectButton);
    connect(selectButton, &QPushButton::clicked, this, [this] {
        if (!gameListCombo->currentText().isEmpty()) {
            gameId = gameListCombo->currentText();
            qDebug() << gameId;
            QMessageBox::information(this, "Game Selected", "ID: " + gameId);
        } else {
            QMessageBox::warning(this, "Warning", "Выберите игру!");
        }
    });

    QPushButton *placeShipsButton = new QPushButton("Расставить корабли", this);
    layout->addWidget(placeShipsButton);
    connect(placeShipsButton, &QPushButton::clicked, this, &BattleField::submitShips);

    startGameButton = new QPushButton("Начать игру", this);
    layout->addWidget(startGameButton);
    connect(startGameButton, &QPushButton::clicked, this, &BattleField::startGame);

    QHBoxLayout *fieldsLayout = new QHBoxLayout();

    QVBoxLayout *playerFieldLayout = new QVBoxLayout();
    QLabel *playerLabel = new QLabel("Ваше поле", this);
    playerFieldLayout->addWidget(playerLabel);

    QGroupBox *playerFieldBox = new QGroupBox(this);
    playerGrid = new QGridLayout();
    playerFieldBox->setLayout(playerGrid);
    playerFieldBox->setStyleSheet("border: 2px solid black;");
    playerFieldLayout->addWidget(playerFieldBox);

    fieldsLayout->addLayout(playerFieldLayout);

    QVBoxLayout *opponentFieldLayout = new QVBoxLayout();
    QLabel *opponentLabel = new QLabel("Поле противника", this);
    opponentFieldLayout->addWidget(opponentLabel);

    QGroupBox *opponentFieldBox = new QGroupBox(this);
    opponentGrid = new QGridLayout();
    opponentFieldBox->setLayout(opponentGrid);
    opponentFieldBox->setStyleSheet("border: 2px solid black;");
    opponentFieldLayout->addWidget(opponentFieldBox);

    fieldsLayout->addLayout(opponentFieldLayout);

    layout->addLayout(fieldsLayout);

    setupFields();
    fetchAvailableGames();
}

void BattleField::updateGameListUI() {
    gameListCombo->clear();
    try {
        QJsonArray games = gameManager->FetchAvailableGames();
        for (const auto &game : games) {
            gameListCombo->addItem(game.toString());
        }
    } catch (const std::runtime_error &e) {
        QMessageBox::warning(this, "Error", "Failed to fetch games: " + QString::fromStdString(e.what()));
    }
}

void BattleField::updateGameStatusUI(const QString &message) {
    QMessageBox::information(this, "Игра создана!", message);
}

void BattleField::createNewGame() {
    QString format = gameFormatCombo->currentText();
    QString gameFormatParam = (format == "User vs User") ? "pvp" : "pvc";

    try {
        gameId = gameManager->createGame(gameFormatParam);
        updateGameListUI();
        updateGameStatusUI("Game Created: " + gameId);
        QMessageBox::information(this, "Game Created", "Game successfully created! Game ID: " + gameId);
    } catch (const std::runtime_error &e) {
        QMessageBox::critical(this, "Error", "Failed to create game: " + QString::fromStdString(e.what()));
    }
}

void BattleField::startGame() {
    if (gameId.isEmpty()) {
        QMessageBox::warning(this, "Error", "Game ID not selected!");
        return;
    }

    QString gameType = (gameFormatCombo->currentText() == "User vs Computer") ? "pvc" : "pvp";

    try {
        gameManager->startGame(gameId, gameType);
        QMessageBox::information(this, "Game Started", "Game successfully started!");
    } catch (const std::runtime_error &e) {
        QMessageBox::critical(this, "Error", "Failed to start game: " + QString::fromStdString(e.what()));
    }

    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 10; ++col) {
            QPushButton *cell = opponentCells[row * 10 + col];
            connect(cell, &QPushButton::clicked, this, [this, row, col]() {
                onOpponentCellClicked(row, col);
            });
        }
    }
}

void BattleField::submitShips() {
    QJsonArray shipsArray;
    for (const auto &ship : playerShips) {
        QJsonArray shipCoords;
        shipCoords.append(ship.first);
        shipCoords.append(ship.second);
        shipsArray.append(shipCoords);
    }

    try {
        gameManager->submitShips(shipsArray, gameId);
        updateGameStatusUI("Ships Submitted");
        QMessageBox::information(this, "Ships Submitted", "Ships successfully placed!");
    } catch (const std::runtime_error &e) {
        QMessageBox::critical(this, "Error", "Failed to submit ships: " + QString::fromStdString(e.what()));
    }
}

void BattleField::onOpponentCellClicked(int row, int col) {
    qDebug() << "Удар по X: " + QString::number(row) + " Y: " + QString::number(col);
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    button->setStyleSheet("background-color: blue;");
    sendCoordinates(row, col);
}

void BattleField::sendCoordinates(int row, int col) {
    QJsonObject attackData;
    attackData["row"] = row;
    attackData["col"] = col;
    attackData["game_id"] = gameId;
    attackData["player"] = "player1";

    try {
        QJsonDocument response = gameManager->sendCoordinates(attackData);
        handleAttackResponse(response);
    } catch (const std::runtime_error &e) {
        QMessageBox::critical(this, "Error", "Failed to send attack coordinates: " + QString::fromStdString(e.what()));
    }
}

void BattleField::setupFields() {
    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 10; ++col) {
            QPushButton *playerCell = new QPushButton(this);
            playerCell->setFixedSize(40, 40);
            playerCell->setStyleSheet("border: 1px solid black;");
            playerGrid->addWidget(playerCell, row, col);
            playerCells.append(playerCell);

            connect(playerCell, &QPushButton::clicked, this, [this, row, col]() {
                placeShip(row, col);
            });
        }
    }

    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 10; ++col) {
            QPushButton *opponentCell = new QPushButton(this);
            opponentCell->setFixedSize(40, 40);
            opponentCell->setStyleSheet("border: 1px solid black;");
            opponentGrid->addWidget(opponentCell, row, col);
            opponentCells.append(opponentCell);

            connect(opponentCell, &QPushButton::clicked, this, [this, row, col]() {
                sendCoordinates(row, col);
            });
        }
    }
}

void BattleField::handleAttackResponse(QJsonDocument *jsonResponse) {
    QJsonObject responseObject = jsonResponse->object();
    int row = responseObject["row"].toInt();
    int col = responseObject["col"].toInt();
    QString result = responseObject["result"].toString();

    updateOpponentCellAppearance(row, col, result);

    if (responseObject["victory"].toBool()) {
        QMessageBox::information(this, "Victory", "You win!");
    }
}

void BattleField::updateOpponentCellAppearance(int row, int col, const QString &result) {
    QPushButton *cell = opponentCells[row * 10 + col];
    if (result == "hit") {
        cell->setStyleSheet("background-color: red;");
    } else {
        cell->setStyleSheet("background-color: gray;");
    }
}

void BattleField::fetchAvailableGames() {
    updateGameListUI();
}
