#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#define PORT 8080
#define MAX_CLIENTS 1000
#define COUNT_CACHE_CELLS 256
#define MAX_SIZE_REQUEST 1048576
#define MAX_SIZE_RESPONSE 1048576
#define MAX_URL 2048

typedef struct headers_struct {
    struct headers_struct *next;
    char *header;
} headers_t;

typedef struct cache_data_struct {
    struct cache_data_struct *next;
    char data[MAX_SIZE_RESPONSE];
    size_t size_data;
} cache_data_t;

typedef struct cache_struct {
    char url[MAX_URL];
    headers_t *headers;
    pthread_cond_t cond;
    cache_data_t *data;
    char is_fulled;
} cache_t;

typedef struct cache_cell_struct {
    pthread_mutex_t lock;
    cache_t cache;
} cache_cell_t;

typedef struct request_struct {
    char request[MAX_SIZE_REQUEST];
    size_t size_request;
    pthread_mutex_t *lock;
    int socket;
    cache_t *cache;
} request_t;

int flag_is_working = 1;
cache_cell_t cache_cells[COUNT_CACHE_CELLS];
int server_socket;

unsigned hash_url(const char *url) {
    unsigned h = 5381;
    while (*url) {
        h = (h * 33) ^ (unsigned char) *url++;
    }
    return h % COUNT_CACHE_CELLS;
}

headers_t *parse_headers(char *request, size_t *index_start_body) {
    headers_t *headers_head = malloc(sizeof(headers_t));
    headers_head->header = malloc(1);
    *headers_head->header = 0;
    headers_t *headers = headers_head;
    char *newline = memchr(request, '\n', MAX_SIZE_REQUEST);
    char *last_line = newline + 1;
    size_t size = MAX_SIZE_REQUEST - (last_line - request);

    while (newline != NULL) {
        newline = memchr(last_line, '\n', size);
        if (newline) {
            size_t line_len = newline - last_line + 1;
            size -= line_len;

            if (line_len == 2 && last_line[0] == '\r' || line_len <= 1) {
                break;
            }

            if (strncasecmp(last_line, "Host:", 5) == 0 ||
                strncasecmp(last_line, "Accept:", 7) == 0 ||
                strncasecmp(last_line, "Accept-Language:", 16) == 0 ||
                strncasecmp(last_line, "Accept-Encoding:", 16) == 0 ||
                strncasecmp(last_line, "If-Modified-Since:", 18) == 0 ||
                strncasecmp(last_line, "If-None-Match:", 14) == 0 ||
                strncasecmp(last_line, "User-Agent:", 11) == 0 ||
                strncasecmp(last_line, "Range:", 6) == 0 ||
                strncasecmp(last_line, "Cookie:", 7) == 0 ||
                strncasecmp(last_line, "Authorization:", 14) == 0 ||
                strncasecmp(last_line, "Cache-Control:", 14) == 0) {

                *newline = 0;
                headers->next = malloc(sizeof(headers_t));
                headers = headers->next;
                headers->next = NULL;
                headers->header = malloc(line_len);
                strcpy(headers->header, last_line);
                *newline = '\n';
            }

            last_line = newline + 1;
        }
    }
    if (newline) {
        *index_start_body = newline - request + 1;
    } else {
        *index_start_body = -1;
    }
    return headers_head;
}

int resolve_hostname(const char *hostname, char *ip, size_t ip_len) {
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        perror("getaddrinfo");
        return -1;
    }

    if (res == NULL) {
        return -1;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        struct sockaddr_in *addr = (struct sockaddr_in *) p->ai_addr;
        inet_ntop(AF_INET, &addr->sin_addr, ip, ip_len);
        break;
    }

    freeaddrinfo(res);
    return 0;
}

void send_http_error(int client_fd, int code, const char *message) {
    char buffer[512];

    int len = snprintf(
            buffer, sizeof(buffer),
            "HTTP/1.0 %d %s\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %zu\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            code, message,
            strlen(message),
            message
    );

    write(client_fd, buffer, len);
}

int connect_to_host(int client_fd, char *host, int port) {
    int socket_host = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_host < 0) {
        return -1;
    }

    struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_port = htons(port)
    };
    if (!inet_aton(host, &addr.sin_addr)) {
        size_t ip_len = 16;
        char ip[ip_len];
        if (resolve_hostname(host, ip, ip_len) != 0) {
            send_http_error(client_fd, 502, "Bad Gateway");
            return -1;
        }
        if (!inet_aton(ip, &addr.sin_addr)) {
            perror("resolve dns");
            send_http_error(client_fd, 502, "Bad Gateway");
            return -1;
        }
    }

    if (connect(socket_host, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        send_http_error(client_fd, 502, "Bad Gateway");
        return -1;
    }

    return socket_host;
}

