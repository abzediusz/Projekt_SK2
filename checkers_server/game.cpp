#include "game.h"
Game::Game() {
isGameOver = false;
    currentPlayer = 0;
}
void Game::setCurrentPlayer(int player) {
    currentPlayer = player;
}
int Game::getCurrentPlayer() {
    return currentPlayer;
}
void Game::setGameOver(bool gameOver) {
    isGameOver = gameOver;
}
bool Game::getGameOver() {
    return isGameOver;
}
void Game::setWinner(int win) {
    winner = win;
}
int Game::getWinner() {
    return winner;
}
Game::~Game() {

}



