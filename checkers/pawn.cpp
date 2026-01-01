#include<SFML/Graphics.hpp>
#include<vector>
#include<string>
#include<algorithm>
#include<iostream>
#include "board.h"
#include "piece.h"
#include "pawn.h"

using namespace std;
Pawn::Pawn(string pos, int col) {
    position=pos;
    color=col;
    if(color==0) {
        texture="white_pawn.png";
    }
    else {
        texture="black_pawn.png";
    }
}

vector<string> Pawn::getValidMoves(Board board,bool was_beaten) {
    vector<string> validMoves;
    validMoves.clear();
    if(color==0) {
        vector<string> black=board.getblackCoordinates();
        vector<string> white=board.getwhiteCoordinates();
        if(position[0]=='A') {
            string pom="";
            pom.push_back(char(position[1]+1));
            pom="B"+pom;
            
            if(!was_beaten && find(black.begin(),black.end(),pom)==black.end() && find(white.begin(),white.end(),pom)==white.end()) {
                validMoves.push_back(pom);
            }
            else if(pom[1]!='8' && find(white.begin(),white.end(),pom)==white.end() && find(black.begin(),black.end(),pom)!=black.end()){
                string pom2="";

                pom2.push_back('C');
                pom2.push_back(char(pom[1]+1));
                if(find(black.begin(),black.end(),pom2)==black.end() && find(white.begin(),white.end(),pom2)==white.end()) {
                    validMoves.push_back(pom2);
                }
            }
            if(position[1]!='2' && position[1]!='1')
            {
                string pom3="";
            pom3.push_back(char(position[1]-1));
            pom3="B"+pom3;
            if(pom3[1]!='1' && find(white.begin(),white.end(),pom3)==white.end() && find(black.begin(),black.end(),pom3)!=black.end()){
                string pom2="";

                pom2.push_back('C');
                pom2.push_back(char(pom3[1]-1));
                if(find(black.begin(),black.end(),pom2)==black.end() && find(white.begin(),white.end(),pom2)==white.end()) {
                    validMoves.push_back(pom2);
                }
            }
        }
        }
        else if(position[0]>='B' && position[0]<='G') {
            string pom1="";
            pom1.push_back(char(position[0]+1));
            pom1.push_back(char(position[1]+1));

            if(!was_beaten && find(black.begin(),black.end(),pom1)==black.end() && find(white.begin(),white.end(),pom1)==white.end()) {
                validMoves.push_back(pom1);
            }

            else if(pom1[1]!='8' && pom1[0]!='H' && find(white.begin(),white.end(),pom1)==white.end() && find(black.begin(),black.end(),pom1)!=black.end()){
                string pom2="";
                pom2.push_back(char(pom1[0]+1));
                pom2.push_back(char(pom1[1]+1));

                if(find(black.begin(),black.end(),pom2)==black.end() && find(white.begin(),white.end(),pom2)==white.end()) {
                    validMoves.push_back(pom2);
                }
            }
            if(position[1]!='1' && position[1]!='2')
            {
                string pom4="";
                pom4.push_back(char(position[0]+1));
                pom4.push_back(char(position[1]-1));
                if(pom4[1]!='1' && pom4[0]!='H' && find(white.begin(),white.end(),pom4)==white.end() && find(black.begin(),black.end(),pom4)!=black.end()){
                string pom2="";

                pom2.push_back(char(pom4[0]+1));
                pom2.push_back(char(pom4[1]-1));
                if(find(black.begin(),black.end(),pom2)==black.end() && find(white.begin(),white.end(),pom2)==white.end()) {
                    validMoves.push_back(pom2);
                }
            }
            }
            string pom3="";
            pom3.push_back(char(position[0]-1));
            pom3.push_back(char(position[1]+1));

            if(!was_beaten && find(black.begin(),black.end(),pom3)==black.end() && find(white.begin(),white.end(),pom3)==white.end()) {
                validMoves.push_back(pom3);
            }
            else if(pom3[1]!='8' && pom3[0]!='A' && find(white.begin(),white.end(),pom3)==white.end() && find(black.begin(),black.end(),pom3)!=black.end()){
                string pom4="";
                pom4.push_back(char(pom3[0]-1));
                pom4.push_back(char(pom3[1]+1));

                if(find(black.begin(),black.end(),pom4)==black.end() && find(white.begin(),white.end(),pom4)==white.end()) {
                    validMoves.push_back(pom4);
                }
            }
            if(position[1]!='1' && position[1]!='2')
            {
                string pom4="";
                pom4.push_back(char(position[0]-1));
                pom4.push_back(char(position[1]-1));
                if(pom4[1]!='1' && pom4[0]!='A' && find(white.begin(),white.end(),pom4)==white.end() && find(black.begin(),black.end(),pom4)!=black.end()){
                string pom2="";

                pom2.push_back(char(pom4[0]-1));
                pom2.push_back(char(pom4[1]-1));
                if(find(black.begin(),black.end(),pom2)==black.end() && find(white.begin(),white.end(),pom2)==white.end()) {
                    validMoves.push_back(pom2);
                }
            }
            }
        }
        else if(position[0]=='H') {

            string pom1="";
            pom1.push_back(char(position[0]-1));
            pom1.push_back(char(position[1]+1));
            cout<<pom1<<endl;
            if(!was_beaten && find(black.begin(),black.end(),pom1)==black.end() && find(white.begin(),white.end(),pom1)==white.end()) {
                validMoves.push_back(pom1);
            }
            else if(pom1[1]!='8' && find(white.begin(),white.end(),pom1)==white.end() && find(black.begin(),black.end(),pom1)!=black.end()){
                string pom2="";
                pom2.push_back(char(pom1[0]-1));
                pom2.push_back(char(pom1[1]+1));

                if(find(black.begin(),black.end(),pom2)==black.end() && find(white.begin(),white.end(),pom2)==white.end()) {
                    validMoves.push_back(pom2);
                }
            }
            if(position[1]!='2' && position[1]!='1')
            {
                string pom3="";
            pom3.push_back(char(position[1]-1));
            pom3="G"+pom3;
            if(pom3[1]!='1' && find(white.begin(),white.end(),pom3)==white.end() && find(black.begin(),black.end(),pom3)!=black.end()){
                string pom2="";

                pom2.push_back('F');
                pom2.push_back(char(pom3[1]-1));
                if(find(black.begin(),black.end(),pom2)==black.end() && find(white.begin(),white.end(),pom2)==white.end()) {
                    validMoves.push_back(pom2);
                }
            }
        }

        }
    }
    else {

        vector<string> black=board.getblackCoordinates();
        vector<string> white=board.getwhiteCoordinates();
        if(position[0]=='A') {
            string pom="";
            pom.push_back(char(position[1]-1));
            pom="B"+pom;
            if(!was_beaten && find(white.begin(),white.end(),pom)==white.end() && find(black.begin(),black.end(),pom)==black.end()) {
                validMoves.push_back(pom);
            }
            else if(pom[1]!='1' && find(black.begin(),black.end(),pom)==black.end() && find(white.begin(),white.end(),pom)!=white.end()){
                string pom2="";
                pom2.push_back('C');
                pom2.push_back(char(pom[1]-1));

                if(find(white.begin(),white.end(),pom2)==white.end() && find(black.begin(),black.end(),pom2)==black.end()) {
                    validMoves.push_back(pom2);
                }
            }
            if(position[1]!='8' && position[1]!='7')
            {
                string pom3="";
            pom3.push_back(char(position[1]+1));
            pom3="B"+pom3;
            if(pom3[1]!='8' && find(white.begin(),white.end(),pom3)!=white.end() && find(black.begin(),black.end(),pom3)==black.end()){
                string pom2="";

                pom2.push_back('C');
                pom2.push_back(char(pom3[1]+1));
                if(find(black.begin(),black.end(),pom2)==black.end() && find(white.begin(),white.end(),pom2)==white.end()) {
                    validMoves.push_back(pom2);
                }
            }
        }
        }
        else if(position[0]>='B' && position[0]<='G') {
            string pom1="";
            pom1.push_back(char(position[0]+1));
            pom1.push_back(char(position[1]-1));

            if(!was_beaten && find(white.begin(),white.end(),pom1)==white.end() && find(black.begin(),black.end(),pom1)==black.end()) {
                validMoves.push_back(pom1);
            }
            else if(pom1[1]!='1' && pom1[0]!='H' && find(black.begin(),black.end(),pom1)==black.end() && find(white.begin(),white.end(),pom1)!=white.end()){
                string pom2="";
                pom2.push_back(char(pom1[0]+1));
                pom2.push_back(char(pom1[1]-1));

                if(find(white.begin(),white.end(),pom2)==white.end() && find(black.begin(),black.end(),pom2)==black.end()) {
                    validMoves.push_back(pom2);
                }
            }
            if(position[1]!='8' && position[1]!='7')
            {
                string pom4="";
                pom4.push_back(char(position[0]+1));
                pom4.push_back(char(position[1]+1));
                if(pom4[1]!='8' && pom4[0]!='H' && find(white.begin(),white.end(),pom4)!=white.end() && find(black.begin(),black.end(),pom4)==black.end()){
                string pom2="";

                pom2.push_back(char(pom4[0]+1));
                pom2.push_back(char(pom4[1]+1));
                if(find(black.begin(),black.end(),pom2)==black.end() && find(white.begin(),white.end(),pom2)==white.end()) {
                    validMoves.push_back(pom2);
                }
            }
            }
            string pom3="";
            pom3.push_back(char(position[0]-1));
            pom3.push_back(char(position[1]-1));

            if(!was_beaten && find(white.begin(),white.end(),pom3)==white.end() && find(black.begin(),black.end(),pom3)==black.end()) {
                validMoves.push_back(pom3);
            }
            else if(pom3[1]!='1' && pom3[0]!='A' && find(black.begin(),black.end(),pom3)==black.end() && find(white.begin(),white.end(),pom3)!=white.end()){
                string pom4="";
                pom4.push_back(char(pom3[0]-1));
                pom4.push_back(char(pom3[1]-1));

                if(find(white.begin(),white.end(),pom4)==white.end() && find(black.begin(),black.end(),pom4)==black.end()) {
                    validMoves.push_back(pom4);
                }
            }
            if(position[1]!='8' && position[1]!='7')
            {
                string pom4="";
                pom4.push_back(char(position[0]-1));
                pom4.push_back(char(position[1]+1));
                if(pom4[1]!='8' && pom4[0]!='A' && find(white.begin(),white.end(),pom4)!=white.end() && find(black.begin(),black.end(),pom4)==black.end()){
                string pom2="";

                pom2.push_back(char(pom4[0]-1));
                pom2.push_back(char(pom4[1]+1));
                if(find(black.begin(),black.end(),pom2)==black.end() && find(white.begin(),white.end(),pom2)==white.end()) {
                    validMoves.push_back(pom2);
                }
            }
            }
        }
        else if(position[0]=='H') {
            string pom1="";
            pom1.push_back(char(position[0]-1));
            pom1.push_back(char(position[1]-1));

            if(!was_beaten && find(white.begin(),white.end(),pom1)==white.end() && find(black.begin(),black.end(),pom1)==black.end()) {
                validMoves.push_back(pom1);
            }
            else if(pom1[1]!='8' && find(black.begin(),black.end(),pom1)==black.end() && find(white.begin(),white.end(),pom1)!=white.end()){
                string pom2="";
                pom2.push_back(char(pom1[0]-1));
                pom2.push_back(char(pom1[1]-1));

                if(find(white.begin(),white.end(),pom2)==white.end() && find(black.begin(),black.end(),pom2)==black.end()) {
                    validMoves.push_back(pom2);
                }
            }
            if(position[1]!='8' && position[1]!='7')
            {
                string pom3="";
            pom3.push_back(char(position[1]+1));
            pom3="G"+pom3;
            if(pom3[1]!='8' && find(white.begin(),white.end(),pom3)!=white.end() && find(black.begin(),black.end(),pom3)==black.end()){
                string pom2="";

                pom2.push_back('F');
                pom2.push_back(char(pom3[1]+1));
                if(find(black.begin(),black.end(),pom2)==black.end() && find(white.begin(),white.end(),pom2)==white.end()) {
                    validMoves.push_back(pom2);
                }
            }
        }

        }
    }
    vector<string> validMoves2;
    validMoves2.clear();
    bool czy=0;
    for(int i=0;i<validMoves.size();i++)
    {
        if(abs(int(position[1])-int(validMoves[i][1]))>1)
        {
            validMoves2.push_back(validMoves[i]);
            czy=1;
        }
    }
    if(czy)
    return validMoves2;
    if(!czy)
    return validMoves;
}
Pawn::~Pawn() {

}

