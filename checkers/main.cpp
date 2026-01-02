//#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <unordered_set>
#include <unordered_map>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include<typeinfo>
#include<cmath>
#include<string>
#include <chrono>
#include <time.h>
#include<fcntl.h>
#include "board.h"
#include "piece.h"
#include "pawn.h"
#include "game.h"
#include<optional>

#include "king.h"
using namespace std;
using Clock = std::chrono::steady_clock;
const int TIMEOUT=60000;
const float X0_B=139;
const float Y0_B=37;
const float X0_W=90;
const float Y0_W=355;
const float DX=93;
const float DY=47;
const float S=0.18;
Board board;
Game game;
vector<Piece*> whitePieces;
vector<Piece*> blackPieces;
vector<string> white=board.getwhiteCoordinates();
vector<string> black=board.getblackCoordinates();
vector<string> blackTex=board.getblackTextures();
vector<string> whiteTex=board.getwhiteTextures();
static int servFd;
bool was_beaten=0;
static string state="PREPARE";
static std::unordered_set<int> clientFds;
static int ready_players = 0;
static std::unordered_map<string,int> votes;
static std::unordered_map<int,int> spectators;
static std::unordered_map<int, int> clientColors;
static std::unordered_map<int, std::string> clientNames; // fd -> "ip:port" klienta
static std::unordered_map<int, string> clientReady;  
static std::unordered_map<int, string> clientChoice;        // fd -> aktualny głos (1-4 lub 0 = brak)
static std::unordered_map<int, std::string> clientNicks;
static std::unordered_map<int, vector<char>> sendBuffers;
Clock::time_point start1;
void setnonblock(int fd) {
int flags = fcntl(fd, F_GETFL, 0);
fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
static void queueSend(int fd, const char* data, size_t len) {
    auto& buf = sendBuffers[fd];
    buf.insert(buf.end(), data, data + len);
}

static void queueSend(int fd, const string& msg) {
    queueSend(fd, msg.c_str(), msg.size());
}
static bool trySend(int fd) {
    auto it=sendBuffers.find(fd);
    if (it==sendBuffers.end() || it->second.empty()) {
        return true; 
    }
    
    auto& buf=it->second;
    ssize_t sent=send(fd,buf.data(),buf.size(),MSG_DONTWAIT | MSG_NOSIGNAL);
    
    if (sent > 0) {
        buf.erase(buf.begin(), buf.begin() + sent);
        return buf.empty();
    } else if (sent==-1 && (errno==EAGAIN || errno==EWOULDBLOCK)) {
        return false; 
    } else {
        return true; 
    }
}
void zakoncz_gre()
{
    if(game.getWinner()==0)
        {
            cout<<"Wygrali biali"<<endl;
            for(int cfd: clientFds)
            {
                //write(cfd,"WHITE WON\n",11);
                queueSend(cfd, "WHITE WON\n");
            }

        }
        if(game.getWinner()==1)
        {
            cout<<"Wygrali czarni"<<endl;
            for(int cfd: clientFds)
            {
                //write(cfd,"BLACK WON\n",11);
                queueSend(cfd, "BLACK WON\n");
            }
            
        }
        state="PREPARE";
        spectators.clear();
}
bool czy_bije(Board board,string przed,string po,int color)
{
    vector<string> white=board.getwhiteCoordinates();
    vector<string> black=board.getblackCoordinates();
    if(color==0)
    {
            if(przed[0]<po[0] && przed[1]<po[1])
            {
                string pom="";
                pom.push_back(char(po[0]-1));
                pom.push_back(char(po[1]-1));
                if(find(black.begin(),black.end(),pom)!=black.end())
                {
                    return true;
                }
                return false;
            }
            else if(przed[0]<po[0] && przed[1]>po[1])
            {
                string pom="";
                pom.push_back(char(po[0]-1));
                pom.push_back(char(po[1]+1));
                if(find(black.begin(),black.end(),pom)!=black.end())
                {
                    return true;
                }
                return false;
            }
            else if(przed[0]>po[0] && przed[1]<po[1])
            {
                string pom="";
                pom.push_back(char(po[0]+1));
                pom.push_back(char(po[1]-1));
                if(find(black.begin(),black.end(),pom)!=black.end())
                {
                    return true;
                }
                return false;
            }
            else if(przed[0]>po[0] && przed[1]>po[1])
            {
                string pom="";
                pom.push_back(char(po[0]+1));
                pom.push_back(char(po[1]+1));
                if(find(black.begin(),black.end(),pom)!=black.end())
                {
                    return true;
                }
                return false;
            }
            
    }
    else
    {
        if(przed[0]<po[0] && przed[1]<po[1])
            {
                string pom="";
                pom.push_back(char(po[0]-1));
                pom.push_back(char(po[1]-1));
                if(find(white.begin(),white.end(),pom)!=white.end())
                {
                    return true;
                }
                return false;
            }
            else if(przed[0]<po[0] && przed[1]>po[1])
            {
                string pom="";
                pom.push_back(char(po[0]-1));
                pom.push_back(char(po[1]+1));
                if(find(white.begin(),white.end(),pom)!=white.end())
                {
                    return true;
                }
                return false;
            }
            else if(przed[0]>po[0] && przed[1]<po[1])
            {
                string pom="";
                pom.push_back(char(po[0]+1));
                pom.push_back(char(po[1]-1));
                if(find(white.begin(),white.end(),pom)!=white.end())
                {
                    return true;
                }
                return false;
            }
            else if(przed[0]>po[0] && przed[1]>po[1])
            {
                string pom="";
                pom.push_back(char(po[0]+1));
                pom.push_back(char(po[1]+1));
                if(find(white.begin(),white.end(),pom)!=white.end())
                {
                    return true;
                }
                return false;
            }
    }
    return false;
}
static void errorExit(const char *msg) {
    perror(msg);
    exit(1);
}
void wyswietl(int cfd)
{
    string biale="";
                            string czarne="";
                            for(int i=0;i<whitePieces.size();i++)
                            {
                                string pom=whitePieces[i]->getPosition();
                                pom=pom+" ";
                                string type=typeid(*whitePieces[i]).name();
                                if(type.find("King")!=string::npos) {
                                        pom="K"+pom;
                                }
                                biale=biale+pom;

                            }
                            biale=biale+"\n";
                            for(auto i=0;i<whitePieces.size();i++)
                            {
                                string pom=blackPieces[i]->getPosition();
                                pom=pom+" ";
                                string type=typeid(*blackPieces[i]).name();
                                if(type.find("King")!=string::npos) {
                                        pom="K"+pom;
                                }
                                czarne=czarne+pom;

                            }
                            czarne=czarne+"\n";
                            if(cfd==-1)
                            {for(int fd: clientFds)
                            {
                                /*write(fd,biale.c_str(),biale.size());
                                write(fd,czarne.c_str(),czarne.size());*/
                                //cout<<biale<<czarne;
                                queueSend(fd, biale);
                                queueSend(fd, czarne);
                            }
                            start1=Clock::now();}
                            else
                            {
                                /*write(cfd,biale.c_str(),biale.size());
                                write(cfd,czarne.c_str(),czarne.size());*/
                                queueSend(cfd, biale);
                                queueSend(cfd, czarne);
                            }
}
static void ctrl_c(int) {
    const char bye[] = "[Serwer kończy pracę]\n";
    for(int fd : clientFds) {
        send(fd, bye, sizeof(bye)-1, MSG_DONTWAIT);
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
    if(servFd >= 0) close(servFd);
    printf("Zamykanie serwera głosowania\n");
    exit(0);
}
static int setup_server(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s <port>\n", argv[0]);
        exit(1);
    }

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    addrinfo *res;
    int rv = getaddrinfo(nullptr, argv[1], &hints, &res);
    if (rv != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd == -1) errorExit("socket");

    int one = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    if (bind(fd, res->ai_addr, res->ai_addrlen) < 0) errorExit("bind");
    if (listen(fd, 8) < 0) errorExit("listen");
    setnonblock(fd);
    freeaddrinfo(res);
    return fd;
}
int ile_kolor()
{
    int w=0;
    for(auto it=clientColors.begin();it!=clientColors.end();it++)
    {
        if(it->second==game.getCurrentPlayer())
        {
            w++;
        }
    }
    cout<<w<<endl;
    return w;
}
static void handle_command_prepare(int clientFd, const std::string &line) {
    const char *name = "?";
    auto itNick = clientNicks.find(clientFd);
    
    if (itNick != clientNicks.end()) {
        name = itNick->second.c_str();
    } else {
        auto it = clientNames.find(clientFd);
        if (it != clientNames.end()) name = it->second.c_str();
    }
    if (line.rfind("NICK ", 0) == 0) {
        std::string nick = line.substr(5);

        // pusty nick -> wygeneruj anonimXYZ z losowym numerem
        if (nick.empty()) {
            int start = std::rand() % 1000; // losowy start 0-999
            for (int offset = 0; offset < 1000; ++offset) {
                int num = (start + offset) % 1000;
                char tmp[16];
                std::snprintf(tmp, sizeof(tmp), "anonim%03d", num);
                std::string cand(tmp);
                bool used = false;
                for (const auto &entry : clientNicks) {
                    if (entry.second == cand) { used = true; break; }
                }
                if (!used) { nick = cand; break; }
            }
        }

        bool taken = false;
        for (const auto &entry : clientNicks) {
            if (entry.first == clientFd) continue; // ten sam klient
            if (entry.second == nick) {
                taken = true;
                break;
            }
        }

        if (taken) {
            std::printf("Klient %s próbował ustawić zajęty nick '%s'\n", name, nick.c_str());
            //const char err[] = "ERR NICK_ZAJETY\n";
            //send(clientFd, err, sizeof(err)-1, 0);
            queueSend(clientFd, "ERR NICK_ZAJETY\n");
        } else {
            clientNicks[clientFd] = nick;
            std::printf("Klient %s ustawił nick na '%s'\n", name, nick.c_str());
            /*const char ok[] = "OK\n";
            send(clientFd, ok, sizeof(ok)-1, 0);*/
            queueSend(clientFd, "OK\n");
        }
    } else if (line.rfind("JOIN", 0) == 0) {
        // Klient zgłasza chęć dołączenia do gry po ustawieniu nicku
        const char *who = name;
        std::printf("Klient %s dołączył do gry.\n", who);
        if(state=="PREPARE")
        {/*const char msg[] = "Dołączono do głosowania.\n";
        send(clientFd, msg, sizeof(msg)-1, 0);*/
        queueSend(clientFd, "Dołączono do głosowania.\n");
        }
        else
        {
            spectators[clientFd]=1;
            /*const char msg[] = "SPECTATE.\n";
        send(clientFd, msg, sizeof(msg)-1, 0);*/
        queueSend(clientFd, "SPECTATE.\n");
        wyswietl(clientFd);
        }
    } else if (line.rfind("READY", 0) == 0) {
       
           
                ready_players++;
                std::printf("Klient %s jest gotowy\n", name);
            

            clientChoice[clientFd] = "1";
            /*const char ok[] = "OK\n";
            send(clientFd, ok, sizeof(ok)-1, 0);*/
            queueSend(clientFd, "OK\n");
        } 
        else if (line.rfind("COLOR", 0) == 0) {
       
           
                int c=atoi(line.c_str()+6);
                clientColors[clientFd]=c;
                std::printf("Klient %s wybrał kolor %d\n", name,c);
            
            /*const char ok[] = "OK\n";
            send(clientFd, ok, sizeof(ok)-1, 0);*/
            queueSend(clientFd, "OK\n");
        } 
     else if (line == "RESULT" || line == "RESULT\n") {
        char buf[128];
        std::snprintf(buf, sizeof(buf), "RESULT %d\n", ready_players);
        //send(clientFd, buf, std::strlen(buf), 0);
        queueSend(clientFd, buf);
    } 
        else {
        const char err[] = "ERR\n";
        //send(clientFd, err, sizeof(err)-1, 0);
        queueSend(clientFd, "ERR\n");
    }
        if (ready_players==clientFds.size()) {
            // jedna wygrywająca opcja
            if(clientColors.size()!=clientFds.size())
            {
                srand(time(0));
                for(int i: clientFds)
                {
                    if(clientColors.find(i)==clientColors.end())
                    {
                        clientColors[i]=rand()%2;
                    }

                }
            }
            int suma=0;
            for(auto it=clientColors.begin();it!=clientColors.end();it++)
            {
                suma+=it->second;
            }
            if(suma==0)
            {
                clientColors[*(next(clientFds.begin(),rand()%clientFds.size()))]=1;
            }
            else if(suma==ready_players)
            {
                clientColors[*(next(clientFds.begin(),rand()%clientFds.size()))]=0;
            }
            char buf[100];
            std::snprintf(buf, sizeof(buf),"PLAY\n");
            state="PLAY";
            std::string msg(buf);
        std::printf("[ADMIN] Zakończenie głosowania, wynik: %s", buf);
            for (int fd : clientFds) {
            //send(fd, msg.c_str(), msg.size(), 0);
            queueSend(fd, "PLAY\n");
            }
            wyswietl(-1);
            
        clientChoice.clear();

        }
        
        
        
    
}
static void handle_command(int clientFd, const std::string &line,string &wynik) {
    const char *name = "?";
    auto itNick = clientNicks.find(clientFd);
    if (itNick != clientNicks.end()) {
        name = itNick->second.c_str();
    } else {
        auto it = clientNames.find(clientFd);
        if (it != clientNames.end()) name = it->second.c_str();
    }
    if (line.rfind("m", 0) == 0 && spectators.find(clientFd)==spectators.end()) {
       string vote = line.c_str() + 2;
       string skad=vote.substr(0,2);
       string dokad=vote.substr(3,2);
       bool valid=1;
                if(game.getCurrentPlayer()==0 && clientColors[clientFd]==0)
                {
                    int ind=-1;
                    for(int i=0;i<whitePieces.size();i++)
                    {
                        if(whitePieces[i]->getPosition()==skad)
                        {
                            ind=i;
                            break;
                        }
                    }
                    if(ind==-1)
                    {
                        valid=0;
                    }
                    if(valid)
                    {
                        bool czy=0;
                        vector<string> val;
                        val.clear();
                        val=whitePieces[ind]->getValidMoves(board,was_beaten);
                        if(val.size()>0)
                                {for(int i=0;i<whitePieces.size();i++)
                                {
                                    vector<string> temp;
                                    temp.clear();
                                    temp=(*whitePieces[i]).getValidMoves(board,was_beaten);
                                    if(temp.size()>0 )
                                    {
                                        /*if(abs(temp[0][1]-(*whitePieces[i]).getPosition()[1])>1 && abs(val[0][1]-whitePieces[ind]->getPosition()[1])==1)
                                        {
                                            val.clear();
                                            break;
                                        }*/
                                       if(czy_bije(board,whitePieces[i]->getPosition(),temp[0],game.getCurrentPlayer()) && !czy_bije(board,whitePieces[ind]->getPosition(),val[0],game.getCurrentPlayer()))
                                        {
                                            //cout<<"KKK"<<endl;
                                            val.clear();
                                            break;
                                        }
                                    }
                                }}
                        for(int i=0;i<val.size();i++)
                        {
                            if(dokad==val[i])
                            {
                                czy=1;
                                break;
                            }
                        }
                        if(!czy)
                        {
                            valid=0;
                        }
                    }
                }
                else if(game.getCurrentPlayer()==1 && clientColors[clientFd]==1)
                {
                    int ind=-1;
                    for(int i=0;i<blackPieces.size();i++)
                    {
                        if(blackPieces[i]->getPosition()==skad)
                        {
                            ind=i;
                            break;
                        }
                    }
                    if(ind==-1)
                    {
                        valid=0;
                    }
                    if(valid)
                    {
                        bool czy=0;
                        vector<string> val;
                        val.clear();
                        val=blackPieces[ind]->getValidMoves(board,was_beaten);
                        if(val.size()>0)
                                {for(int i=0;i<blackPieces.size();i++)
                                {
                                    vector<string> temp;
                                    temp.clear();
                                    temp=(*blackPieces[i]).getValidMoves(board,was_beaten);
                                    if(temp.size()>0 )
                                    {
                                        /*if(abs(temp[0][1]-(*blackPieces[i]).getPosition()[1])>1 && abs(val[0][1]-blackPieces[ind]->getPosition()[1])==1)
                                        {
                                            val.clear();
                                            break;
                                        }*/
                                       if(czy_bije(board,blackPieces[i]->getPosition(),temp[0],game.getCurrentPlayer()) && !czy_bije(board,blackPieces[ind]->getPosition(),val[0],game.getCurrentPlayer()))
                                        {
                                            val.clear();
                                            break;
                                        }
                                    }
                                }}
                        for(int i=0;i<val.size();i++)
                        {
                            if(dokad==val[i])
                            {
                                czy=1;
                                break;
                            }
                        }
                        if(!czy)
                        {
                            valid=0;
                        }
                    }
                }
                /*else
                {
                    valid=0;
                }*/
                if(valid && game.getCurrentPlayer()==clientColors[clientFd])
                {if(votes.find(vote)==votes.end())
                {
                    votes[vote]=1;

                }
                else
                {
                    votes[vote]++;
                }
                //std::printf("Klient %s zagłosował na %s\n", name,vote);
                cout<<"Klient "<<name<<" zagłosował na "<<vote<<"\n";

            clientChoice[clientFd] = vote;
            /*const char ok[] = "OK\n";
            send(clientFd, ok, sizeof(ok)-1, 0);*/
            queueSend(clientFd, "OK\n");}
            
            else
            {
                //send(clientFd, "Invalid move\n", 14, 0);
                queueSend(clientFd, "Invalid move\n");
            }
        } 
     else if (line == "RESULT" || line == "RESULT\n") {
        string result="";
        for(auto it=votes.begin();it!=votes.end();it++)
        {
            result=result+it->first+to_string(it->second)+" ";
        }
        result=result+"\n";
        //send(clientFd, result.c_str(), result.size(), 0);
        queueSend(clientFd, result);
    } 
        else {
        /*const char err[] = "ERR\n";
        send(clientFd, err, sizeof(err)-1, 0);*/
        queueSend(clientFd, "ERR\n");
    }
    int maks=0;
    for(auto i=votes.begin();i!=votes.end();i++)
    {
        if(i->second>maks)
        {
            maks=i->second;
        }
    }
        if (maks>ile_kolor()/2 || line == "END" || line == "END\n" || votes.size()==ile_kolor()) {
            if((line == "END" || line == "END\n") && votes.size()==0)
            {
                
                cout<<"[ADMIN] Zakończenie głosowania, brak głosów"<<endl;
                for(auto it=clientColors.begin();it!=clientColors.end();it++)
                {
                    if(it->second==game.getCurrentPlayer())
                    //send(it->first,"TIMEOUT\n",9,0);
                    queueSend(it->first, "TIMEOUT\n");
                }
                game.setCurrentPlayer(1-game.getCurrentPlayer());
                clientChoice.clear();
            votes.clear();
                return;
            }
            for (auto it = votes.begin(); it != votes.end(); ++it)
            {
                    if (it->second == maks)
                {
                    wynik=it->first;
                    break;
                }
            }
        //std::printf("[ADMIN] Zakończenie głosowania, wynik: %s", wynik);
        cout<<"[ADMIN] Zakończenie głosowania, wynik: "<<wynik<<endl;
            
            
        clientChoice.clear();
            votes.clear();
        }
        
        
        
        
    
}
int check()
{
    bool jest_bialy=0;
    bool jest_czarny=0;
    for(auto it=clientColors.begin();it!=clientColors.end();it++)
    {
        if(it->second==0)
        {
            jest_bialy=1;
        }
        else if(it->second==1)
        {
            jest_czarny=1;
        }
        if(jest_bialy && jest_czarny)
        {
            return -1;
        }
    }
    if(jest_bialy && jest_czarny)
        {
            return -1;
        }
        if(jest_bialy)
        {
            return 0;
        }
        return 1;
}
int main(int argc,char** argv) {
    
    
    srand(time(nullptr));
    servFd = setup_server(argc, argv);
    signal(SIGINT, ctrl_c);
    signal(SIGPIPE, SIG_IGN);

    std::vector<pollfd> polls;
    polls.push_back({servFd, POLLIN, 0});

    // Bufory linii dla klientów
    std::unordered_map<int, std::string> buffers;
    
    for(int i=0;i<12;i++) {
        Piece* newPawn=new Pawn(white[i],0);
        whitePieces.push_back(newPawn);
    }
   
    for(int i=0;i<12;i++) {
        Piece* newPiece=new Pawn(black[i],1);
        blackPieces.push_back(newPiece);
    }
    bool chosen=0;

    vector<string> val;
    
    string wynik="XX";
    int currentColor;
    while(1) {
        //sf::Event event;
        polls.resize(1);
        for (int fd : clientFds){
            short events = POLLIN | POLLHUP;
            if (!sendBuffers[fd].empty()) {
                events |= POLLOUT;
            }
            polls.push_back({fd, POLLIN, 0});

        } 

        int ready = poll(polls.data(), polls.size(), 10);
        if (ready < 0) {
            perror("poll");
            continue;
        }

        // nowe połączenia
        if (polls[0].revents & POLLIN) {
            sockaddr_storage addr{};
            socklen_t alen = sizeof(addr);
            int cfd = accept(servFd, (sockaddr*)&addr, &alen);
            if (cfd < 0) { perror("accept"); continue; }
            clientFds.insert(cfd);
            setnonblock(cfd);
            if(state=="PLAY")
            {
                spectators[cfd]=0;
            }
            buffers[cfd] = "";

            // zapamiętaj adres klienta jako "ip:port"
            char host[NI_MAXHOST], serv[NI_MAXSERV];
            if (getnameinfo((sockaddr*)&addr, alen, host, sizeof(host), serv, sizeof(serv),
                            NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
                clientNames[cfd] = std::string(host) + ":" + serv;
            } else {
                clientNames[cfd] = "unknown";
            }
            std::printf("Nowy klient połączony: %s (fd=%d)\n", clientNames[cfd].c_str(), cfd);
        }
        wynik="XX";
        
        for (size_t i = 1; i < polls.size(); ++i) {
            int cfd = polls[i].fd;
            if(polls[i].revents & POLLHUP)
            {
                    auto itVote = clientChoice.find(cfd);
                if (itVote != clientChoice.end()) {
                    string v = itVote->second;
                    if (v=="1" || (v[0]>='A' && v[0]<='H')) {
                        ready_players--;
                        auto itNameNick = clientNicks.find(cfd);
                        const char *nm;
                        if (itNameNick != clientNicks.end()) nm = itNameNick->second.c_str();
                        else {
                            auto itName = clientNames.find(cfd);
                            nm = (itName != clientNames.end()) ? itName->second.c_str() : "?";
                        }
                        //std::printf("Klient %s rozłączył się %s\n", nm, v);
                        cout<<"Klient "<<nm<<" rozłączył się\n";
                    }
                    if((v[0]>='A' && v[0]<='H'))
                    {
                        votes[v]--;
                    }
                    clientChoice.erase(itVote);
                }
                if(spectators.find(cfd)!=spectators.end())
                {
                    spectators.erase(cfd);
                }
                
                clientFds.erase(cfd);
                buffers.erase(cfd);
                clientColors.erase(cfd);
                clientNames.erase(cfd);
                clientNicks.erase(cfd);
                sendBuffers.erase(cfd);
                shutdown(cfd,SHUT_RDWR);
                close(cfd);
                if(check()!=-1)
                {
                    game.setWinner(check());
                    game.setGameOver(1);
                    zakoncz_gre();
                }
                continue;
            }
            if (polls[i].revents & POLLOUT) {
                trySend(cfd);
            }
            if (polls[i].revents & POLLIN) 
            {
            char buf[256];
            int n = read(cfd, buf, sizeof(buf));
            if (n <= 0) {
                // on disconnect, remove client vote from totals
                auto itVote = clientChoice.find(cfd);
                if (itVote != clientChoice.end()) {
                    string v = itVote->second;
                    if (v=="1" || (v[0]>='A' && v[0]<='H')) {
                        ready_players--;
                        auto itNameNick = clientNicks.find(cfd);
                        const char *nm;
                        if (itNameNick != clientNicks.end()) nm = itNameNick->second.c_str();
                        else {
                            auto itName = clientNames.find(cfd);
                            nm = (itName != clientNames.end()) ? itName->second.c_str() : "?";
                        }
                        //std::printf("Klient %s rozłączył się %s\n", nm, v);
                        cout<<"Klient "<<nm<<" rozłączył się\n";
                    }
                    if((v[0]>='A' && v[0]<='H'))
                    {
                        votes[v]--;
                    }
                    clientChoice.erase(itVote);
                }
                if(spectators.find(cfd)!=spectators.end())
                {
                    spectators.erase(cfd);
                }
                
                clientFds.erase(cfd);
                buffers.erase(cfd);
                clientColors.erase(cfd);
                clientNames.erase(cfd);
                clientNicks.erase(cfd);
                sendBuffers.erase(cfd);
                if(check()!=-1)
                {
                    game.setWinner(check());
                    game.setGameOver(1);
                    zakoncz_gre();
                }
                shutdown(cfd,SHUT_RDWR);
                close(cfd);
                continue;
            }
            if(state=="PREPARE" || (spectators.find(cfd)!=spectators.end() && spectators[cfd]==0))
            {
                buffers[cfd].append(buf, n);
            auto &acc = buffers[cfd];
            size_t pos;
            while ((pos = acc.find('\n')) != std::string::npos) {
                std::string line = acc.substr(0, pos);
                if (!line.empty() && line.back() == '\r') line.pop_back();
                acc.erase(0, pos+1);
                handle_command_prepare(cfd, line);
            }
        }
        else if(state=="PLAY")
        {
            
            
            buffers[cfd].append(buf, n);
            auto &acc = buffers[cfd];
            size_t pos;
            
            while ((pos = acc.find('\n')) != std::string::npos) {
                
                std::string line = acc.substr(0, pos);
                if (!line.empty() && line.back() == '\r') line.pop_back();
                acc.erase(0, pos+1);
                handle_command(cfd, line,wynik);
            }
            
        }
    }
    
    }
    if(state=="PLAY" && wynik=="XX" && !game.getGameOver())
    {
        auto teraz = Clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(teraz - start1);
            if(elapsed.count()>TIMEOUT && wynik=="XX")
            {
                
                handle_command(1, "END",wynik);
            }
    }
    if(state=="PLAY" && wynik!="XX")
    {
        currentColor=game.getCurrentPlayer();
        string a(1,wynik[0]);
        string b(1,wynik[1]);
        string pos=a+b;
        string c(1,wynik[3]);
        string d(1,wynik[4]);
        string to=c+d;
        

        if(currentColor==0) {
                            
                            int ind;
        for(int i=0;i<whitePieces.size();i++) {
            if((*whitePieces[i]).getPosition()==pos) {
                        ind=i;
                        break;
                }
        }
                            
                                string oldPosition=pos;
                                (*whitePieces[ind]).move(to);
                                string newPosition=to;

                                if(abs(int(newPosition[1])-int(oldPosition[1]))>=2) {
                                    string beaten="";
                                    string whichClass=typeid(*whitePieces[ind]).name();
                                    if(whichClass.find("Pawn")!=string::npos) {
                                        if(newPosition[0]>oldPosition[0] && newPosition[1]>oldPosition[1]) {
                                            beaten.push_back(newPosition[0]-1);
                                            beaten.push_back(newPosition[1]-1);
                                        }
                                        else if(newPosition[0]<oldPosition[0] && newPosition[1]>oldPosition[1]) {
                                            beaten.push_back(newPosition[0]+1);
                                            beaten.push_back(newPosition[1]-1);
                                        }
                                        else if(newPosition[0]>oldPosition[0] && newPosition[1]<oldPosition[1]) {
                                            beaten.push_back(newPosition[0]-1);
                                            beaten.push_back(newPosition[1]+1);
                                        }
                                        else if(newPosition[0]<oldPosition[0] && newPosition[1]<oldPosition[1]) {
                                            beaten.push_back(newPosition[0]+1);
                                            beaten.push_back(newPosition[1]+1);
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
                                        int ind3=-1;
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
                                        white.clear();
                                        black.clear();
                                        
                                        for(int i=0;i<whitePieces.size();i++) {
                                            white.push_back((*whitePieces[i]).getPosition());
                                            
                                        }
                                        for(int i=0;i<blackPieces.size();i++) {
                                            black.push_back((*blackPieces[i]).getPosition());
                                            
                                        }


                                        board.setupBoard(white,black);
                                        string type=typeid(*whitePieces[ind]).name();
                                        
                                            was_beaten=1;
                                        

                                        if( (whitePieces[ind]->getValidMoves(board,was_beaten)).empty()) {
                                            was_beaten=0;
                                        }
                                        else  {
                                            val=(*whitePieces[ind]).getValidMoves(board,was_beaten);
                                        }
                                    }
                                }
                                if(whitePieces[ind]->getPosition()[1]=='8') {
                                    
                                    whitePieces[ind]=new King(newPosition,0);
                                    was_beaten=0;
                                }
                                if(!was_beaten)
                                game.setCurrentPlayer(1);

                            


                           

                        }




                        else {
                                int ind;
        for(int i=0;i<blackPieces.size();i++) {
            if((*blackPieces[i]).getPosition()==pos) {
                        ind=i;
                        break;
                }
        }
                                string oldPosition=pos;
                                (*blackPieces[ind]).move(to);
                                string newPosition=to;
                                if(abs(int(newPosition[1])-int(oldPosition[1]))>=2) {
                                    string beated="";
                                    string whichClass=typeid(*blackPieces[ind]).name();
                                    if(whichClass.find("Pawn")!=string::npos) {
                                        if(newPosition[0]>oldPosition[0] && newPosition[1]<oldPosition[1]) {
                                            beated.push_back(newPosition[0]-1);
                                            beated.push_back(newPosition[1]+1);
                                        }
                                        else if(newPosition[0]<oldPosition[0] && newPosition[1]<oldPosition[1]) {
                                            beated.push_back(newPosition[0]+1);
                                            beated.push_back(newPosition[1]+1);
                                        }
                                        else if(newPosition[0]>oldPosition[0] && newPosition[1]>oldPosition[1]) {
                                            beated.push_back(newPosition[0]-1);
                                            beated.push_back(newPosition[1]-1);
                                        }
                                        else if(newPosition[0]<oldPosition[0] && newPosition[1]>oldPosition[1]) {
                                            beated.push_back(newPosition[0]+1);
                                            beated.push_back(newPosition[1]-1);
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
                                        int ind3=-1;
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


                                        board.setupBoard(white,black);
                                        string type=typeid(*blackPieces[ind]).name();
                                        
                                        was_beaten=1;
                                        
                                        if( (blackPieces[ind]->getValidMoves(board,was_beaten)).empty()) {
                                            was_beaten=0;
                                        }
                                        else  {
                                            val=blackPieces[ind]->getValidMoves(board,was_beaten);
                                        }
                                    }
                                }
                                    if(blackPieces[ind]->getPosition()[1]=='1') {
                                        delete blackPieces[ind];
                                        blackPieces[ind]=new King(newPosition,1);
                                        was_beaten=0;
                                    }
                                if(!was_beaten)
                                game.setCurrentPlayer(0);
                                

                            }
                            wyswietl(-1);
                            white.clear();
    black.clear();
    
    for(int i=0;i<whitePieces.size();i++) {
        white.push_back((*whitePieces[i]).getPosition());
        //whiteTex.push_back((*whitePieces[i]).getTexture());
    }
    for(int i=0;i<blackPieces.size();i++) {
        black.push_back((*blackPieces[i]).getPosition());
        //blackTex.push_back((*blackPieces[i]).getTexture());
    }


    board.setupBoard(white,black);
    if(game.getGameOver())
    {
        zakoncz_gre();
    }     
    }
        /*for(int i=0;i<polls.size();i++)
        {
            int fd=polls[i].fd;
            if(polls[i].revents & POLLOUT)
                trySend(fd);
        }*/
    for(int fd : clientFds) {
            if(!sendBuffers[fd].empty()) {
                bool correct=trySend(fd);
                if(!correct)
                {
                    auto itVote = clientChoice.find(fd);
                if (itVote != clientChoice.end()) {
                    string v = itVote->second;
                    if (v=="1" || (v[0]>='A' && v[0]<='H')) {
                        ready_players--;
                        auto itNameNick = clientNicks.find(fd);
                        const char *nm;
                        if (itNameNick != clientNicks.end()) nm = itNameNick->second.c_str();
                        else {
                            auto itName = clientNames.find(fd);
                            nm = (itName != clientNames.end()) ? itName->second.c_str() : "?";
                        }
                        //std::printf("Klient %s rozłączył się %s\n", nm, v);
                        cout<<"Klient "<<nm<<" rozłączył się\n";
                    }
                    if((v[0]>='A' && v[0]<='H'))
                    {
                        votes[v]--;
                    }
                    clientChoice.erase(itVote);
                }
                if(spectators.find(fd)!=spectators.end())
                {
                    spectators.erase(fd);
                }
                
                clientFds.erase(fd);
                buffers.erase(fd);
                clientColors.erase(fd);
                clientNames.erase(fd);
                clientNicks.erase(fd);
                sendBuffers.erase(fd);
                if(check()!=-1)
                {
                    game.setWinner(check());
                    game.setGameOver(1);
                    zakoncz_gre();
                }
                shutdown(fd,SHUT_RDWR);
                close(fd);
                
                }
            }
        }
}
    return 0;
    }

