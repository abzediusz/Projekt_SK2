

class King:public Piece {
    public:
    King(std::string pos,int col);
    std::vector<std::string> getValidMoves(Board board,bool was_beaten);
    ~King();
};
