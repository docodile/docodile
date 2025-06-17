#include "server.h"

#define PORT           6006
#define BUFFER_SIZE    4096 / 2
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
  } else if (ext && strcmp(ext, ".json") == 0) {
    mime_type = "application/json; charset=utf-8";
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
  BuildSiteDirectory(site_directory, DOCSDIR, 0);
  SortDirectory(site_directory);
  InitializeSite(HIDDENBUILDDIR);
  const char *index_file_path = HIDDENBUILDDIR "/assets/search.json";
  if (access(index_file_path, F_OK) != 0) {
    SearchIndex index = BuildSearchIndex(site_directory);
    WriteSearchIndex(index, index_file_path);
    FreeSearchIndex(index);
  }
  BuildSite(site_directory, site_directory, HIDDENBUILDDIR);
  FreeDirectory(site_directory);
}

void SigchildHandler(int s) {
  LOG_DEBUG("Handling SIGCHLD signal.");

  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) > 0);

  errno = saved_errno;
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
  char url[1000];
  sprintf(url, "http://localhost:%d", PORT);
  printf("Serving on %s\n", url);
  // OpenBrowser(url);

  struct sigaction sa;
  sa.sa_handler = SigchildHandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) < 0) {
    perror("Failed to set up sigaction");
    exit(1);
  }

  while (1) {
    int client_fd = accept(server_fd, NULL, NULL);
    pid_t pid     = fork();
    if (pid == 0) {
      close(server_fd);

      // TODO only build if change detected
      Build();
      char request[BUFFER_SIZE];
      read(client_fd, request, BUFFER_SIZE - 1);

      char method[8], path[1024];
      sscanf(request, "%s %s", method, path);

      if (strcmp(path, "/") == 0) strcpy(path, "/index.html");
      char full_path[1024];
      snprintf(full_path, sizeof(full_path), HIDDENBUILDDIR "/.%s", path);

      SendFile(client_fd, full_path);
      close(client_fd);
      exit(0);
    } else {
      close(client_fd);
    }
  }

  close(server_fd);
}