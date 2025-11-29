#include<SFML/Graphics.hpp>
class Board {
    public:
    Board();
    int convertX(char x);
    int convertY(char y);
    std::string convertMouse(float x,float y);
    std::vector<std::string> getwhiteCoordinates();
    std::vector<std::string> getblackCoordinates();
    std::vector<std::string> getwhiteTextures();
    std::vector<std::string> getblackTextures();
    void setupBoard(std::vector<std::string>&whitePos,std::vector<std::string>&whiteTex, std::vector<std::string>&blackPos,std::vector<std::string>&blackTex);
    void displayBoard(sf::RenderWindow &window);
    ~Board();
    private:
    std::vector<std::string> whiteCoordinates;
    std::vector<std::string> blackCoordinates;
    std::vector<std::string> whiteTextures;
    std::vector<std::string> blackTextures;
};
