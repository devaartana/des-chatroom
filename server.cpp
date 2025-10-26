#include <arpa/inet.h>
#include <bits/stdc++.h>
using namespace std;

struct terminal {
  int socket;
  thread th;
};

unordered_map<int, terminal> clients;
long long int seed = 0;
mutex cout_mtx, clients_mtx;

void shared_print(string str, bool endLine = true) {
  lock_guard<mutex> guard(cout_mtx);
  cout << str;
  if (endLine) cout << endl;
}

void sendall(int sock, string text) {
  string length_str = to_string(text.length());
  send(sock, length_str.c_str(), length_str.length(), 0);
  send(sock, "\n", 1, 0);

  int bytes_sent = 0;
  while (bytes_sent < text.length()) {
    string string_to_send = text.substr(bytes_sent, text.length() - bytes_sent);
    int bytes = send(sock, string_to_send.c_str(), string_to_send.length(), 0);
    bytes_sent += bytes;
  }
}

bool recvall(int sock, string& result) {
  char buffer[1024] = {0};
  string length_str = "";

  while (true) {
    char c;
    int bytes = recv(sock, &c, 1, 0);

    if (bytes <= 0) {
      return false;
    }

    if (c == '\n') {
      break;
    }
    length_str += c;
  }

  int total_length = stoi(length_str);

  string received_data = "";
  int bytes_received = 0;

  while (bytes_received < total_length) {
    memset(buffer, 0, sizeof(buffer));
    int bytes = recv(sock, buffer, min(1024, total_length - bytes_received), 0);

    received_data.append(buffer, bytes);
    bytes_received += bytes;
  }

  result = received_data;
  return true;
}

void broadcast_message(string message, int sender_id) {
  lock_guard<mutex> guard(clients_mtx);
  for (auto& pair : clients) {
    if (pair.first != sender_id) {
      sendall(pair.second.socket, message);
    }
  }
}

void end_connection(int id) {
  lock_guard<mutex> guard(clients_mtx);
  clients[id].th.detach();
  close(clients[id].socket);
  clients.erase(id);
}

void handle_client(int client_socket, int id, sockaddr_in client_addr) {
  string str;

  string client_ip = inet_ntoa(client_addr.sin_addr);
  int client_port = ntohs(client_addr.sin_port);
  string client_info = "('" + client_ip + "', " + to_string(client_port) + ")";

  shared_print(client_info + " joined the chatroom");

  while (1) {
    int status = recvall(client_socket, str);

    if (!status) {
      shared_print(client_info + " has left");
      end_connection(id);
      return;
    }

    broadcast_message(str, id);
    shared_print(client_info + " sent " + str);
  }
}

int main() {
  int server_socket;
  if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket: ");
    return -1;
  }

  int opt = 1;
  if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
    perror("setsockopt");
  }

  sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(10000);
  server.sin_addr.s_addr = INADDR_ANY;

  if ((bind(server_socket, (sockaddr*)&server, sizeof(sockaddr_in))) == -1) {
    perror("bind error: ");
    return -1;
  }

  if ((listen(server_socket, 8)) == -1) {
    perror("listen error: ");
    return -1;
  }

  sockaddr_in client;
  int client_socket;
  unsigned int len = sizeof(sockaddr_in);

  cout << "SERVER RUNNING ON (" << inet_ntoa(server.sin_addr) << ", " << ntohs(server.sin_port) << ")" << endl;

  while (true) {
    if ((client_socket = accept(server_socket, (sockaddr*)&client, &len)) == -1) {
      perror("accept error: ");
      continue;
    }

    seed++;
    thread t(handle_client, client_socket, seed, client);
    lock_guard<mutex> guard(clients_mtx);
    clients[seed] = terminal{
      .socket = client_socket, 
      .th = move(t)
    };
  }

  for (auto& pair : clients) {
    if (pair.second.th.joinable()) {
      pair.second.th.join();
    }
  }

  close(server_socket);
  return 0;
}