int check_cache(cache_t *cache, char *url, headers_t *headers) {
    if (strcmp(cache->url, url) != 0) {
        return 0;
    }
    while (headers != NULL) {
        headers_t *headers1 = cache->headers;
        while (headers1 != NULL) {
            if (strcmp(headers->header, headers1->header) == 0) {
                break;
            }
            headers1 = headers1->next;
        }
        if (headers1 == NULL) {
            return 0;
        }
        headers = headers->next;
    }
    return 1;
}

size_t read_http_headers(int fd, char *buf, size_t maxlen) {
    size_t total = 0;
    int state = 0;
    char c;

    while (total < maxlen - 1) {
        size_t n = read(fd, &c, 1);
        if (n == 0) {
            break;
        }
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }

        buf[total++] = c;

        switch (state) {
            case 0:
                state = (c == '\r') ? 1 : 0;
                break;
            case 1:
                state = (c == '\n') ? 2 : 0;
                break;
            case 2:
                state = (c == '\r') ? 3 : 0;
                break;
            case 3:
                if (c == '\n') {
                    buf[total] = '\0';
                    return total;
                }
                state = 0;
                break;
        }
    }

    buf[total] = '\0';
    return total;
}

size_t get_content_length(const char *headers, size_t *len) {
    const char *p = headers;

    while (*p != 0) {
        const char *line_end = strstr(p, "\r\n");
        if (!line_end) {
            break;
        }

        size_t line_len = line_end - p;

        const char *key = "Content-Length:";
        size_t key_len = strlen(key);

        if (line_len > key_len &&
            strncasecmp(p, "Content-Length:", 15) == 0) {

            const char *v = p + key_len;
            while (*v == ' ' || *v == '\t') {
                v++;
            }

            char *endptr;
            size_t val = strtoull(v, &endptr, 10);

            if (endptr == v) {
                return -1;
            }

            *len = (size_t) val;
            printf("LENGTH %zu\n\n\n", val);
            return 0;
        }

        p = line_end + 2;
    }

    return -1;
}

void *request_data(void *arg) {
    request_t *struct_request = (request_t *) arg;

    pthread_mutex_lock(struct_request->lock);

    int socket_host = struct_request->socket;
    char *new_request = struct_request->request;
    size_t size_request = struct_request->size_request;
    cache_t *cache = struct_request->cache;

    printf("Send request:\n%s\n\n", new_request);

    int size_write = write(socket_host, new_request, size_request);
    if (size_write < size_request) {
        perror("write");
        cache->is_fulled = 2;
        pthread_mutex_unlock(struct_request->lock);
        pthread_cond_broadcast(&cache->cond);
        return NULL;
    }

    cache->data = malloc(sizeof(cache_data_t));
    cache->data->next = NULL;
    cache->data->size_data = 0;
    cache_data_t *data = cache->data;

    data->size_data = read_http_headers(socket_host, data->data, MAX_SIZE_RESPONSE);
    if (data->size_data < 0) {
        cache->is_fulled = 2;

        pthread_mutex_unlock(struct_request->lock);
        pthread_cond_broadcast(&cache->cond);
        return NULL;
    }

    printf("Receive response:\n%s\n\n", data->data);

    size_t content_length;
    if (get_content_length(data->data, &content_length)) {
        content_length = 0xFFFFFFFFFFFFFFFF;
    }

    printf("LENGTH AFTER %zu\n\n\n", content_length);
    data->next = malloc(sizeof(cache_data_t));
    data = data->next;

    size_t x = 0;
    while (x < content_length && (data->size_data = read(socket_host, data->data, MAX_SIZE_RESPONSE)) > 0) {
        x += data->size_data;
        printf("Receive response:\n\ncurrent_size = %zu, content_length = %zu\n\n", x, content_length);
        data->next = malloc(sizeof(cache_data_t));
        data = data->next;
    }

    cache->is_fulled = 1;

    pthread_mutex_unlock(struct_request->lock);
    pthread_cond_broadcast(&cache->cond);
    return NULL;
}

headers_t *create_copy_headers(headers_t *headers) {
    if (headers == NULL) {
        return NULL;
    }
    headers_t *headers_head = malloc(sizeof(headers_t));
    headers_head->next = NULL;
    headers_head->header = malloc(strlen(headers->header) + 1);

    strcpy(headers_head->header, headers->header);
    headers_t *header = headers->next;
    headers_t *header1 = headers_head;
    while (header != NULL) {
        header1->next = malloc(sizeof(headers_t));
        header1 = header1->next;
        header1->next = NULL;
        header1->header = malloc(strlen(header->header) + 1);
        strcpy(header1->header, header->header);
        header = header->next;
    }
    return headers_head;
}

