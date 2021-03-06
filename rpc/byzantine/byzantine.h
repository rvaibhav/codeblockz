/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _BYZANTINE_H_RPCGEN
#define _BYZANTINE_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct requestMessage {
	int message;
	int srcId;
	int destId;
};
typedef struct requestMessage requestMessage;

struct responseMessage {
	int message;
	int srcId;
	int destId;
};
typedef struct responseMessage responseMessage;

#define BYZANTINE_PROG 0x31240000
#define BYZANTINE_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define SENDMESSAGE 1
extern  responseMessage * sendmessage_1(requestMessage *, CLIENT *);
extern  responseMessage * sendmessage_1_svc(requestMessage *, struct svc_req *);
extern int byzantine_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define SENDMESSAGE 1
extern  responseMessage * sendmessage_1();
extern  responseMessage * sendmessage_1_svc();
extern int byzantine_prog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_requestMessage (XDR *, requestMessage*);
extern  bool_t xdr_responseMessage (XDR *, responseMessage*);

#else /* K&R C */
extern bool_t xdr_requestMessage ();
extern bool_t xdr_responseMessage ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_BYZANTINE_H_RPCGEN */
