#include <cstdlib>
#include <cstring>
#include <csignal>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

// Prosty serwer głosowania
// Protokół:
//  KLIENT -> SERWER: "NICK <nick>\n"        - ustawienie nicku (pusty = anonimXYZ)
//  SERWER -> KLIENT: "OK\n" / "ERR NICK_ZAJETY\n"
//  KLIENT -> SERWER: "JOIN\n"               - dołączenie do gry po ustawieniu nicku
//  SERWER -> KLIENT: "Dołączono do głosowania.\n"
//  KLIENT -> SERWER: "VOTE <n>\n"           - głos, gdzie 0<=n<=4 (0 = cofnięcie głosu)
//  SERWER -> KLIENT: "OK\n" / "ERR\n"
//  KLIENT -> SERWER: "RESULT\n"             - prośba o wyniki
//  SERWER -> KLIENT: "RESULT a b c d\n"     - aktualne wyniki
//  ADMIN  -> SERWER: "END\n"                - zakończenie głosowania, wysłanie wyniku do wszystkich
//  SERWER -> WSZYSCY: "GLOSOWANIE_ZAKONCZONE a b c d\n" i zamknięcie połączeń

static int servFd;
static std::unordered_set<int> clientFds;
static int votes[4] = {0,0,0,0};
static std::unordered_map<int, std::string> clientNames; // fd -> "ip:port" klienta
static std::unordered_map<int, int> clientChoice;        // fd -> aktualny głos (1-4 lub 0 = brak)
static std::unordered_map<int, std::string> clientNicks; // fd -> ustawiony nick klienta

static void errorExit(const char *msg) {
    perror(msg);
    exit(1);
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

    freeaddrinfo(res);
    return fd;
}

static void handle_command(int clientFd, const std::string &line) {
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
            const char err[] = "ERR NICK_ZAJETY\n";
            send(clientFd, err, sizeof(err)-1, 0);
        } else {
            clientNicks[clientFd] = nick;
            std::printf("Klient %s ustawił nick na '%s'\n", name, nick.c_str());
            const char ok[] = "OK\n";
            send(clientFd, ok, sizeof(ok)-1, 0);
        }
    } else if (line.rfind("JOIN", 0) == 0) {
        // Klient zgłasza chęć dołączenia do gry po ustawieniu nicku
        const char *who = name;
        std::printf("Klient %s dołączył do gry.\n", who);
        const char msg[] = "Dołączono do głosowania.\n";
        send(clientFd, msg, sizeof(msg)-1, 0);
    } else if (line.rfind("VOTE ", 0) == 0) {
        int n = std::atoi(line.c_str() + 5);
        if (n == 0) {
            // cofnięcie głosu
            int prev = 0;
            auto itc = clientChoice.find(clientFd);
            if (itc != clientChoice.end()) prev = itc->second;

            if (prev >= 1 && prev <= 4) {
                votes[prev-1]--;
                std::printf("Klient %s cofnął głos %d (VOTE 0)\n", name, prev);
            } else {
                std::printf("Klient %s wysłał VOTE 0, ale nie miał aktywnego głosu\n", name);
            }

            clientChoice[clientFd] = 0;
            const char ok[] = "OK\n";
            send(clientFd, ok, sizeof(ok)-1, 0);
        } else if (n >= 1 && n <= 4) {
            int prev = 0;
            auto itc = clientChoice.find(clientFd);
            if (itc != clientChoice.end()) prev = itc->second;

            if (prev == 0) {
                // pierwszy głos tego klienta
                votes[n-1]++;
                std::printf("Klient %s zagłosował na opcję %d\n", name, n);
            } else if (prev != n) {
                // zmiana głosu: zmniejsz poprzednią opcję, zwiększ nową
                if (prev >= 1 && prev <= 4) {
                    votes[prev-1]--;
                }
                votes[n-1]++;
                std::printf("Klient %s zmienił głos z %d na %d\n", name, prev, n);
            } else {
                // ponowne wysłanie tego samego głosu – brak zmian
                std::printf("Klient %s powtórzył głos na %d (bez zmian)\n", name, n);
            }

            clientChoice[clientFd] = n;
            const char ok[] = "OK\n";
            send(clientFd, ok, sizeof(ok)-1, 0);
        } else {
            const char err[] = "ERR\n";
            send(clientFd, err, sizeof(err)-1, 0);
        }
    } else if (line == "RESULT" || line == "RESULT\n") {
        char buf[128];
        std::snprintf(buf, sizeof(buf), "RESULT %d %d %d %d\n", votes[0], votes[1], votes[2], votes[3]);
        send(clientFd, buf, std::strlen(buf), 0);
    } else if (line == "END" || line == "END\n") {
        // Komenda administracyjna: zakończenie głosowania i wysłanie wyniku do wszystkich

        // znajdź maksymalną liczbę głosów
        int maxVotes = votes[0];
        for (int i = 1; i < 4; ++i) {
            if (votes[i] > maxVotes) maxVotes = votes[i];
        }

        // zbierz wszystkie opcje, które mają maxVotes
        int winners[4];
        int winnersCount = 0;
        for (int i = 0; i < 4; ++i) {
            if (votes[i] == maxVotes) {
                winners[winnersCount++] = i + 1; // opcje numerowane 1..4
            }
        }

        char buf[256];
        if (winnersCount <= 1) {
            // jedna wygrywająca opcja
            int winningOption = winnersCount == 1 ? winners[0] : 0;
            std::snprintf(buf, sizeof(buf),
                          "\nGLOSOWANIE_ZAKONCZONE %d %d %d %d \nWYGRALA_OPCJA %d\n",
                          votes[0], votes[1], votes[2], votes[3], winningOption);
        } else {
            // remis pomiędzy kilkoma opcjami
            // zbuduj listę opcji z remisem, np. "1 3 4"
            char remis[64];
            int offset = 0;
            for (int i = 0; i < winnersCount; ++i) {
                offset += std::snprintf(remis + offset, sizeof(remis) - offset,
                                        i == 0 ? "%d" : " %d", winners[i]);
                if (offset >= (int)sizeof(remis)) break;
            }

            std::snprintf(buf, sizeof(buf),
                          "\n\nGLOSOWANIE_ZAKONCZONE %d %d %d %d \nREMIS_MIEDZY_OPCJAMI %s\n",
                          votes[0], votes[1], votes[2], votes[3], remis);
        }

        std::string msg(buf);
        std::printf("[ADMIN] Zakończenie głosowania, wynik: %s", buf);

        // Wyślij do wszystkich klientów i zamknij połączenia
        for (int fd : clientFds) {
            send(fd, msg.c_str(), msg.size(), 0);
            shutdown(fd, SHUT_RDWR);
            close(fd);
        }
        clientFds.clear();
        clientNames.clear();
        clientNicks.clear();
        clientChoice.clear();
    } else {
        const char err[] = "ERR\n";
        send(clientFd, err, sizeof(err)-1, 0);
    }
}

