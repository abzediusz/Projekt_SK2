#include<string>
#include<iostream>
#include<vector>
#include "board.h"
#include "piece.h"

using namespace std;
Piece::Piece() {

}
Piece::Piece(string pos, int col) {

    if(col!=0 && col!=1) {
        cout<<"Invalid color"<<endl;
    }
    else {
        color=col;
        if(pos.size()!=2 || pos[0]<'A' || pos[0]>'H' || pos[1]<'1' || pos[1]>'8') {
            cout<<"Invalid position"<<endl;
        }
        else
            position=pos;
    }

}
void Piece::move(string newPosition) {
    if(newPosition.size()!=2 || newPosition[0]<'A' || newPosition[0]>'H' || newPosition[1]<'1' || newPosition[1]>'8') {
        cout<<"Invalid position"<<endl;
    }
    else
    position=newPosition;
}
string Piece::getPosition() {
    return position;
}
int Piece::getColor() {
    return color;
}
void Piece::setColor(int col) {
    if(col!=0 && col!=1) {
        cout<<"Invalid color"<<endl;
    }
    else {
        color=col;
    }
}
std::string Piece::getTexture() {
    return texture;
}

vector<string> Piece::getValidMoves(Board board,bool was_beaten) {
;;
}
Piece::~Piece() {

}



