#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <bits/stdc++.h>
using namespace std;


// ----------------------------------------------------------------------------------------------------------------
string permute(string key, int arr[], int len) {
  string permutation_key = "";
  for (int i = 0; i < len; i++) {
    permutation_key = permutation_key + key[arr[i] - 1]; // arr[i] tidak zero indexed (1..n)
  }
  return permutation_key;
}

string hex_to_bin(string s) {
  char str[16 + 5] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  string hex[16 + 5] = {"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"};
  string ret = "";
  for (int i = 0; i < s.length(); i++) {
    for (int j = 0; j < 16; j++) {
      if (s[i] == str[j]) {
        ret = ret + hex[j];
        break;
      }
    }
  }
  return ret;
}

string bin_to_hex(string s) {
  char str[16 + 5] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  string hex[16 + 5] = {"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"};
  string ret = "";
  for (int i = 0; i < s.length(); i += 4) {
    string four_bits = "";
    for (int j = i; j < i + 4; j++) {
      four_bits = four_bits + s[j];
    }
    for (int j = 0; j < 16; j++) {
      if (four_bits == hex[j]) {
        ret = ret + str[j];
        break;
      }
    }
  }
  return ret;
}

int bin_to_dec(string s) {
  int decimal = 0;
  int power = 0;
  for (int i = s.length() - 1; i >= 0; i--) {
    if (s[i] == '1') {
      decimal += pow(2, power);
    }
    power++;
  }
  return decimal;
}

string dec_to_bin(int n) {
  string ret = "";
  while (n != 0) {
    if (n % 2 == 0) {
      ret = ret + "0";
    } else {
      ret = ret + "1";
    }
    n /= 2;
  }
  while (ret.length() < 4) {
    ret = ret + "0";
  }
  reverse(ret.begin(), ret.end());
  return ret;
}

string xor_f(string a, string b) {
  string ret = "";
  for (int i = 0; i < a.length(); i++) {
    if (a[i] == b[i]) {
      ret = ret + "0";
    } else {
      ret = ret + "1";
    }
  }
  return ret;
}

string generate_key() {
  srand(time({}));
  int range_ascii[] = {48, 57, 65, 70};
  string key = "";
  for (int i = 0; i < 16; i++) {
    char c = rand() % (range_ascii[3] + 1 - range_ascii[0]) + range_ascii[0];
    if (c > range_ascii[1] && c < range_ascii[2]) {
      i--;
      continue;
    }
    key = key + c;
  }
  return key;
}

string parity_bit_drop_key(string key) {
  int parity_table[] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4
  };
  return (permute(key, parity_table, 56));
}

string compression_56_to_48(string key) {
  int compression_table[] = {
      14, 17, 11, 24, 1, 5,
			3, 28, 15, 6, 21, 10,
			23, 19, 12, 4, 26, 8,
			16, 7, 27, 20, 13, 2,
			41, 52, 31, 37, 47, 55,
			30, 40, 51, 45, 33, 48,
			44, 49, 39, 56, 34, 53,
			46, 42, 50, 36, 29, 32
  };
  return (permute(key, compression_table, 48));
}

void split_string(string str, string &left, string &right) {
  for (int i = 0; i < str.length() / 2; i++) {
    left = left + str[i];
    right = right + str[str.length() / 2 + i];
  }
}

string shift_left(string key, int n_shifts) {
  int shift_table[] = {
    1, 1, 2, 2,
    2, 2, 2, 2,
    1, 2, 2, 2,
    2, 2, 2, 1
  };

  string shifted_key = "";
  for (int i = 0; i < shift_table[n_shifts]; i++) {
    for (int j = 1; j < key.length(); j++) {
      shifted_key = shifted_key + key[j];
    }
    shifted_key = shifted_key + key[0];
    key = shifted_key;
    shifted_key = "";
  }
  return key;
}

struct round_keys {
  string random_hex;
  string round_keys_bin[16 + 5];
  string round_keys_hex[16 + 5];
};

