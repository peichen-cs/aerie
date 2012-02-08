#include "dpo/base/client/dpo.h"
#include "common/errno.h"
#include "dpo/base/client/smgr.h"
#include "dpo/base/client/omgr.h"
#include "dpo/base/client/lckmgr.h"
#include "dpo/base/client/hlckmgr.h"


namespace dpo {
namespace client {

int
DpoLayer::Init()
{
	if ((lckmgr_ = new ::dpo::cc::client::LockManager(rpc_client_, rpc_server_, id_)) == NULL) {
		return -E_NOMEM;
	}
	if ((hlckmgr_ = new ::dpo::cc::client::HLockManager(lckmgr_)) == NULL) {
		delete lckmgr_;
		return -E_NOMEM;
	}
	if ((omgr_ = new ObjectManager(lckmgr_, hlckmgr_)) == NULL) {
		delete hlckmgr_;
		delete lckmgr_;
		return -E_NOMEM;
	}
	if ((smgr_ = new StorageManager(rpc_client_, principal_id_)) == NULL) {
		delete omgr_;
		delete hlckmgr_;
		delete lckmgr_;
		return -E_NOMEM;
	}
	return E_SUCCESS;
}


DpoLayer::~DpoLayer()
{
	delete hlckmgr_;
	delete lckmgr_;
	hlckmgr_ = NULL; 
	lckmgr_ = NULL;
}

} // namespace client
} // namespace dpo