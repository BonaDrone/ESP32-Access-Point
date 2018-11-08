SUPERFLY_ADDR        = '192.168.4.1'
SUPERFLY_PORT        = 80
SUPERFLY_TIMEOUT_SEC = 4

from socket import socket


from msppg import serialize_RC_NORMAL_Request


# Initialize pygame for joystick support
'''
pygame.display.init()
pygame.joystick.init()
controller = pygame.joystick.Joystick(0)
controller.init()
# Get axis map for controller
controller_name = controller.get_name()
if not controller_name in axismaps.keys():
    print('Unrecognized controller: %s' % controller_name)
    exit(1)
'''

# Set up socket connection to SuperFly
sock = socket()
sock.settimeout(SUPERFLY_TIMEOUT_SEC)
sock.connect((SUPERFLY_ADDR, SUPERFLY_PORT))


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
print("of length: {} bytes".format(len(recv_data)))