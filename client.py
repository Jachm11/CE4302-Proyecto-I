import socket
import time

def receive_large_message(socket_obj, buffer_size=1024):
    data = b""
    while True:
        chunk = socket_obj.recv(buffer_size)
        if not chunk:
            break
        data += chunk
    return data

def main():
    server_ip = '127.0.0.1'  # Replace with the IP address of the server
    server_port = 4000  # Replace with the port number the server is listening on


    while(True):
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        client_socket.connect((server_ip, server_port))
        print("Connected to the server.")

        data = b'{"command": "start", "protocol: "moesi"}'
        client_socket.sendall(data)

        received_data = receive_large_message(client_socket)
        print(f"Received {len(received_data)} bytes of data:\n")
        print(received_data.decode('utf-8'))

        client_socket.close()

        time.sleep(1)


if __name__ == "__main__":
    main()