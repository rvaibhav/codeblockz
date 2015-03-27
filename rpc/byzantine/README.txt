gcc -lnsl -pthread agreement.c  byzantine_xdr.c byzantine_clnt.c

#Then you can run ./a.out localhost to start the output of the program.

#The server and module are in one file agreement.c

#agreement.c is the main file

#Change values at line#17 and line#20 to test the output.

#No. of elements in nodes array on line#20 must be equal to the value of N specified on line#17

Screenshot of sample program is in [byzantine.png]

Troubleshooting
1. If you face error saying Unable to register (BYZANTINE_PROG, BYZANTINE_VERS, "udp")
	then probably the service by this name is already registered.
	Try unregistering the service and retyr again.
