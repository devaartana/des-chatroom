# Data Encryption Standard

| Name           | NRP        | Kelas     |
| ---            | ---        | ----------|
| Randi Palguna Artayasa | 5025231020 | Keamanan Informasi (B) |
| I Kadek Arya Deva Artana  | 5025231157 | Keamanan Informasi (B) |

## Encryption Method
![DES output](./resource/image/DES-Encryption%20Method.svg)

## Decryption Method
To decrypt the ciphertext, reverse the order of the 16 round keys and run the encryption algorithm using those keys.

# Chat Room Diagram
![Chat room diagram](./resource/image/Chat-Room-Diagram.svg)

# How To Run

1. Clone the repository

```sh
git clone https://github.com/devaartana/des-chatroom.git
cd des-chatroom
```

2. Compile to binary file

```sh
g++ server.cpp -o server
g++ client.cpp -o client
```

3. Run the Chatroom

    You must run the server first, before any clients can connect.    
    
    Run the server in your terminal
    ```
    ./server
    ```

    Run the client in new terminal window 
    ```
    ./client
    ```
    (You can repeat this step in multiple terminals to have many clients connect to the chatroom)
    

# Reference

https://github.com/cjchirag7/chatroom-cpp#
