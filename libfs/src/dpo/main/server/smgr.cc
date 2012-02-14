#include "dpo/main/server/smgr.h"
#include <stdio.h>
#include <stddef.h>
#include "common/errno.h"
#include "ipc/ipc.h"
#include "dpo/main/common/storage_protocol.h"
#include "ipc/main/server/cltdsc.h"
#include "server/session.h"


namespace dpo {
namespace server {


//
// Group Containers per (type, ACL) 
//
//

class ContainersPerClient {
public:
	// verify_allocation (capability)
	// 
	

private:
	// index of available containers to client 


};


StorageManager::StorageManager(::server::Ipc* ipc)
	: ipc_(ipc)
{ }


int
StorageManager::Init()
{
    ipc_->reg(::dpo::StorageProtocol::kAllocateContainerVector, this, 
	          &::dpo::server::StorageManager::AllocateContainerVector);

	StorageDescriptor::Register();

	return E_SUCCESS;
}




#if 0

// Makes the OS dependent call to allocate space
int
StorageManager::CreateExtent(int acl)
{
	

}


int
StorageManager::CreateStorageContainer()
{


}


int 
StorageManager::AllocateContainerInternal(int clt, int type, int acl)
{


}



int 
StorageManager::AllocateContainer(int clt, int type, int acl, int n)
{


}

#endif


int
StorageManager::AllocateRaw(::server::Session* session, size_t size, void** ptr)
{


}

int 
StorageManager::AllocateContainer(int clt, int type, int num, ::dpo::StorageProtocol::Capability& cap)
{


}


int 
StorageManager::AllocateContainerVector(int clt, 
                                        std::vector< ::dpo::StorageProtocol::ContainerRequest> container_req_vec, 
                                        std::vector<int>& result)
{
	std::vector< ::dpo::StorageProtocol::ContainerRequest>::iterator vit;
	
	for (vit = container_req_vec.begin(); vit != container_req_vec.end(); vit++) {
		//printf("type: %d, num=%d\n", (*vit).type, (*vit).num);
		
		result.push_back(1);
	}

	return E_SUCCESS;
}


} // namespace server
} // namespace dpo