#ifndef _PRODCONS_H_RPCGEN
#define _PRODCONS_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct REQUEST {
	int idx;
	char type;
	char result;
};
typedef struct REQUEST REQUEST;

#define PRODCONS_PROG 0x20000001
#define PRODCONS_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define service 1
extern  REQUEST * service_1(REQUEST *, CLIENT *);
extern  REQUEST * service_1_svc(REQUEST *, struct svc_req *);
extern int prodcons_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define service 1
extern  REQUEST * service_1();
extern  REQUEST * service_1_svc();
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
