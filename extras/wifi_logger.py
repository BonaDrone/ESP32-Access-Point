"""
Script that stores data received from the ESP32 in a csv file.

Setup:
	- ESP32 should run:
		https://github.com/BonaDrone/ESP32-Sketchs/blob/master/ESP32-Proxy-Data/ESP32-Proxy-Data.ino
	- Board should be BonaDrone FC, where board Serial4 is ESP32 Serial
	- To send data from the board to the PC / Laptop :
		Serial4.print(val_to_send_1);
		Serial4.print(",");
		Serial4.print(val_to_send_2);
		Serial4.print(",");
		Serial.print(val_to_send_3);
		Serial.print(terminator);  -> Terminator should match the terminator in this file, "\n" by default
	- Laptop / PC should be connected to the network created by the ESP32 before running this script

Author: Juan Gallostra
Date: 29-10-2018
"""

import socket
import argparse
import time
import csv


# Default ESP32 HOST and PORT
HOST = "192.168.4.1"
PORT = 80
# default number of bytes to close and open new socket
DEFAULT_BYTES_LIMIT = 50


# Command line arguments
parser = argparse.ArgumentParser(description='ESP32 wifi based data logger')

parser.add_argument('-f','--file', type=str, action='store', help="file to store data", default="data.csv")
parser.add_argument('-t','--terminator', type=str, action='store', help="data line terminator", default="\n")
parser.add_argument('-b','--bytes', type=int, action='store', 
					help="number of bytes to receive before closing and opening new socket")

args = parser.parse_args()

def data_not_corrupted(data, reference):
	"""
	Perform sanity checks to validate that the received data
	matches the expected format
	"""
	# Check equal lengths
	if len(reference)-3 > len(data) > len(reference)+3:
		return False
	# Check equal number of values
	data_vals = data.split(",")
	ref_vals = reference.split(",")
	if len(data_vals) != len(ref_vals):
		return False
	# Check equal value format for each value
	for idx in range(len(ref_vals)):
		if data_vals[idx].count(".") != ref_vals[idx].count("."):
			return False
	return True

def write_log(reference, success, fail):
	"""
	Write transmission log to terminal
	"""
	print("\nReference line was: {}\nTotal lines received: {}\nSuccessful: {}\nCorrupted: {}".format(
		reference,
		success+fail,
		success,
		fail))

def main(host, port, bytes_limit, terminator="\n", file="data.csv"):
	# intialize counters of successful and corrupted lines
	# as well as the reference line variable
	successful_lines = 0
	corrupted_lines = 0
	reference_line = ""
	ref_line_set = False
	received_terminator = False
	# create new socket and bind it to the ESP32
	sock = socket.socket()
	sock.connect((host, port))
	print("Connected to ESP32")
	# Wait for everything to setup
	time.sleep(0.6)
	# to store ESP32 received data
	recv_data = ""
	# Start counting the number of received bytes.
	# For some yet unknown reason, when the number
	# of received bytes reaches 50 the socket starts
	# receiving inconsistent data. If the socket is
	# closed and a new one is created this problem
	# is magically avoided.
	# Upon further inspection, it seems that limit
	# has to be fine tuned depending on the data
	# to be received
	j = 0
	with open(file, "w") as f:
		# Loop forever my friend
		while True:
			try:
				data = sock.recv(1).decode()
				recv_data += data
				if data == terminator:
					# wipe out terminator since it is
					# not required for it to be "\n"
					recv_data = recv_data[:-1]
					print(recv_data)
					# the second time we receive the terminator
					# set the received data as the reference format
					if not ref_line_set and received_terminator:
						reference_line = recv_data
						ref_line_set = True
					if data_not_corrupted(recv_data, reference_line):
						# write to csv file and reset line
						f.write(recv_data)
						f.write("\n")
						successful_lines += 1
					else:
						corrupted_lines += 1	
					recv_data = ""
					received_terminator = True
				# Update received bytes counter and
				# reset if necessary
				j+=1
				if j == bytes_limit:
					j = 0
					sock.close()
					sock = socket.socket()
					sock.connect((host, port))
			# since the loop executes forever the program will be stopped
			# via Ctrl+C. If we don't handle the "event" gracefully and exit
			# ourselves the loop the data will not be stored.
			except KeyboardInterrupt:
				break
	sock.close()
	write_log(reference_line, successful_lines, corrupted_lines)

if __name__ == '__main__':
	if not args.bytes:
		args.bytes = DEFAULT_BYTES_LIMIT
	main(HOST, PORT, args.bytes, terminator=args.terminator, file=args.file)
