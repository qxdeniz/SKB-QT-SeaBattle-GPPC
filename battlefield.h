#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QComboBox>
#include "game_manager.h"

class BattleField : public QWidget {
    Q_OBJECT

public:
    BattleField(QWidget *parent = nullptr);

signals:
    void coordinateSelected(int row, int col);

private slots:
    void onOpponentCellClicked(int row, int col);
    void startGame();

private:
    GameManager *gameManager;
    QGridLayout *playerGrid;
    QGridLayout *opponentGrid;
    QComboBox *gameListCombo;
    QComboBox *gameFormatCombo;
    QPushButton *startGameButton;
    QString gameId;
    QNetworkAccessManager *manager;
    QVector<QPushButton*> playerCells;
    QVector<QPushButton*> opponentCells;
    QString gameType;
    int placeShipsNum;

    void fetchAvailableGames();
    void selectGame();
    void setupFields();
    void sendCoordinates(int row, int col);
    void handleAttackResponse(QJsonDocument *jsonResponse);
    void updateOpponentCellAppearance(int row, int col, const QString &result);
    void updatePlayerField(const QJsonArray &ships);
    void endGame();
    void placeShip(int row, int col);
    void submitShips();
    QVector<QPair<int, int>> playerShips;
    void createNewGame();
    void sendGameStartData();
    void updateGameListUI();
    void updateGameStatusUI(const QString &message);

};

#endif // BATTLEFIELD_H
