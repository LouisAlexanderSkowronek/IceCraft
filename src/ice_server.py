import socket
import threading

class Block:
    def __init__(self, x, y, z, material_id):
        self.x = x
        self.y = y
        self.z = z
        self.material_id = material_id
    
    def get_place_cmd(self) -> str:
        return f'place {self.x} {self.y} {self.z} {self.material_id}'

    def get_destroy_cmd(self) -> str:
        return f'destroy {self.x} {self.y} {self.z}'


mods: list[str] = [
    'place 5 4 -5 1',
    'place 6 4 -5 2',
    'place 7 4 -5 3',
    'place 8 4 -5 5',
    'place 6 10 -5 0'
]

def update_client(client_socket):
    mods_msg = '\n'.join(mods)
    client_socket.sendall(len(mods_msg).to_bytes(8, 'big'))
    client_socket.sendall(mods_msg.encode('utf-8'))
    

def handle_client(client_socket, client_address):
    """Handles a single client connection."""
    print(f'[INFO] Connected to {client_address}')
    try:
        update_client(client_socket)

        while True:
            message = client_socket.recv(1024).decode('utf-8')  # Receive message
            if not message:
                print(f'[INFO] Connection to {client_address} closed by client.')
                break

            print(f'[{client_address}] {message}')

            if message == 'update':
                update_client(client_socket)
            else:
                mods.append(message)
                client_socket.sendall(message.encode('utf-8'))  # Echo message back
                
    except Exception as e:
        print(f"[ERROR] Exception with client {client_address}: {e}")
    finally:
        client_socket.close()
        print(f"[INFO] Connection to {client_address} closed.")

def start_server(host='0.0.0.0', port=12345):
    """Starts the crash-safe multi-client echo server."""

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # Reuse address
    server_socket.bind((host, port))
    server_socket.listen(5)
    print(f"[INFO] Server started on {host}:{port}")
    
    try:
        while True:
            client_socket, client_address = server_socket.accept()
            client_thread = threading.Thread(target=handle_client, args=(client_socket, client_address), daemon=True)
            client_thread.start()
            print(f"[INFO] Client {client_address} connected. Total threads: {threading.active_count() - 1}")
    except KeyboardInterrupt:
        print("\n[INFO] Server shutting down.")
    except Exception as e:
        print(f"[ERROR] Unexpected error: {e}")
    finally:
        server_socket.close()
        print("[INFO] Server socket closed.")

if __name__ == "__main__":
    start_server()
