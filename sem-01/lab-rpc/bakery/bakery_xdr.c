/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "bakery.h"

bool_t
xdr_REQUEST (XDR *xdrs, REQUEST *objp)
{
	register int32_t *buf;


	if (xdrs->x_op == XDR_ENCODE) {
		buf = XDR_INLINE (xdrs, 4 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->pid))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->idx))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->num))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->op))
				 return FALSE;

		} else {
		IXDR_PUT_LONG(buf, objp->pid);
		IXDR_PUT_LONG(buf, objp->idx);
		IXDR_PUT_LONG(buf, objp->num);
		IXDR_PUT_LONG(buf, objp->op);
		}
		 if (!xdr_float (xdrs, &objp->arg1))
			 return FALSE;
		 if (!xdr_float (xdrs, &objp->arg2))
			 return FALSE;
		 if (!xdr_float (xdrs, &objp->res))
			 return FALSE;
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		buf = XDR_INLINE (xdrs, 4 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->pid))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->idx))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->num))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->op))
				 return FALSE;

		} else {
		objp->pid = IXDR_GET_LONG(buf);
		objp->idx = IXDR_GET_LONG(buf);
		objp->num = IXDR_GET_LONG(buf);
		objp->op = IXDR_GET_LONG(buf);
		}
		 if (!xdr_float (xdrs, &objp->arg1))
			 return FALSE;
		 if (!xdr_float (xdrs, &objp->arg2))
			 return FALSE;
		 if (!xdr_float (xdrs, &objp->res))
			 return FALSE;
	 return TRUE;
	}

	 if (!xdr_int (xdrs, &objp->pid))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->idx))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->num))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->op))
		 return FALSE;
	 if (!xdr_float (xdrs, &objp->arg1))
		 return FALSE;
	 if (!xdr_float (xdrs, &objp->arg2))
		 return FALSE;
	 if (!xdr_float (xdrs, &objp->res))
		 return FALSE;
	return TRUE;
}