round_keys generate_round_keys(string option, string key = "") {
  round_keys ret;
  if (option == "encryption_scheme") {
    ret.random_hex = "AABB09182736CCDD";
  } else {
    ret.random_hex = key;
  }
  string key_bin = hex_to_bin(ret.random_hex); 
  string key_bin_56 = parity_bit_drop_key(key_bin);
  
  string key_left;
  string key_right;
  split_string(key_bin_56, key_left, key_right);

  for (int i = 0; i < 16; i++) {
    key_left = shift_left(key_left, i);
    key_right = shift_left(key_right, i);
    string combined_key = key_left + key_right;
    string round_key = compression_56_to_48(combined_key);
    ret.round_keys_bin[i] = round_key;
    ret.round_keys_hex[i] = bin_to_hex(round_key);
  }
  return ret;
}

string initial_permutation(string text) {
  int IP_table[] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
  };
  return (permute(text, IP_table, 64));
}

string expansion(string text) {
  int expansion_table[] = {
    32, 1, 2, 3, 4, 5, 4, 5,
    6, 7, 8, 9, 8, 9, 10, 11,
    12, 13, 12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21, 20, 21,
    22, 23, 24, 25, 24, 25, 26, 27,
    28, 29, 28, 29, 30, 31, 32, 1
  };
  return (permute(text, expansion_table, 48));
}

