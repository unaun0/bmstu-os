#include "prodcons.h"

bool_t
xdr_REQUEST (XDR *xdrs, REQUEST *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->idx))
		 return FALSE;
	 if (!xdr_char (xdrs, &objp->type))
		 return FALSE;
	 if (!xdr_char (xdrs, &objp->result))
		 return FALSE;
	return TRUE;
}