void method_get(int client_fd, int socket_host, char *url, char *method, headers_t *headers, char *request,
                size_t size_request, char *new_request) {
    int hash = hash_url(url);
    pthread_mutex_lock(&cache_cells[hash].lock);
    cache_t *cache = &cache_cells[hash].cache;

    do {
        if (!check_cache(cache, url, headers)) {
            strcpy(cache->url, url);
            headers_t *headers_copy = cache->headers;
            while (headers_copy != NULL) {
                headers_t *next = headers_copy->next;
                free(headers_copy->header);
                free(headers_copy);
                headers_copy = next;
            }
            cache->headers = create_copy_headers(headers);

            cache->is_fulled = 0;

            cache_data_t *data = cache->data;
            while (data != NULL) {
                cache_data_t *data_prev = data;
                data = data->next;
                free(data_prev);
            }
            cache->data = NULL;

            request_t struct_request = {
                    .lock = &cache_cells[hash].lock,
                    .socket = socket_host,
                    .size_request = size_request,
                    .cache = cache
            };
            memcpy(struct_request.request, new_request, size_request);

            pthread_t thread;
            if (pthread_create(&thread, NULL, request_data, &struct_request)) {
                perror("pthread_create");
                send_http_error(client_fd, 500, "Internal Server Error");

                pthread_mutex_unlock(&cache_cells[hash].lock);
                return;
            }
            pthread_detach(thread);
        }

        while (!cache->is_fulled) {
            pthread_cond_wait(&cache->cond, &cache_cells[hash].lock);
        }
        if (cache->is_fulled == 2) {
            printf("error\n");
            send_http_error(client_fd, 500, "Internal Server Error");
            memset(cache->url, 0, MAX_URL);
            headers_t *headers_copy = cache->headers;
            while (headers_copy != NULL) {
                headers_t *next = headers_copy->next;
                free(headers_copy->header);
                free(headers_copy);
                headers_copy = next;
            }
            cache->headers = NULL;
            pthread_mutex_unlock(&cache_cells[hash].lock);
            return;
        }
    } while (!check_cache(cache, url, headers));

    printf("start send data\n");

    cache_data_t *data = cache->data;
    while (data != NULL) {
        size_t size_write = write(client_fd, data->data, data->size_data);
        if (size_write < data->size_data) {
            perror("write");
            send_http_error(client_fd, 500, "Internal Server Error");

            pthread_mutex_unlock(&cache_cells[hash].lock);
            return;
        }
        data = data->next;
    }

    pthread_mutex_unlock(&cache_cells[hash].lock);
}

void another_method(int client_fd, int socket_host, char *url, char *method, char *request, size_t size_request,
                    char *new_request) {
    size_t content_length;
    if (get_content_length(request, &content_length)) {
        content_length = 0xFFFFFFFFFFFFFFFF;
    }
    size_t x = 0;

    int size_write = write(socket_host, new_request, size_request);
    if (size_write < size_request) {
        perror("write");
        send_http_error(client_fd, 500, "Internal Server Error");
        return;
    }
    while (x < content_length && (size_request = read(client_fd, request, MAX_SIZE_REQUEST)) > 0) {
        x += size_write;
        size_write = write(socket_host, request, size_request);
        if (size_write < size_request) {
            perror("write");
            send_http_error(client_fd, 500, "Internal Server Error");
            return;
        }
    }

    int size_response;
    char response[MAX_SIZE_RESPONSE];

    size_response = read_http_headers(socket_host, response, MAX_SIZE_RESPONSE);
    if (size_response < 0) {
        perror("read");
        send_http_error(client_fd, 500, "Internal Server Error");
        return;
    }

    if (get_content_length(response, &content_length)) {
        content_length = 0xFFFFFFFFFFFFFFFF;
    }
    x = 0;
    while (x < content_length && (size_response = read(socket_host, response, MAX_SIZE_RESPONSE)) > 0) {
        x += size_response;

        size_write = write(client_fd, response, size_response);
        if (size_write < size_response) {
            perror("write");
            send_http_error(client_fd, 500, "Internal Server Error");
            return;
        }
    }
}

