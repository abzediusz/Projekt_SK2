//#include<SFML/Graphics.hpp>
//#include<SFML/Graphics/Sprite.hpp>
#include <string>
#include<vector>
#include<iostream>
#include "board.h"
#include "piece.h"
#include "pawn.h"
#include "king.h"

using namespace std;
Board::Board() {
whiteCoordinates.clear();
blackCoordinates.clear();
whiteTextures.clear();
blackTextures.clear();
    for(int i=0;i<12;i++) {
        whiteTextures.push_back("white_pawn.png");
        blackTextures.push_back("black_pawn.png");
    }
for(int i=0;i<3;i++)
{
    if(i!=1) {
        for(int j=0;j<8;j+=2)
        {
            string xy="";

            xy.push_back(char(j+65));
            xy.push_back(char(i+1+48));
            //xy=char(j+65)+char(i+1+48);
            whiteCoordinates.push_back(xy);
        }
    } else {
        for(int j=1;j<8;j+=2)
        {
            string xy="";

            xy.push_back(char(j+65));
            xy.push_back(char(i+1+48));
            //xy=char(j+65)+char(i+1+48);
            whiteCoordinates.push_back(xy);
        }
    }
}
    for(int i=8;i>=6;i--)
    {
        if(i==7) {
            for(int j=0;j<8;j+=2)
            {
                string xy="";

                xy.push_back(char(j+65));
                xy.push_back(char(i+48));
                //xy=char(j+65)+char(i+48);
                blackCoordinates.push_back(xy);
            }
        } else {
            for(int j=1;j<8;j+=2)
            {
                string xy="";

                xy.push_back(char(j+65));
                xy.push_back(char(i+48));
                //xy=char(j+65)+char(i+48);
                blackCoordinates.push_back(xy);
            }
        }
    }
}
int Board::convertX(char x) {
    switch(x) {
        case 'A':
            return 95;
        case 'B':
            return 140;
        case 'C':
            return 185;
        case 'D':
            return 230;
        case 'E':
            return 275;
        case 'F':
            return 320;
        case 'G':
            return 365;
        case 'H':
            return 410;
        default:
            return -1;
    }
}
int Board::convertY(char y) {
    switch(y) {
        case '1':
            return 352;
        case '2':
            return 307;
        case '3':
            return 262;
        case '4':
            return 217;
        case '5':
            return 172;
        case '6':
            return 127;
        case '7':
            return 82;
        case '8':
            return 37;
        default:
            return -1;
    }
}
void Board::setupBoard(std::vector<string> &white, std::vector<string> &black)
{
    whiteCoordinates.clear();
    blackCoordinates.clear();
    //whiteTextures.clear();
    //blackTextures.clear();
    for(int i=0;i<white.size();i++) {
        whiteCoordinates.push_back(white[i]);
        //whiteTextures.push_back(whiteTex[i]);
    }
    for(int i=0;i<black.size();i++) {
        blackCoordinates.push_back(black[i]);
        //blackTextures.push_back(blackTex[i]);
    }

}
/*
void Board::displayBoard(sf::RenderWindow &window) {
    sf::Texture board;
    
    vector<sf::Texture> textureWhite;
    vector<sf::Texture> textureBlack;
    for(int i=0;i<whiteCoordinates.size();i++) {
        sf::Texture pom;
        pom.loadFromFile(whiteTextures[i]);
        textureWhite.push_back(pom);
    }
    for(int i=0;i<blackCoordinates.size();i++) {
        sf::Texture pom;
        pom.loadFromFile(blackTextures[i]);
        textureBlack.push_back(pom);
    }
    
    vector<sf::Sprite>white_pawnS;
    vector<sf::Sprite>black_pawnS;
    for(int i=0;i<whiteCoordinates.size();i++) {
        sf::Sprite pion(textureWhite[i]);
        white_pawnS.push_back(pion);
    }
    for(int i=0;i<blackCoordinates.size();i++) {
        sf::Sprite pion(textureBlack[i]);
        black_pawnS.push_back(pion);
    }
    board.loadFromFile("board.jpg");
    sf::Sprite boardS(board);
    
    boardS.setTexture(board);
    window.draw(boardS);
    for(int i=0;i<whiteCoordinates.size();i++) {
        textureWhite[i].loadFromFile(whiteTextures[i]);
        white_pawnS[i].setTexture(textureWhite[i]);
        white_pawnS[i].setPosition(sf::Vector2f(convertX(whiteCoordinates[i][0]),convertY(whiteCoordinates[i][1])));
        white_pawnS[i].setScale(sf::Vector2f(0.18,0.18));

        
    }
    for(int i=0;i<blackCoordinates.size();i++) {
        textureBlack[i].loadFromFile(blackTextures[i]);
        black_pawnS[i].setTexture(textureBlack[i]);
        black_pawnS[i].setPosition(sf::Vector2f(convertX(blackCoordinates[i][0]),convertY(blackCoordinates[i][1])));
        black_pawnS[i].setScale(sf::Vector2f(0.18,0.18));
        //window.draw(black_pawnS[i]);
    }
    for(int i=0;i<whiteCoordinates.size();i++) {
        window.draw(white_pawnS[i]);
    }
    for(int i=0;i<blackCoordinates.size();i++) {
        window.draw(black_pawnS[i]);
    }
    
}*/
vector<string> Board::getwhiteCoordinates() {
    return whiteCoordinates;
}
vector<string> Board::getblackCoordinates() {
    return blackCoordinates;
}
vector<string> Board::getwhiteTextures() {
    return whiteTextures;
}
vector<string> Board::getblackTextures() {
    return blackTextures;
}
string Board::convertMouse(float x, float y) {
    string a,b;
    if(x>=95 && x<=139) {
        a="A";
    }
    else if(x>=140 && x<=184) {
        a="B";
    }
    else if(x>=185 && x<=229) {
        a="C";
    }
    else if(x>=230 && x<=274) {
        a="D";
    }
    else if(x>=275 && x<=319) {
        a="E";
    }
    else if(x>=320 && x<=364) {
        a="F";
    }
    else if(x>=365 && x<=409) {
        a="G";
    }
    else if(x>=410 && x<=454) {
        a="H";
    }
    else {
        return "XX";
    }
    if(y>=352 && y<=392) {
        b="1";
    }
    else if(y>=307 && y<=351) {
        b="2";
    }
    else if(y>=262 && y<=306) {
        b="3";
    }
    else if(y>=217 && y<=261) {
        b="4";
    }
    else if(y>=172 && y<=216) {
        b="5";
    }
    else if(y>=127 && y<=171) {
        b="6";
    }
    else if(y>=82 && y<=126) {
        b="7";
    }
    else if(y>=37 && y<=81) {
        b="8";
    }
    else {
        return "XX";
    }
    return a+b;
}
Board::~Board() {

}



