#ifndef __STAMNOS_KVFS_FILESYSTEM_PROTOCOL_H
#define __STAMNOS_KVFS_FILESYSTEM_PROTOCOL_H

#include "bcs/bcs.h"
#include "osd/main/common/stsystem.h"


class FileSystemProtocol {
public:
	
	enum RpcNumbers {
		DEFINE_RPC_NUMBER(KVFS_FILESYSTEM_PROTOCOL)
	};

	class MountReply;
};


class FileSystemProtocol::MountReply {
public:
	StorageSystemDescriptor desc_;
};


inline marshall& operator<<(marshall &m, FileSystemProtocol::MountReply& val) {
	m << val.desc_;
    return m;
}


inline unmarshall& operator>>(unmarshall &u, FileSystemProtocol::MountReply& val) {
	u >> val.desc_;
	return u;
}


#endif // __STAMNOS_KVFS_FILESYSTEM_PROTOCOL_H
