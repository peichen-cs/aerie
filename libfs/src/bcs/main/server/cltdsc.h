#ifndef __STAMNOS_BCS_SERVER_CLIENTDESCRIPTOR_H
#define __STAMNOS_BCS_SERVER_CLIENTDESCRIPTOR_H

#include "bcs/backend/rpc.h"
#include "bcs/main/common/macros.h"
#include "bcs/main/server/shbuf.h"

namespace server {

struct ClientDescriptor {
public:
	ClientDescriptor(int clt, rpcc* rpccl)
		: clt_(clt),
		  rpcc_(rpccl)
	{ }
	
	int Init();
	int clt() { return clt_; }
	SharedBuffer* shbuf() {	return shbuf_; }
	
	RPC_CALL(rpcc_, rpcc::to_max)

protected:
	rpcc*         rpcc_;
	int           clt_;
	SharedBuffer* shbuf_; 
};


} // namespace server

#endif // __STAMNOS_BCS_SERVER_CLIENTDESCRIPTOR_H
