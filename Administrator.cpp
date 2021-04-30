#include "Administrator.h"
#include "Controller.h"
int Administrator::sock;
fd_set Administrator::active_set;
queue<pair<int, string>> Administrator::responses;

int Administrator::prepare () {
    int opt = 1;
    int err;
    struct sockaddr_in addr{};
    // Creating TCP socket for getting connection requests
    sock = socket (PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }
    setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof (opt));
    
    // Заполняем адресную структуру и
    // связываем сокет с любым адресом
    addr.sin_family = AF_INET;
    addr.sin_port = htons (PORT);
    addr.sin_addr.s_addr = htonl (INADDR_ANY);
    err = bind (sock, (struct sockaddr *) &addr, sizeof (addr));
    if (err < 0) {
        return -2;
    }
    
    // Создаем очередь на 3 входящих запроса соединения
    err = listen (sock, 100);
    if (err < 0) {
        return -3;
    }
    
    return 0;
}

int Administrator::run () {
    
    fd_set read_set;
    int new_sock;
    int err;
    socklen_t size;
    sockaddr_in client{};
    char buf[BUFLEN];
    // Подготавливаем множества дескрипторов каналов ввода-вывода.
    // Для простоты не вычисляем максимальное значение дескриптора,
    // а далее будем проверять все дескрипторы вплоть до максимально
    // возможного значения FD_SETSIZE.
    FD_ZERO(&active_set);
    FD_SET(sock, &active_set);
    thread responser (answer);
    responser.detach ();
    // Основной бесконечный цикл проверки состояния сокетов
    while (true) {
        // Проверим, не появились ли данные в каком-либо сокете.
        // В нашем варианте ждем до фактического появления данных.
        read_set = active_set;
        if (select (FD_SETSIZE, &read_set, nullptr, nullptr, nullptr) < 0) {
            perror ("Server: select  failure");
            exit (EXIT_FAILURE);
        }
        
        // Данные появились. Проверим в каком сокете.
        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &read_set)) {
                if (i == sock) {
                    // пришел запрос на новое соединение
                    size = sizeof (client);
                    new_sock = accept (sock, (struct sockaddr *) &client, &size);
                    if (new_sock < 0) {
                        perror ("accept");
                        exit (EXIT_FAILURE);
                    }
                    fprintf (stdout, "Server: connect from host %s, port %hu.\n",
                             inet_ntoa (client.sin_addr),
                             ntohs (client.sin_port));
                    FD_SET(new_sock, &active_set);
                    // socket_id.emplace_back (new_sock, );
                } else {
                    // пришли данные в уже существующем соединени
                    err = get_from_client (i, buf);
                    if (err < 0) {
                        // ошибка или конец данных
                        /*close (i);
                        FD_CLR(i, &active_set);*/
                    }
                    Controller::add_next (i, string (buf));
                    /*else {
                        // а если это команда закончить работу?
                        if (strstr (buf, "stop")) {
                            // this should be moved to Controller and called by Administrator::disconnect
                        } else {
                            // данные прочитаны нормально
                            send_to_client (i, buf);
                        }
                    }*/
                }
            }
        }
    }
}

bool Administrator::get_from_client (int fd, char *buf) {
    int nbytes;
    nbytes = read (fd, buf, BUFLEN);
    if (nbytes < 0) {
        perror ("Server: read failure");
        return false;
    } else if (nbytes == 0) {
        return false;
    } else {
        //Controller::do_next (fd, string (to_string (sock) + ' ' + buf));
        //fprintf (stdout, "Server gets message: %s\n", buf);
        // there possibly can be smth useful
        return true;
    }
}

bool Administrator::send_to_client (int fd, char *buf) {
    int nbytes;
    unsigned char *s;
    // ответ переводим в верхний регистр
    for (s = (unsigned char *) buf; *s; s++) *s = toupper (*s);
    // отправляем клиенту
    nbytes = write (fd, buf, strlen (buf) + 1);
    fprintf (stdout, "Write back: %s\nnbytes=%d\n", buf, nbytes);
    if (nbytes < 0) {
        perror ("Server: write failure");
        return false;
    }
    return true;
}

void Administrator::disconnect (int fd) {
    // closes connection for client fd
    close (fd);
    FD_CLR (fd, &active_set);
}

void Administrator::answer () {
    //here should be an infinite loop checking response queue and sending it
    while (Controller::working ()) {
        if (!responses.empty ()) {
            char a[BUFLEN];
            strcpy (a, responses.front ().second.c_str ());
            send_to_client (responses.front ().first, a);
            responses.pop ();
        }
    }
    sleep (1); // letting other processes to be finished
    // here should be accurate finishing the work
}

void Administrator::add_response (int fd, const string &result) {
    // adds new response to queue
    responses.push (pair<int, string> (fd, result));
}
