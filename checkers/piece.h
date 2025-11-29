
class Piece {
    public:
    Piece(std::string pos,int col);
    Piece();
    void move(std::string newPosition);
    std::string getPosition();
    int getColor();
    std::string getTexture();
    void setColor(int col);
    virtual std::vector<std::string> getValidMoves(Board board,bool was_beaten)=0;
    virtual ~Piece();
protected:
    std::string position;
    int color;
    std::string texture;
};
