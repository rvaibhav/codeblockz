#include <stdio.h>
#include "byzantine.h"
#include <pthread.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <rpc/rpc.h>

#define LOYAL 1
#define TRAITOR 0
#define ATTACK 1
#define RETREAT 0
#define DILEMMA 2
#define N 4
#define ARBIT_VALUE 9999

int nodes[N] = {LOYAL,LOYAL,TRAITOR,LOYAL};
int req[N][N];
int resp[N][N]; /*Array used to store message sent from i reveived at j*/
int decisionAtNode[N]; /* Array to store decision at nodes */
int args;
char *host;
char *clientMessage, *serverMessage;

responseMessage * sendmessage_1_svc(requestMessage *in,struct svc_req *rqst)
{
	static responseMessage response;
	int source = in->srcId;
	int destination = in->destId;
	int message = in->message;
	int i;
	resp[source][destination] = message;
	if(nodes[destination] == LOYAL)
	{
		//Do nothing
	}
	else
	{
		if(message == ATTACK)
		{
			message = RETREAT;
		}
		else
		{
			message =ATTACK;
		}
	}
	if(message == ATTACK)
	{
		serverMessage = "ATTACK";
	}
	else
	{
		serverMessage = "RETREAT";
	}
	for(i=0;i<N;i++)
	{
		if(i == destination)
		{
			continue;		
		}
		if(req[destination][i] == ARBIT_VALUE && i != source)
		{
			req[destination][i] = message;
			resp[destination][i] = message;
		}
	}
	response.srcId = destination;
	response.destId = source;
	return &response;
}

static void byzantine_prog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		requestMessage sendmessage_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case SENDMESSAGE:
		_xdr_argument = (xdrproc_t) xdr_requestMessage;
		_xdr_result = (xdrproc_t) xdr_responseMessage;
		local = (char *(*)(char *, struct svc_req *)) sendmessage_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}

