class Game {
    public:
    Game();
    void setCurrentPlayer(int player);
    int getCurrentPlayer();
    void setGameOver(bool gameOver);
    void setWinner(int win);
    int getWinner();
    bool getGameOver();
    //void play();
    ~Game();
    private:
    int currentPlayer;
    bool isGameOver;
    int winner;
};
