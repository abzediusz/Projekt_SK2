#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <atomic>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

static void errorExit(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        return 1;
    }

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo *res;
    int rv = getaddrinfo(argv[1], argv[2], &hints, &res);
    if (rv != 0) {
        std::fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) errorExit("socket");
    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) errorExit("connect");
    freeaddrinfo(res);

    // Ustawienie nicku na początku – w pętli aż do OK
    while (true) {
        char nickBuf[64];
        std::printf("Podaj swój nick (puste = anonim): ");
        std::fflush(stdout);

        if (!std::fgets(nickBuf, sizeof(nickBuf), stdin)) {
            std::fprintf(stderr, "Błąd odczytu nicku.\n");
            close(sock);
            return 1;
        }

        size_t len = std::strlen(nickBuf);
        if (len > 0 && nickBuf[len-1] == '\n') nickBuf[len-1] = '\0';

        std::string nickCmd = "NICK ";
        nickCmd += nickBuf;  // może być pusty => anonimXYZ po stronie serwera
        nickCmd += "\n";

        if (write(sock, nickCmd.c_str(), nickCmd.size()) != (ssize_t)nickCmd.size()) {
            errorExit("write");
        }

        char nb[256];
        int rn = read(sock, nb, sizeof(nb)-1);
        if (rn <= 0) {
            std::fprintf(stderr, "Serwer zakończył połączenie przy ustawianiu nicku.\n");
            close(sock);
            return 1;
        }
        nb[rn] = '\0';

        if (std::strncmp(nb, "OK\n", 2) == 0) {
            std::printf("Nick ustawiony: %s", nb);
            break;
        } else if (std::strncmp(nb, "ERR NICK_ZAJETY\n", 16) == 0) {
            std::printf("Ten nick jest już zajęty, spróbuj inny.\n");
            continue; // wróć na początek pętli i zapytaj ponownie
        } else {
            std::printf("Nieoczekiwana odpowiedź serwera przy ustawianiu nicku: %s", nb);
            // pozwalamy przejść dalej, ale informujemy użytkownika
            break;
        }
    }

    // Chęć dołączenia do gry
    {
        const char joinCmd[] = "JOIN\n";
        if (write(sock, joinCmd, sizeof(joinCmd)-1) != (ssize_t)(sizeof(joinCmd)-1)) {
            errorExit("write");
        }
    }

    // Flaga sygnalizująca zakończenie głosowania przez serwer
    std::atomic<bool> ended{false};

    // Wątek nasłuchujący wiadomości z serwera
    std::thread recvThread([&]() {
        char rbuf[256];
        while (true) {
            int r = read(sock, rbuf, sizeof(rbuf)-1);
            if (r <= 0) {
                // socket zamknięty
                ended = true;
                break;
            }
            rbuf[r] = '\0';

            if (std::strncmp(rbuf, "GLOSOWANIE_ZAKONCZONE", 21) == 0) {
                std::printf("%s", rbuf);
                std::printf("Głosowanie zostało zakończone przez serwer.\n");
                ended = true;
                break;
            } else {
                // inne odpowiedzi (powitanie, OK, RESULT itp.)
                std::printf("Odpowiedź: %s", rbuf);
            }
        }
    });

    while (true) {
        if (ended.load()) break;
        std::printf("Wybierz opcję (1-4), '0' cofnij głos, 'r' wyniki, 'q' wyjście: ");
        std::fflush(stdout);
        char line[32];
        if (!std::fgets(line, sizeof(line), stdin)) break;

        if (line[0] == 'q') break;
        std::string cmd;
        if (line[0] == 'r' || line[0] == 'R') {
            cmd = "RESULT\n";
        } else if (line[0] >= '1' && line[0] <= '4') {
            cmd = "VOTE ";
            cmd.push_back(line[0]);
            cmd.push_back('\n');
        } else if (line[0] == '0') {
            cmd = "VOTE 0\n";
        } else {
            std::printf("Nieznana komenda.\n");
            continue;
        }

        if (write(sock, cmd.c_str(), cmd.size()) != (ssize_t)cmd.size()) {
            errorExit("write");
        }
    }

    ended = true;
    if (recvThread.joinable()) recvThread.join();
    close(sock);
    return 0;
}
