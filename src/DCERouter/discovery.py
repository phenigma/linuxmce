#!/usr/bin/python

# Server program
import socket

# Set the socket parameters
addr = ('', 33333)  # host, port

# Create socket and bind to address
UDPSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
UDPSock.bind(addr)

# Receive messages
while True:
	data, addr = UDPSock.recvfrom(1024)
	print "From addr: '%s', msg: '%s'" % (addr[0], data)
	UDPSock.sendto("Service: DCEROUTER", addr);

# Close socket
UDPSock.close()
print 'Server stopped.'