int main(int argc, char **argv) {
    std::srand(std::time(nullptr));
    servFd = setup_server(argc, argv);
    signal(SIGINT, ctrl_c);
    signal(SIGPIPE, SIG_IGN);

    std::vector<pollfd> polls;
    polls.push_back({servFd, POLLIN, 0});

    // Bufory linii dla klientów
    std::unordered_map<int, std::string> buffers;

    while (true) {
        polls.resize(1);
        for (int fd : clientFds) polls.push_back({fd, POLLIN, 0});

        int ready = poll(polls.data(), polls.size(), -1);
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

        // istniejący klienci
        for (size_t i = 1; i < polls.size(); ++i) {
            if (!(polls[i].revents & POLLIN)) continue;
            int cfd = polls[i].fd;
            char buf[256];
            int n = read(cfd, buf, sizeof(buf));
            if (n <= 0) {
                // on disconnect, remove client vote from totals
                auto itVote = clientChoice.find(cfd);
                if (itVote != clientChoice.end()) {
                    int v = itVote->second;
                    if (v >= 1 && v <= 4) {
                        votes[v-1]--;
                        auto itNameNick = clientNicks.find(cfd);
                        const char *nm;
                        if (itNameNick != clientNicks.end()) nm = itNameNick->second.c_str();
                        else {
                            auto itName = clientNames.find(cfd);
                            nm = (itName != clientNames.end()) ? itName->second.c_str() : "?";
                        }
                        std::printf("Klient %s rozłączył się, usuwam jego głos %d\n", nm, v);
                    }
                    clientChoice.erase(itVote);
                }

                clientFds.erase(cfd);
                buffers.erase(cfd);
                clientNames.erase(cfd);
                clientNicks.erase(cfd);
                close(cfd);
                continue;
            }
            buffers[cfd].append(buf, n);
            auto &acc = buffers[cfd];
            size_t pos;
            while ((pos = acc.find('\n')) != std::string::npos) {
                std::string line = acc.substr(0, pos);
                if (!line.empty() && line.back() == '\r') line.pop_back();
                acc.erase(0, pos+1);
                handle_command(cfd, line);
            }
        }
    }
}
