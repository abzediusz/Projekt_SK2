#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <vector>
#include<typeinfo>
#include<cmath>
#include<string>
#include "board.h"
#include "piece.h"
#include "pawn.h"
#include "game.h"


#include "king.h"
using namespace std;

const float X0_B=139;
const float Y0_B=37;
const float X0_W=90;
const float Y0_W=355;
const float DX=93;
const float DY=47;
const float S=0.18;
int main() {
    sf::RenderWindow window(sf::VideoMode(550,432,32),"Checkers");
    /*sf::Texture board,white_pawn,black_pawn;
    sf::Sprite boardS;
    sf::Sprite white_pawnS[12];
    sf::Sprite black_pawnS[12];
    board.loadFromFile("board.jpg");
    white_pawn.loadFromFile("white_pawn.png");
    black_pawn.loadFromFile("black_pawn.png");
    boardS.setTexture(board);
    for(int i=0;i<12;i++) {
        white_pawnS[i].setTexture(white_pawn);
        black_pawnS[i].setTexture(black_pawn);
    }
    white_pawnS[0].setTexture(white_pawn);*/
    Board board;
    Game game;
    vector<Piece*> whitePieces;
    vector<Piece*> blackPieces;
    vector<string> white=board.getwhiteCoordinates();
    vector<string> black=board.getblackCoordinates();
    vector<string> blackTex=board.getblackTextures();
    vector<string> whiteTex=board.getwhiteTextures();
    for(int i=0;i<12;i++) {
        Piece* newPawn=new Pawn(white[i],0);
        whitePieces.push_back(newPawn);
    }
    /*for(int i=0;i<12;i++) {
        Piece* newPiece = new Pawn(black[i],1);
        pieces.push_back(newPiece);
    }*/
    for(int i=0;i<12;i++) {
        Piece* newPiece=new Pawn(black[i],1);
        blackPieces.push_back(newPiece);
    }
    bool chosen=0;

    vector<string> val;
    bool was_beaten=0;
    while(window.isOpen() && !game.getGameOver()) {
        sf::Event event;
        int currentColor=game.getCurrentPlayer();
        while(window.pollEvent(event)) {
            if(event.type==sf::Event::Closed) {
                window.close();
            }
            if(event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left) {
                sf::Vector2i mouse=sf::Mouse::getPosition(window);
                //sf::Vector2f mousePos=window.convertCoords(mouse);
                string pos=board.convertMouse(mouse.x,mouse.y);
                int ind;
                if(!chosen) {
                    if(pos!="XX") {
                        if(currentColor==0) {

                            auto tmp=find(white.begin(),white.end(),pos);
                            if(tmp!=white.end()) {
                                for(int i=0;i<whitePieces.size();i++) {
                                    if((*whitePieces[i]).getPosition()==*tmp) {
                                        ind=i;
                                        break;
                                    }
                                }
                                //int ind=int(tmp-white.begin());
                                //cout<<pos<<" "<<whitePieces[ind].getPosition()<<endl;
                                chosen=1;
                                val.clear();
                                val=(*whitePieces[ind]).getValidMoves(board,was_beaten);
                                for(int i=0;i<val.size();i++) {
                                    cout<<val[i]<<" ";
                                }
                                cout<<endl;
                            }
                        }
                        else {
                            auto tmp=find(black.begin(),black.end(),pos);
                            if(tmp!=black.end()) {
                                for(int i=0;i<blackPieces.size();i++) {
                                    if((*blackPieces[i]).getPosition()==*tmp) {
                                        ind=i;
                                        break;
                                    }
                                }
                                chosen=1;
                                val.clear();
                                val=(*blackPieces[ind]).getValidMoves(board,was_beaten);
                                for(int i=0;i<val.size();i++) {
                                    cout<<val[i]<<" ";
                                }
                                cout<<endl;
                            }
                        }
                    }
                }
                else {
                    if(pos!="XX") {
                        if(currentColor==0) {
                            auto tmp=find(val.begin(),val.end(),pos);
                            if(tmp!=val.end()) {
                                int ind2=int(tmp-val.begin());
                                string oldPosition=(*whitePieces[ind]).getPosition();
                                (*whitePieces[ind]).move(val[ind2]);
                                string newPosition=(*whitePieces[ind]).getPosition();

                                if(abs(int(newPosition[1])-int(oldPosition[1]))>=2) {
                                    string beaten="";
                                    string whichClass=typeid(*whitePieces[ind]).name();
                                    if(whichClass.find("Pawn")!=string::npos) {
                                        if(newPosition[0]>oldPosition[0]) {
                                            beaten.push_back(newPosition[0]-1);
                                            beaten.push_back(newPosition[1]-1);
                                        }
                                        else if(newPosition[0]<oldPosition[0]) {
                                            beaten.push_back(newPosition[0]+1);
                                            beaten.push_back(newPosition[1]-1);
                                        }
                                    }
                                    else {
                                        if(newPosition[0]>oldPosition[0] && newPosition[1]>oldPosition[1]) {
                                            beaten.push_back(newPosition[0]-1);
                                            beaten.push_back(newPosition[1]-1);
                                            if(find(black.begin(),black.end(),beaten)==black.end()) {
                                                beaten="XX";
                                            }
                                        }
                                        else if(newPosition[0]>oldPosition[0] && newPosition[1]<oldPosition[1]) {
                                            beaten.push_back(newPosition[0]-1);
                                            beaten.push_back(newPosition[1]+1);
                                            if(find(black.begin(),black.end(),beaten)==black.end()) {
                                                beaten="XX";
                                            }
                                        }
                                        else if(newPosition[0]<oldPosition[0] && newPosition[1]<oldPosition[1]) {
                                            beaten.push_back(newPosition[0]+1);
                                            beaten.push_back(newPosition[1]+1);
                                            if(find(black.begin(),black.end(),beaten)==black.end()) {
                                                beaten="XX";
                                            }
                                        }
                                        else if(newPosition[0]<oldPosition[0] && newPosition[1]>oldPosition[1]) {
                                            beaten.push_back(newPosition[0]+1);
                                            beaten.push_back(newPosition[1]-1);
                                            if(find(black.begin(),black.end(),beaten)==black.end()) {
                                                beaten="XX";
                                            }
                                        }
                                    }
                                    if(beaten!="XX") {
                                        int ind3;
                                        for(int i=0;i<blackPieces.size();i++) {
                                            if((*blackPieces[i]).getPosition()==beaten) {
                                                ind3=i;
                                                break;
                                            }
                                        }
                                        blackPieces.erase(blackPieces.begin()+ind3);
                                        if(blackPieces.empty()) {
                                            game.setWinner(0);
                                            game.setGameOver(1);
                                        }
                                        string type=typeid(*whitePieces[ind]).name();
                                        if(type.find("Pawn")!=string::npos) {
                                            was_beaten=1;
                                        }

                                        if(type.find("Pawn")!=string::npos && (whitePieces[ind]->getValidMoves(board,was_beaten)).empty()) {
                                            was_beaten=0;
                                        }
                                        else if(type.find("Pawn")!=string::npos) {
                                            val=(*whitePieces[ind]).getValidMoves(board,was_beaten);
                                        }
                                    }
                                }
                                if(whitePieces[ind]->getPosition()[1]=='8') {
                                    whitePieces[ind]=new King(newPosition,0);
                                    was_beaten=0;
                                }
                                if(was_beaten) {
                                    continue;
                                }
                                game.setCurrentPlayer(1);

                            }


                            //game.setCurrentPlayer(1);

                        }




                        else {
                            auto tmp=find(val.begin(),val.end(),pos);
                            if(tmp!=val.end()) {
                                int ind2=int(tmp-val.begin());
                                string oldPosition=(*blackPieces[ind]).getPosition();
                                (*blackPieces[ind]).move(val[ind2]);
                                string newPosition=(*blackPieces[ind]).getPosition();
                                if(abs(int(newPosition[1])-int(oldPosition[1]))>=2) {
                                    string beated="";
                                    string whichClass=typeid(*blackPieces[ind]).name();
                                    if(whichClass.find("Pawn")!=string::npos) {
                                        if(newPosition[0]>oldPosition[0]) {
                                            beated.push_back(newPosition[0]-1);
                                            beated.push_back(newPosition[1]+1);
                                        }
                                        else if(newPosition[0]<oldPosition[0]) {
                                            beated.push_back(newPosition[0]+1);
                                            beated.push_back(newPosition[1]+1);
                                        }
                                    }
                                    else {
                                        if(newPosition[0]>oldPosition[0] && newPosition[1]>oldPosition[1]) {
                                            beated.push_back(newPosition[0]-1);
                                            beated.push_back(newPosition[1]-1);
                                            if(find(white.begin(),white.end(),beated)==white.end()) {
                                                beated="XX";
                                            }
                                        }
                                        else if(newPosition[0]>oldPosition[0] && newPosition[1]<oldPosition[1]) {
                                            beated.push_back(newPosition[0]-1);
                                            beated.push_back(newPosition[1]+1);
                                            if(find(white.begin(),white.end(),beated)==white.end()) {
                                                beated="XX";
                                            }
                                        }
                                        else if(newPosition[0]<oldPosition[0] && newPosition[1]<oldPosition[1]) {
                                            beated.push_back(newPosition[0]+1);
                                            beated.push_back(newPosition[1]+1);
                                            if(find(white.begin(),white.end(),beated)==white.end()) {
                                                beated="XX";
                                            }
                                        }
                                        else if(newPosition[0]<oldPosition[0] && newPosition[1]>oldPosition[1]) {
                                            beated.push_back(newPosition[0]+1);
                                            beated.push_back(newPosition[1]-1);
                                            if(find(white.begin(),white.end(),beated)==white.end()) {
                                                beated="XX";
                                            }
                                        }
                                    }
                                    if(beated!="XX") {
                                        int ind3;
                                        for(int i=0;i<whitePieces.size();i++) {
                                            if((*whitePieces[i]).getPosition()==beated) {
                                                ind3=i;
                                                break;
                                            }
                                        }
                                        whitePieces.erase(whitePieces.begin()+ind3);
                                        if(whitePieces.empty()) {
                                            game.setWinner(1);
                                            game.setGameOver(1);
                                        }
                                        string type=typeid(*blackPieces[ind]).name();
                                        if(type.find("Pawn")!=string::npos) {
                                        was_beaten=1;
                                        }
                                        if(type.find("Pawn")!=string::npos && (blackPieces[ind]->getValidMoves(board,was_beaten)).empty()) {
                                            was_beaten=0;
                                        }
                                        else if(type.find("Pawn")!=string::npos) {
                                            val=blackPieces[ind]->getValidMoves(board,was_beaten);
                                        }
                                    }
                                }
                                    if(blackPieces[ind]->getPosition()[1]=='1') {
                                        blackPieces[ind]=new King(newPosition,1);
                                        was_beaten=0;
                                    }
                                if(was_beaten) {
                                    continue;
                                }
                                game.setCurrentPlayer(0);
                                }

                            }

                        }

                    chosen=0;
                }
                    }

                }




    /*int k=0;
    for(float i=0;i<3;i++) {
        for(float j=0;j<4;j++) {
            //cout<<(i+1)*(j+1)-1<<endl;
            if(i==1) {
                black_pawnS[k].setPosition(X0_B-DX/2+j*DX,Y0_B+i*DY);
                black_pawnS[k].setScale(S,S);
            }
            else {
                black_pawnS[k].setPosition(X0_B+j*DX,Y0_B+i*DY);
                black_pawnS[k].setScale(S,S);
            }
            k++;
        }

    }
    k=0;
    for(float i=0;i<3;i++) {
        for(float j=0;j<4;j++) {
            //cout<<(i+1)*(j+1)-1<<endl;
            if(i==1) {
                white_pawnS[k].setPosition(X0_W+DX/2+j*DX,Y0_W-i*DY);
                white_pawnS[k].setScale(S,S);
            }
            else {
                white_pawnS[k].setPosition(X0_W+j*DX,Y0_W-i*DY);
                white_pawnS[k].setScale(S,S);
            }
            k++;
        }

    }
    window.draw(boardS);
    for(int i=0;i<12;i++) {
        window.draw(black_pawnS[i]);
        window.draw(white_pawnS[i]);
    }*/
    white.clear();
    black.clear();
    blackTex.clear();
    whiteTex.clear();
    for(int i=0;i<whitePieces.size();i++) {
        white.push_back((*whitePieces[i]).getPosition());
        whiteTex.push_back((*whitePieces[i]).getTexture());
    }
    for(int i=0;i<blackPieces.size();i++) {
        black.push_back((*blackPieces[i]).getPosition());
        blackTex.push_back((*blackPieces[i]).getTexture());
    }


    board.setupBoard(white,whiteTex,black,blackTex);
    board.displayBoard(window);

    window.display();



        }
        if(game.getWinner()==0) {
            cout<<"White won"<<endl;
        }
    else {
        cout<<"Black won"<<endl;
    }
        return 0;
    }