string sbox_operation(string text) {
  int sbox[8 + 5][4 + 5][16 + 5] = {
    {
      {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
      {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
      {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
      {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
    },

    {
      {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
      {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
      {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
      {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
    },

    {
      {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
      {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
      {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
      {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
    },

    {
      {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
      {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
      {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
      {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
    },

    {
      {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
      {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
      {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
      {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
    },

    {
      {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
      {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
      {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
      {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
    },

    {
      {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
      {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
      {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
      {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
    },

    {
      {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
      {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
      {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
      {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
    }
  };
  
  string ret = "";
  for (int i = 0; i < 8; i++) {
    string outer = "";
    string inner = "";
    outer = outer + text[i * 6] + text[i * 6 + 5];
    inner = inner + text[i * 6 + 1] + text[i * 6 + 2] + text[i * 6 + 3] + text[i * 6 + 4];
    int val = sbox[i][bin_to_dec(outer)][bin_to_dec(inner)];
    ret = ret + dec_to_bin(val);
  }
  return ret;
}

string per(string text) {
  int per[] = {
    16, 7, 20, 21,
    29, 12, 28, 17,
    1, 15, 23, 26,
    5, 18, 31, 10,
    2, 8, 24, 14,
    32, 27, 3, 9,
    19, 13, 30, 6,
    22, 11, 4, 25
  };
  return permute(text, per, 32);
}

string final_permutation(string text) {
  int final_perm[] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25
  };
  return permute(text, final_perm, 64);
}

string DES_encrypt(string plain_text, round_keys key) {
  plain_text = hex_to_bin(plain_text);
  plain_text = initial_permutation(plain_text);

  string cipher_left;
  string cipher_right;

  // THE FUNCTION
  split_string(plain_text, cipher_left, cipher_right);
  
  for (int i = 0; i < 16; i++) {
    string cipher_right_expanded = expansion(cipher_right);

    string first_xor = xor_f(cipher_right_expanded, key.round_keys_bin[i]);

    string sbox_str = sbox_operation(first_xor);
    sbox_str = per(sbox_str);
    string result = xor_f(cipher_left, sbox_str);
    
    cipher_left = result;

    if (i != 15) {
      swap(cipher_left, cipher_right);
    }
  }
  string cipher_text = cipher_left + cipher_right;
  cipher_text = final_permutation(cipher_text);

  return bin_to_hex(cipher_text);
}

string DES_decrypt(string cipher_text, string key_hex) {
  round_keys key = generate_round_keys("decryption_scheme", key_hex);
  round_keys reversed_key;
  for (int i = 15; i >= 0; i--) {
    reversed_key.round_keys_bin[15 - i] = key.round_keys_bin[i];
    reversed_key.round_keys_hex[15 - i] = key.round_keys_hex[i];
  }
  return DES_encrypt(cipher_text, reversed_key);
}

vector<string> input_encrypt() { 
  // This function is used for converting literal string into hexadecimal
  string input_text;
  getline(cin, input_text);
  vector<string> plain_text(1);

  int k = 0;
  for (int i = 0; i < input_text.length(); i++) {
    string character = "";
    for(int j = 7; j >= 0; j--) {
      bool is_set = input_text[i] & (1 << j);
      if (is_set == 1) {
        character = character + "1";
      } else {
        character = character + "0";
      }
    }
    string character_left, character_right;
    split_string(character, character_left, character_right);
    if (plain_text[k].length() == 16) {
      k++;
      plain_text.push_back("" );
    }
    plain_text[k] = plain_text[k] + bin_to_hex(character_left);
    plain_text[k] = plain_text[k] + bin_to_hex(character_right);
  }

  if (plain_text[k].length() != 16) {
    for (int i = plain_text[k].length(); i < 16; i++) {
      plain_text[k] = plain_text[k] + '0';
    }
  }
  return plain_text;
}

string output_decrypt(vector<string> plain_text_hex) {
  string ret = "";
  for (int i = 0; i < plain_text_hex.size(); i++) {
    for (int j = 0; j < 16; j += 2) {
      string two_hex = plain_text_hex[i].substr(j, 2);
      int value = stoi(two_hex, 0, 16);
      if (value != 0) {
        ret += (char)value;
      }
    }
  }
  return ret;
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

string recvall(int sock) {
  char buffer[1024] = {0};
  string length_str = "";
  
  while (true) {
    char c;
    int bytes = recv(sock, &c, 1, 0);
    if (bytes <= 0 || c == '\n') break;
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
  
  return received_data;
}

int connect_to_server(const char* server_ip, int port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    cerr << "Gagal membuat socket" << '\n';
    close(sock);
    return -1;
  }

  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
    cerr << "Alamat IP tidak valid" << '\n';
    close(sock);
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    cerr << "Koneksi ke server gagal" << '\n';
    close(sock);
    return -1;
  }
  
  cout << "Berhasil terhubung ke server " << server_ip << ":" << port << '\n';
  return sock;
}

void receive_messages(int sock, round_keys key) {
  while (true) {
    string cipher_text = recvall(sock);
    
    if (cipher_text.empty()) {
      cout << "\nKoneksi server ditutup" << '\n';
      break;
    }

    vector<string> plain_text_hex;

    for (int i = 0; i < cipher_text.length(); i += 16) {
      string cipher_text_partition = cipher_text.substr(i, 16);
      plain_text_hex.push_back(DES_decrypt(cipher_text_partition, key.random_hex));
    }
    string plain_text = output_decrypt(plain_text_hex);
    
    cout << "\33[2K\r";
    cout << "[Anonim]: " << plain_text << '\n';
    cout << "[You]: " << flush;
  }
}


void send_messages(int sock, round_keys key) {
  while (true) {
    cout << "[You]: ";
    vector<string> plain_text = input_encrypt();
    const int message_size = plain_text.size();
    string cipher_text = "";

    for (int i = 0; i < message_size; i++) {
      cipher_text = cipher_text + (DES_encrypt(plain_text[i], key));
    }

    sendall(sock, cipher_text);
  }
}

int main() {
  string server_ip;
  cout << "Masukkan server IP" << '\n';
  cout << "[You]: ";
  cin >> server_ip;
  cin.ignore();
  int sock = connect_to_server(server_ip.c_str(), 10000);
  round_keys key = generate_round_keys("encryption_scheme");
  
  thread receiver_thread(receive_messages, sock, key);
  receiver_thread.detach();
  send_messages(sock, key);

  close(sock);
  return 0;
}