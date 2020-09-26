import socket

cli = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

cli.connect(("127.0.0.1", 8080))

try:
    while True:
        cli.send(input("msg: ").encode())
except:
    cli.close()


