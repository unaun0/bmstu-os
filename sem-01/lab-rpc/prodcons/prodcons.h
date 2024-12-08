/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _PRODCONS_H_RPCGEN
#define _PRODCONS_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct REQUEST {
	int op;
	int pid;
	int idx;
	int res;
};
typedef struct REQUEST REQUEST;

#define PRODCONS_PROG 0x20000001
#define PRODCONS_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define PRODUCER 1
extern  int * producer_1(REQUEST *, CLIENT *);
extern  int * producer_1_svc(REQUEST *, struct svc_req *);
#define CONSUMER 2
extern  int * consumer_1(REQUEST *, CLIENT *);
extern  int * consumer_1_svc(REQUEST *, struct svc_req *);
extern int prodcons_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define PRODUCER 1
extern  int * producer_1();
extern  int * producer_1_svc();
#define CONSUMER 2
extern  int * consumer_1();
extern  int * consumer_1_svc();
extern int prodcons_prog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_REQUEST (XDR *, REQUEST*);

#else /* K&R C */
extern bool_t xdr_REQUEST ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_PRODCONS_H_RPCGEN */
