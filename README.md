# hangman
To compile the server program, run the following command:-

gcc -pthread -o server server.c connection.c reader.c game.c sys_ops.c

For client program:-

gcc -o client client.c