void* server_call()
{
	register SVCXPRT *transp;

	pmap_unset (BYZANTINE_PROG, BYZANTINE_VERS);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, BYZANTINE_PROG, BYZANTINE_VERS, byzantine_prog_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (BYZANTINE_PROG, BYZANTINE_VERS, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, BYZANTINE_PROG, BYZANTINE_VERS, byzantine_prog_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (BYZANTINE_PROG, BYZANTINE_VERS, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}

void* client_call()
{
	CLIENT *cl;
	requestMessage in;
	responseMessage *outp;
	int resp_source,resp_dest,attackCount,retreatCount,dilemmaCount,i,j,client;
	int message,finalDecision;
	if(args != 2)
	{
		printf("usage: client <hostname>\n");
		exit(1);
	}
	cl = clnt_create(host, BYZANTINE_PROG, BYZANTINE_VERS, "tcp");
	if(cl == NULL){
		clnt_pcreateerror(host);
       		exit(2);	
	}
	i = 0;
	in.message = ATTACK;
	message = in.message;
	if(message == ATTACK)
	{
		clientMessage = "ATTACK";
	}
	else
	{
		clientMessage = "RETREAT";
	}
	in.srcId = i;
	for(j=1;j<N;j++)
	{
		if(req[i][j] == ARBIT_VALUE)
		{	
			in.destId = j;
			req[i][j] = message;
			if ((outp=sendmessage_1(&in, cl))==NULL)
			{
				printf("%s", clnt_sperror(cl, host));
			}
		}
	}
	//Making decision at nodes
	client = 0;
	for(j=1;j<N;j++)
	{
		attackCount = 0;
		retreatCount = 0;
		for(i=0;i<N;i++)
		{	
			if(i != j){
				if(req[i][j] == ATTACK)
				{
					attackCount++;			
				}
				else{
					retreatCount++;			
				}
			}		
		}
		if(attackCount > retreatCount)
		{
			decisionAtNode[j] = ATTACK;	
		}
		else if(attackCount > retreatCount)
		{
			decisionAtNode[j] = RETREAT;
		}
		else
		{
			decisionAtNode[j] = DILEMMA;
		}
		/* Considering Traitor Node */
		if(nodes[j] == TRAITOR)
		{
			if(decisionAtNode[j] == ATTACK)
			{
				decisionAtNode[j] = RETREAT;
			}
			else if(decisionAtNode[j] == RETREAT)
			{
				decisionAtNode[j] = ATTACK;			
			}
		}
	}
	//Displaying the array
	printf("\nreq[i][j] means request sent from i and received at j ");
	printf("\n------------------------------------------------------");	
	for(j=1;j<N;j++)
	{
		for(i=0;i<N;i++)
		{	
			if(i != j){
				if(req[i][j] == ATTACK)
				{
					clientMessage = "ATTACK";			
				}
				else if(req[i][j] == RETREAT)
				{
					clientMessage = "RETREAT";
				}
				printf("\nreq[%d][%d] --- %s",i,j,clientMessage);
			}
		}
	}
	/* Sending message to Client Node */
	attackCount = 0;
	retreatCount = 0;
	dilemmaCount = 0;
	for(i=1;i<N;i++)
	{
		if(decisionAtNode[i] == ATTACK)
		{
			attackCount++;
		}
		else if(decisionAtNode[i] == RETREAT)
		{
			retreatCount++;		
		}
		else
		{
			dilemmaCount++;
		}
		req[i][client] = decisionAtNode[i];
	}
	/* Making decision at Client Node */
	if(attackCount > (N-1)/2)
	{
		decisionAtNode[client] = ATTACK;
	}
	else if(retreatCount > (N-1)/2)
	{
		decisionAtNode[client] = RETREAT;
	}
	else
	{
		decisionAtNode[client] = DILEMMA;
	}
	if(nodes[client] == TRAITOR)
	{
		if(decisionAtNode[client] = ATTACK)
		{
			decisionAtNode[client] = RETREAT;
		}
		else if(decisionAtNode[client] = RETREAT)
		{
			decisionAtNode[client] = ATTACK;
		}
	}
	attackCount = 0;
	retreatCount = 0;
	dilemmaCount = 0;
	for(i=0;i<N;i++){
		if(decisionAtNode[i] == ATTACK)
		{
			clientMessage = "ATTACK";
			attackCount++;
		}
		else if(decisionAtNode[i] == RETREAT)
		{
			clientMessage = "RETREAT";
			retreatCount++;
		}
		else
		{
			clientMessage = "DILEMMA";
			dilemmaCount++;
		}
		printf("\nDecision taken at Node: %d is %s",i,clientMessage);	
	}
	if(attackCount > N/2)
	{
		finalDecision = ATTACK;
		clientMessage = "ATTACK";
	}
	else if(retreatCount > N/2)
	{
		finalDecision = RETREAT;
		clientMessage = "RETREAT";
	}
	else
	{
		finalDecision = DILEMMA;
		clientMessage = "DILEMMA";
	}
	printf("\n\nFinal Consensus is %s",clientMessage);
	printf("\n Program execution completed on client side");
}

int main(int argc, char *argv[])
{
	int i,j,count;
	pthread_t client,server;
	if(argc != 2)
	{
		printf("usage: ./a.out <hostname>\n");
		exit(1);
	}
	if(nodes[0] == TRAITOR)
	{
		printf("You have specified the first element as TRAITOR.\nThe first node needs to be LOYAL! \n");
		exit(1);
	}
	printf("\nStarting server");
	args = argc;
	host = argv[1];
	count = 0;
	printf("\nValue of N is %d",N);
	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
			req[i][j] = ARBIT_VALUE;
		}
	}
	pthread_create(&server,NULL,server_call,NULL);
	printf("\nStarting Server");
	sleep(1);
	pthread_create(&client,NULL,client_call,NULL);
	printf("\nStarting Client\n");	
	sleep(2);
	pthread_join(server,NULL);
	pthread_join(client,NULL);
	exit(0);
}
