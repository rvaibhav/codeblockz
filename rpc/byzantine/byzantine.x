struct requestMessage{
	int message;
	int srcId;
	int destId;
};

struct responseMessage{
	int message;
	int srcId;
	int destId;
};

program BYZANTINE_PROG {
	version BYZANTINE_VERS {
		responseMessage SENDMESSAGE(requestMessage) = 1;
	} = 1;
} = 0x31240000;
