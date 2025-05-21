#include "server.h"

#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builder.h"

#define PORT           6006
#define BUFFER_SIZE    4096
#define HIDDENBUILDDIR ".site"

void Send404(int client_fd) {
  const char *response =
      "HTTP/1.1 404 Not Found\r\n"
      "Content-Length: 13\r\n"
      "Content-Type: text/plain\r\n"
      "\r\n"
      "404 Not Found";
  write(client_fd, response, strlen(response));
}

static void SendFile(int client_fd, const char *path) {
  int file_fd = open(path, O_RDONLY);
  if (file_fd == -1) {
    Send404(client_fd);
    return;
  }

  struct stat st;
  fstat(file_fd, &st);

  const char *mime_type = "text/html";
  const char *ext       = strrchr(path, '.');
  if (ext && strcmp(ext, ".css") == 0) {
    mime_type = "text/css";
  }

  char header[256];
  snprintf(header, sizeof(header),
           "HTTP/1.1 200 OK\r\n"
           "Content-Length: %ld\r\n"
           "Content-Type: %s\r\n"
           "\r\n",
           st.st_size, mime_type);
  write(client_fd, header, strlen(header));

  char buffer[BUFFER_SIZE];
  ssize_t bytes;
  while ((bytes = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
    write(client_fd, buffer, bytes);
  }

  close(file_fd);
}

static void Build() {
  Directory *site_directory = NewDirectory("");
  BuildSiteDirectory(site_directory, DOCSDIR);
  InitializeSite(HIDDENBUILDDIR);
  BuildSite(site_directory, HIDDENBUILDDIR);
  FreeDirectory(site_directory);
}

void Serve(const char *dir) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  int opt       = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  struct sockaddr_in addr = {.sin_family      = AF_INET,
                             .sin_port        = htons(PORT),
                             .sin_addr.s_addr = INADDR_ANY};

  bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
  listen(server_fd, 10);
  printf("Serving on http://localhost:%d\n", PORT);

  while (1) {
    int client_fd = accept(server_fd, NULL, NULL);
    // TODO only build if change detected
    Build();
    char request[BUFFER_SIZE];
    read(client_fd, request, BUFFER_SIZE - 1);

    char method[8], path[1024];
    sscanf(request, "%s %s", method, path);

    if (strcmp(path, "/") == 0) strcpy(path, "/index.html");
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), HIDDENBUILDDIR"/.%s", path);

    SendFile(client_fd, full_path);
    close(client_fd);
  }

  close(server_fd);
}