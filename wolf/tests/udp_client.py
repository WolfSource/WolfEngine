import socket

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 8000        # The port used by the server

# Create a UDP socket at client side
udp = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
# Send to server using created UDP socket
udp.sendto(str.encode("Hello UDP Server"), (HOST, PORT))
msg = udp.recvfrom(1024)
print('Received', msg.decode("utf-8"))