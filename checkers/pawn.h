
class Pawn:public Piece
{
    public:
    Pawn(std::string pos,int col);
    std::vector<std::string> getValidMoves(Board board,bool was_beaten);
    ~Pawn();
};
