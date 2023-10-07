# echo-client.py

import socket
import time;
HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 4000  # The port used by the server


while(True):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        data = b'{"command": "start", "protocol": "MESI"}'
        s.sendall(data)
        data = s.recv(10240000)
        f = open("demofile2.txt", "w")
        f.write(str(data))
        f.close()
        s.close()
        print("-------------------------------")
        time.sleep(2)

print(f"Received {data!r}")