#include<vector>
#include<string>
#include<algorithm>
#include "board.h"
#include "piece.h"
#include "king.h"
using namespace std;
King::King(std::string pos,int col) {
    position = pos;
    color=col;
    if(color==0) {
        texture="white_king.png";
    }
    else {
        texture="black_king.png";
    }
}
vector<string> King::getValidMoves(Board board,bool was_beaten) {
    vector<string> validMoves;
    if(color==0) {
        vector<string> black=board.getblackCoordinates();
        vector<string> white=board.getwhiteCoordinates();
        string cur="";
        if(position[1]!='8' && position[0]!='H') {
            cur="";
            cur.push_back(char(position[0]+1));
            cur.push_back(char(position[1]+1));

            while(cur[1]<='8' && cur[0]<='H' && find(black.begin(),black.end(),cur)==black.end() && find(white.begin(),white.end(),cur)==white.end()) {
                validMoves.push_back(cur);
                cur[0]=char(cur[0]+1);
                cur[1]=char(cur[1]+1);
            }
            if(cur[1]<'8' && cur[0]<'H' && find(white.begin(),white.end(),cur)==white.end()) {
                cur[0]=char(cur[0]+1);
                cur[1]=char(cur[1]+1);
                if(find(black.begin(),black.end(),cur)==black.end() && find(white.begin(),white.end(),cur)==white.end()) {
                    validMoves.push_back(cur);
                }
            }
        }
        if(position[1]!='1' && position[0]!='H') {
            cur="";
            cur.push_back(char(position[0]+1));
            cur.push_back(char(position[1]-1));

            while(cur[1]>='1' && cur[0]<='H' && find(black.begin(),black.end(),cur)==black.end() && find(white.begin(),white.end(),cur)==white.end()) {
                validMoves.push_back(cur);
                cur[0]=char(cur[0]+1);
                cur[1]=char(cur[1]-1);
            }
            if(cur[1]>'1' && cur[0]>'H' && find(white.begin(),white.end(),cur)==white.end()) {
                cur[0]=char(cur[0]+1);
                cur[1]=char(cur[1]-1);
                if(find(black.begin(),black.end(),cur)==black.end() && find(white.begin(),white.end(),cur)==white.end()) {
                    validMoves.push_back(cur);
                }
            }
        }
        if(position[1]!='1' && position[0]!='A') {
            cur="";
            cur.push_back(char(position[0]-1));
            cur.push_back(char(position[1]-1));

            while(cur[1]>='1' && cur[0]>='A' && find(black.begin(),black.end(),cur)==black.end() && find(white.begin(),white.end(),cur)==white.end()) {
                validMoves.push_back(cur);
                cur[0]=char(cur[0]-1);
                cur[1]=char(cur[1]-1);
            }
            if(cur[1]>'1' && cur[0]>'A' && find(white.begin(),white.end(),cur)==white.end()) { //poczatkowo != czemu? nie wiem
                cur[0]=char(cur[0]-1);
                cur[1]=char(cur[1]-1);
                if(find(black.begin(),black.end(),cur)==black.end() && find(white.begin(),white.end(),cur)==white.end()) {
                    validMoves.push_back(cur);
                }
            }
        }
        if(position[1]!='8' && position[0]!='A') {
            cur="";
            cur.push_back(char(position[0]-1));
            cur.push_back(char(position[1]+1));

            while(cur[1]<='8' && cur[0]>='A' && find(black.begin(),black.end(),cur)==black.end() && find(white.begin(),white.end(),cur)==white.end()) {
                validMoves.push_back(cur);
                cur[0]=char(cur[0]-1);
                cur[1]=char(cur[1]+1);
            }
            if(cur[1]<'8' && cur[0]>'A' && find(white.begin(),white.end(),cur)==white.end()) {
                cur[0]=char(cur[0]-1);
                cur[1]=char(cur[1]+1);
                if(find(black.begin(),black.end(),cur)==black.end() && find(white.begin(),white.end(),cur)==white.end()) {
                    validMoves.push_back(cur);
                }
            }
        }

    }
    else {
        vector<string> black=board.getblackCoordinates();
        vector<string> white=board.getwhiteCoordinates();
        string cur="";
        if(position[1]!='8' && position[0]!='H') {
            cur.push_back(char(position[0]+1));
            cur.push_back(char(position[1]+1));

            while(cur[1]<='8' && cur[0]<='H' && find(black.begin(),black.end(),cur)==black.end() && find(white.begin(),white.end(),cur)==white.end()) {
                validMoves.push_back(cur);
                cur[0]=char(cur[0]+1);
                cur[1]=char(cur[1]+1);
            }
            if(cur[1]<'8' && cur[0]<'H' && find(black.begin(),black.end(),cur)==black.end()) {
                cur[0]=char(cur[0]+1);
                cur[1]=char(cur[1]+1);
                if(find(white.begin(),white.end(),cur)==white.end() && find(black.begin(),black.end(),cur)==black.end()) {
                    validMoves.push_back(cur);
                }
            }
        }
        if(position[1]!='1' && position[0]!='H') {
            cur="";
            cur.push_back(char(position[0]+1));
            cur.push_back(char(position[1]-1));

            while(cur[1]>='1' && cur[0]<='H' && find(black.begin(),black.end(),cur)==black.end() && find(white.begin(),white.end(),cur)==white.end()) {
                validMoves.push_back(cur);
                cur[0]=char(cur[0]+1);
                cur[1]=char(cur[1]-1);
            }
            if(cur[1]>'1' && cur[0]>'H' && find(black.begin(),black.end(),cur)==black.end()) {
                cur[0]=char(cur[0]+1);
                cur[1]=char(cur[1]-1);
                if(find(white.begin(),white.end(),cur)==white.end() && find(black.begin(),black.end(),cur)==black.end()) {
                    validMoves.push_back(cur);
                }
            }
        }
        if(position[1]!='1' && position[0]!='A') {
            cur="";
            cur.push_back(char(position[0]-1));
            cur.push_back(char(position[1]-1));

            while(cur[1]>='1' && cur[0]>='A' && find(black.begin(),black.end(),cur)==black.end() && find(white.begin(),white.end(),cur)==white.end()) {
                validMoves.push_back(cur);
                cur[0]=char(cur[0]-1);
                cur[1]=char(cur[1]-1);
            }
            if(cur[1]>'1' && cur[0]>'A' && find(black.begin(),black.end(),cur)==black.end()) {
                cur[0]=char(cur[0]-1);
                cur[1]=char(cur[1]-1);
                if(find(black.begin(),black.end(),cur)==black.end() && find(white.begin(),white.end(),cur)==white.end()) {
                    validMoves.push_back(cur);
                }
            }
        }
        if(position[1]!='8' && position[0]!='A') {
            cur="";
            cur.push_back(char(position[0]-1));
            cur.push_back(char(position[1]+1));

            while(cur[1]<='8' && cur[0]>='A' && find(black.begin(),black.end(),cur)==black.end() && find(white.begin(),white.end(),cur)==white.end()) {
                validMoves.push_back(cur);
                cur[0]=char(cur[0]-1);
                cur[1]=char(cur[1]+1);
            }
            if(cur[1]<'8' && cur[0]>'A' && find(black.begin(),black.end(),cur)==black.end()) {
                cur[0]=char(cur[0]-1);
                cur[1]=char(cur[1]+1);
                if(find(black.begin(),black.end(),cur)==black.end() && find(white.begin(),white.end(),cur)==white.end()) {
                    validMoves.push_back(cur);
                }
            }
        }
    }
    return validMoves;
}
King::~King() {

}

