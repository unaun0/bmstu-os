#include <memory.h> /* for memset */
#include "prodcons.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

REQUEST *
service_1(REQUEST *argp, CLIENT *clnt)
{
	static REQUEST clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, service,
		(xdrproc_t) xdr_REQUEST, (caddr_t) argp,
		(xdrproc_t) xdr_REQUEST, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}
