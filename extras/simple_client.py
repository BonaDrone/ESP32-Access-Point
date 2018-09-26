import socket
import argparse

# Default ESP32 HOST and PORT
HOST = "192.168.4.1"
PORT = 80

# Command line arguments
parser = argparse.ArgumentParser(description='Simple ESP32 connection test program')
parser.add_argument('-d','--data', type=str, action='store', help="data to send")

args = parser.parse_args()

def main(host, port, data="test data"):
	# create new socket
	sock = socket.socket()
	sock.connect((host, port))
	data_sent = sock.send(data)
	print "Sent "+str(data_sent)+" bytes of data"


if __name__ == '__main__':
	main(HOST, PORT, args.data)