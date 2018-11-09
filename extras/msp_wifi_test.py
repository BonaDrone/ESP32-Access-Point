ADDR        = '192.168.4.1'
PORT        = 80
TIMEOUT_SEC = 4

from socket import socket
from msppg import serialize_RC_NORMAL_Request

# Set up socket connection to SuperFly
sock = socket()
sock.settimeout(TIMEOUT_SEC)
sock.connect((ADDR, PORT))

data = serialize_RC_NORMAL_Request()
print("Data to send: {}".format(data))
sock.send(data)

recv_data = ""
while True:
	try:
		recv_data += sock.recv(1).decode()
	except:
		break

sock.close()

print("Received message: {}".format(str(recv_data)))
print("of length: {} bytes".format(len(recv_data))