void *client_handler(void *arg) {
    int client_fd = (int) (intptr_t) arg;
    char request[MAX_SIZE_REQUEST + 1];

    size_t size_request = read_http_headers(client_fd, request, MAX_SIZE_REQUEST);
    if (size_request <= 0) {
        close(client_fd);
        return NULL;
    }
    request[size_request] = 0;

    printf("Receive request:\n%s\n\n", request);

    char method[8];
    char url[MAX_URL];
    char version[9];
    if (sscanf(request, "%7s %2047s %8s", method, url, version) != 3) {
        send_http_error(client_fd, 400, "Bad Request");
        close(client_fd);
        return NULL;
    }

    if (strcmp(version, "HTTP/1.0") != 0) {
        send_http_error(client_fd, 505, "HTTP Version Not Supported");
        close(client_fd);
        return NULL;
    }

    char host[MAX_URL / 2], path[MAX_URL / 2];
    int port = 80;

    if (strncmp(url, "http://", 7) != 0) {
        send_http_error(client_fd, 400, "Bad Request");
        close(client_fd);
        return NULL;
    }

    const char *pointer_url = url + 7;
    const char *slash = strchr(pointer_url, '/');
    if (slash == NULL) {
        strcpy(path, "/");
        slash = pointer_url + strlen(pointer_url);
    } else {
        snprintf(path, sizeof(path), "%s", slash);
    }

    const char *colon = memchr(pointer_url, ':', slash - pointer_url);
    if (colon != NULL) {
        snprintf(host, sizeof(host), "%.*s", (int) (colon - pointer_url), pointer_url);
        port = atoi(colon + 1);
    } else {
        snprintf(host, sizeof(host), "%.*s", (int) (slash - pointer_url), pointer_url);
    }

    size_t index_start_body;
    headers_t *headers = parse_headers(request, &index_start_body);

    if (index_start_body == -1) {
        send_http_error(client_fd, 400, "Bad Request");
        close(client_fd);
        return NULL;
    }

    int socket_host = connect_to_host(client_fd, host, port);
    if (socket_host == -1) {
        close(socket_host);
        close(client_fd);

        return NULL;
    }

    char *new_request = request + (slash - url);
    sprintf(new_request, "%s", method);
    new_request[strlen(method)] = ' ';

    size_request -= (slash - url);

    if (strcmp(method, "GET") == 0) {
        method_get(client_fd, socket_host, url, method, headers, request, size_request, new_request);
    } else {
        another_method(client_fd, socket_host, url, method, request, size_request, new_request);
    }

    while (headers != NULL) {
        headers_t *next = headers->next;
        free(headers->header);
        free(headers);
        headers = next;
    }

    close(socket_host);
    close(client_fd);

    return NULL;
}

void init_cache() {
    for (int i = 0; i < COUNT_CACHE_CELLS; i++) {
        if (pthread_mutex_init(&cache_cells[i].lock, NULL)) {
            perror("pthread_mutex_init");
            exit(EXIT_FAILURE);
        }
        if (pthread_cond_init(&cache_cells[i].cache.cond, NULL)) {
            perror("pthread_cond_init");
            exit(EXIT_FAILURE);
        }
        cache_cells[i].cache.data = NULL;
        cache_cells[i].cache.headers = NULL;
    }
}

void free_cache() {
    for (int i = 0; i < COUNT_CACHE_CELLS; i++) {
        cache_cells[i].cache.is_fulled = 2;
        pthread_cond_broadcast(&cache_cells[i].cache.cond);
    }
    sleep(1);
    for (int i = 0; i < COUNT_CACHE_CELLS; i++) {
        cache_cells[i].cache.is_fulled = 2;
        pthread_cond_destroy(&cache_cells[i].cache.cond);
        pthread_mutex_destroy(&cache_cells[i].lock);

        headers_t *headers = cache_cells[i].cache.headers;
        while (headers != NULL) {
            headers_t *next = headers->next;
            free(headers->header);
            free(headers);
            headers = next;
        }

        cache_data_t *data = cache_cells[i].cache.data;
        while (data != NULL) {
            cache_data_t *data_prev = data;
            data = data->next;
            free(data_prev);
        }
    }
}

void handle_sigint(int sig) {
    flag_is_working = 0;
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_port = htons(PORT),
            .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(server_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Proxy on port %d\n", PORT);

    while (flag_is_working) {
        int client_fd = accept(server_socket, NULL, NULL);
        if (client_fd < 0) {
            flag_is_working = 0;
            break;
        }
        if (!flag_is_working) {
            close(client_fd);
            break;
        }
        pthread_t thread;
        pthread_create(&thread, NULL, client_handler, (void *) (intptr_t) client_fd);
        pthread_detach(thread);
    }
    free_cache();
    close(server_socket);
}
