#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <string>
#include "common/errno.h"
#include "tool/testfw/integrationtest.h"
#include "tool/testfw/testfw.h"
#include "ssa/main/client/rwproxy.h"
#include "ssa/main/client/omgr.h"
#include "ssa/containers/byte/container.h"
#include "ssa/containers/name/container.h"
#include "ssa/containers/typeid.h"
#include "pxfs/client/client_i.h"
#include "pxfs/client/libfs.h"
#include "test/integration/ssa/ssa.fixture.h"
#include "pxfs/mfs/client/dir_inode.h"
#include "pxfs/mfs/client/file_inode.h"
#include "pxfs/common/publisher.h"
#include "mfs.fixture.h"

static ssa::common::ObjectId OID[32];

typedef ssa::containers::client::NameContainer NameContainer;
typedef ssa::containers::client::ByteContainer ByteContainer;

static const char* storage_pool_path = "/tmp/stamnos_pool";

SUITE(MFSDirInode)
{
	void InitNames(client::Session* session, client::Inode* dinode) 
	{
		::client::Inode*                     inode;
		::ssa::common::ObjectProxyReference* rw_ref;
		::NameContainer::Reference*          rw_reft;
		::mfs::client::DirInode*             cinode;

		/* foo */
		CHECK(global_storage_system->omgr()->GetObject(session, OID[2], &rw_ref) == E_SUCCESS);
		cinode = new ::mfs::client::DirInode(rw_ref);
		CHECK(dinode->Link(session, "foo", cinode, false) == 0);
		/* bar */
		CHECK(global_storage_system->omgr()->GetObject(session, OID[3], &rw_ref) == E_SUCCESS);
		cinode = new ::mfs::client::DirInode(rw_ref);
		CHECK(dinode->Link(session, "bar", cinode, false) == 0);
		/* doc */
		CHECK(global_storage_system->omgr()->GetObject(session, OID[4], &rw_ref) == E_SUCCESS);
		cinode = new ::mfs::client::DirInode(rw_ref);
		CHECK(dinode->Link(session, "doc", cinode, false) == 0);
	}

	void CreateFilesDirect(client::Session* session, ssa::common::ObjectId dir_oid, std::vector<std::string>& vec) 
	{
		ByteContainer::Object* byte_obj;
		NameContainer::Object* dir_obj;

		dir_obj = ssa::containers::client::NameContainer::Object::Load(OID[0]);
		for (int i=0; i < vec.size(); i++) {
			byte_obj = ByteContainer::Object::Load(OID[16+i]);
			dir_obj->Insert(session, vec[i].c_str(), byte_obj->oid());
			byte_obj->set_parent(dir_obj->oid());
		}
	}


	TEST_FIXTURE(MFSFixture, TestLink)
	{
		::client::Inode*                   inode;
		ssa::common::ObjectProxyReference* rw_ref;
		NameContainer::Reference*          rw_reft;
		::mfs::client::DirInode*           dinode;
		::mfs::client::DirInode*           cinode;

		// FIXME
		// ugly hack: to load the storage pool/allocator we mount the pool as a filesystem.
		// instead the ssa layer should allow us to mount just the storage system 
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);


		CHECK(MapObjects<NameContainer::Object>(session, SELF, OID, 0, 16) == 0);
		
		CHECK(global_storage_system->omgr()->GetObject(session, OID[1], &rw_ref) == E_SUCCESS);
		rw_reft = static_cast<NameContainer::Reference*>(rw_ref);
		dinode = new ::mfs::client::DirInode(rw_ref);
		
		dinode->Lock(session, lock_protocol::Mode::XL);
		InitNames(session, dinode);
		
		CHECK(dinode->Lookup(session, "foo", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "bar", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "doc", 0, &inode) == E_SUCCESS);

		CHECK(global_storage_system->omgr()->GetObject(session, OID[5], &rw_ref) == E_SUCCESS);
		cinode = new ::mfs::client::DirInode(rw_ref);
		CHECK(dinode->Link(session, "media", cinode, false) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "media", 0, &inode) == E_SUCCESS);
		
		dinode->Unlock(session);
	}


	TEST_FIXTURE(MFSFixture, TestUnlink)
	{
		::client::Inode*                   inode;
		ssa::common::ObjectProxyReference* rw_ref;
		NameContainer::Reference*          rw_reft;
		::mfs::client::DirInode*           dinode;
		::mfs::client::DirInode*           cinode;

		// FIXME
		// ugly hack: to load the storage pool/allocator we mount the pool as a filesystem.
		// instead the ssa layer should allow us to mount just the storage system 
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);


		CHECK(MapObjects<NameContainer::Object>(session, SELF, OID, 0, 16) == 0);
		
		CHECK(global_storage_system->omgr()->GetObject(session, OID[1], &rw_ref) == E_SUCCESS);
		rw_reft = static_cast<NameContainer::Reference*>(rw_ref);
		dinode = new ::mfs::client::DirInode(rw_ref);
		
		dinode->Lock(session, lock_protocol::Mode::XL);
		InitNames(session, dinode);
		
		CHECK(dinode->Lookup(session, "foo", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "bar", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "doc", 0, &inode) == E_SUCCESS);

		CHECK(dinode->Unlink(session, "foo") == E_SUCCESS);
		CHECK(dinode->Lookup(session, "foo", 0, &inode) != E_SUCCESS);


		CHECK(global_storage_system->omgr()->GetObject(session, OID[5], &rw_ref) == E_SUCCESS);
		cinode = new ::mfs::client::DirInode(rw_ref);
		CHECK(dinode->Link(session, "foo", cinode, false) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "foo", 0, &inode) == E_SUCCESS);
		
		dinode->Unlock(session);
	}


	TEST_FIXTURE(MFSFixture, TestLink1_publisher)
	{
		::client::Inode*                   inode;
		ssa::common::ObjectProxyReference* rw_ref;
		NameContainer::Reference*          rw_reft;
		::mfs::client::DirInode*           dinode;
		::mfs::client::DirInode*           child1;

		// FIXME
		// ugly hack: to load the storage pool/allocator we mount the pool as a filesystem.
		// instead the ssa layer should allow us to mount just the storage system 
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);

		EVENT("BeforeMapObjects");
		CHECK(MapObjects<NameContainer::Object>(session, SELF, OID, 0, 16) == 0);
		EVENT("AfterMapObjects");
		
		CHECK(global_storage_system->omgr()->GetObject(session, OID[1], &rw_ref) == E_SUCCESS);
		rw_reft = static_cast<NameContainer::Reference*>(rw_ref);
		dinode = new ::mfs::client::DirInode(rw_ref);
	
		EVENT("BeforeLock");
		dinode->Lock(session, lock_protocol::Mode::XL);
		EVENT("AfterLock");
		InitNames(session, dinode);

		CHECK(dinode->Lookup(session, "foo", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "bar", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "doc", 0, &inode) == E_SUCCESS);
		
		CHECK(global_storage_system->omgr()->GetObject(session, OID[5], &rw_ref) == E_SUCCESS);
		child1 = new ::mfs::client::DirInode(rw_ref);
		CHECK(dinode->Link(session, "media", child1, false) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "media", 0, &inode) == E_SUCCESS);
		
		dinode->Unlock(session);
		EVENT("AfterUnlock");
		EVENT("End");
	}


	TEST_FIXTURE(MFSFixture, TestLink1_consumer)
	{
		::client::Inode*                   inode;
		ssa::common::ObjectProxyReference* rw_ref;
		NameContainer::Reference*          rw_reft;
		::mfs::client::DirInode*           dinode;

		// FIXME
		// ugly hack: to load the storage pool/allocator we mount the pool as a filesystem.
		// instead the ssa layer should allow us to mount just the storage system 
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);

		EVENT("BeforeMapObjects");
		CHECK(MapObjects<NameContainer::Object>(session, SELF, OID, 0, 16) == 0);
		EVENT("AfterMapObjects");
		
		CHECK(global_storage_system->omgr()->GetObject(session, OID[1], &rw_ref) == E_SUCCESS);
		rw_reft = static_cast<NameContainer::Reference*>(rw_ref);
		dinode = new ::mfs::client::DirInode(rw_ref);
	
		EVENT("BeforeLock");
		dinode->Lock(session, lock_protocol::Mode::XL);

		CHECK(dinode->Lookup(session, "foo", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "bar", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "doc", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "media", 0, &inode) == E_SUCCESS);
		
		dinode->Unlock(session);
		EVENT("AfterUnlock");
		EVENT("End");
	}


	TEST_FIXTURE(MFSFixture, TestLink2_publisher)
	{
		::client::Inode*                   inode;
		ssa::common::ObjectProxyReference* rw_ref;
		NameContainer::Reference*          rw_reft;
		::mfs::client::DirInode*           dinode;
		::mfs::client::DirInode*           child1;

		// FIXME
		// ugly hack: to load the storage pool/allocator we mount the pool as a filesystem.
		// instead the ssa layer should allow us to mount just the storage system 
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);

		EVENT("BeforeMapObjects");
		CHECK(MapObjects<NameContainer::Object>(session, SELF, OID, 0, 16) == 0);
		EVENT("AfterMapObjects");
		
		CHECK(global_storage_system->omgr()->GetObject(session, OID[1], &rw_ref) == E_SUCCESS);
		rw_reft = static_cast<NameContainer::Reference*>(rw_ref);
		dinode = new ::mfs::client::DirInode(rw_ref);
	
		EVENT("BeforeLock");
		dinode->Lock(session, lock_protocol::Mode::XL);
		EVENT("AfterLock");
		InitNames(session, dinode);

		CHECK(dinode->Lookup(session, "foo", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "bar", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "doc", 0, &inode) == E_SUCCESS);
		
		CHECK(dinode->Unlink(session, "foo") == E_SUCCESS);

		CHECK(global_storage_system->omgr()->GetObject(session, OID[5], &rw_ref) == E_SUCCESS);
		child1 = new ::mfs::client::DirInode(rw_ref);
		CHECK(dinode->Link(session, "foo", child1, false) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "foo", 0, &inode) == E_SUCCESS);
		
		dinode->Unlock(session);
		EVENT("AfterUnlock");
		EVENT("End");
	}


	TEST_FIXTURE(MFSFixture, TestLink2_consumer)
	{
		::client::Inode*                   inode;
		ssa::common::ObjectProxyReference* rw_ref;
		NameContainer::Reference*          rw_reft;
		::mfs::client::DirInode*           dinode;

		// FIXME
		// ugly hack: to load the storage pool/allocator we mount the pool as a filesystem.
		// instead the ssa layer should allow us to mount just the storage system 
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);

		EVENT("BeforeMapObjects");
		CHECK(MapObjects<NameContainer::Object>(session, SELF, OID, 0, 16) == 0);
		EVENT("AfterMapObjects");
		
		CHECK(global_storage_system->omgr()->GetObject(session, OID[1], &rw_ref) == E_SUCCESS);
		rw_reft = static_cast<NameContainer::Reference*>(rw_ref);
		dinode = new ::mfs::client::DirInode(rw_ref);
	
		EVENT("BeforeLock");
		dinode->Lock(session, lock_protocol::Mode::XL);

		CHECK(dinode->Lookup(session, "foo", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "bar", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "doc", 0, &inode) == E_SUCCESS);
		
		dinode->Unlock(session);
		EVENT("AfterUnlock");
		EVENT("End");
	}


	TEST_FIXTURE(MFSFixture, TestUnlink1_publisher)
	{
		::client::Inode*                   inode;
		ssa::common::ObjectProxyReference* rw_ref;
		NameContainer::Reference*          rw_reft;
		::mfs::client::DirInode*           dinode;
		::mfs::client::DirInode*           child1;

		// FIXME
		// ugly hack: to load the storage pool/allocator we mount the pool as a filesystem.
		// instead the ssa layer should allow us to mount just the storage system 
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);

		EVENT("BeforeMapObjects");
		CHECK(MapObjects<NameContainer::Object>(session, SELF, OID, 0, 16) == 0);
		EVENT("AfterMapObjects");
		
		CHECK(global_storage_system->omgr()->GetObject(session, OID[1], &rw_ref) == E_SUCCESS);
		rw_reft = static_cast<NameContainer::Reference*>(rw_ref);
		dinode = new ::mfs::client::DirInode(rw_ref);
	
		EVENT("BeforeLock");
		dinode->Lock(session, lock_protocol::Mode::XL);
		EVENT("AfterLock");
		InitNames(session, dinode);

		CHECK(dinode->Lookup(session, "foo", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "bar", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "doc", 0, &inode) == E_SUCCESS);
		
		CHECK(dinode->Unlink(session, "foo") == E_SUCCESS);
		
		dinode->Unlock(session);
		EVENT("AfterUnlock");
		EVENT("End");
	}

	TEST_FIXTURE(MFSFixture, TestUnlink1_consumer)
	{
		::client::Inode*                   inode;
		ssa::common::ObjectProxyReference* rw_ref;
		NameContainer::Reference*          rw_reft;
		::mfs::client::DirInode*           dinode;

		// FIXME
		// ugly hack: to load the storage pool/allocator we mount the pool as a filesystem.
		// instead the ssa layer should allow us to mount just the storage system 
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);

		EVENT("BeforeMapObjects");
		CHECK(MapObjects<NameContainer::Object>(session, SELF, OID, 0, 16) == 0);
		EVENT("AfterMapObjects");
		
		CHECK(global_storage_system->omgr()->GetObject(session, OID[1], &rw_ref) == E_SUCCESS);
		rw_reft = static_cast<NameContainer::Reference*>(rw_ref);
		dinode = new ::mfs::client::DirInode(rw_ref);
	
		EVENT("BeforeLock");
		dinode->Lock(session, lock_protocol::Mode::XL);

		CHECK(dinode->Lookup(session, "foo", 0, &inode) != E_SUCCESS);
		CHECK(dinode->Lookup(session, "bar", 0, &inode) == E_SUCCESS);
		CHECK(dinode->Lookup(session, "doc", 0, &inode) == E_SUCCESS);
		
		dinode->Unlock(session);
		EVENT("AfterUnlock");
		EVENT("End");
	}


	TEST_FIXTURE(SsaFixture, TestFileWrite)
	{
		char                               buf[512];
		::client::Inode*                   inode;
		ssa::common::ObjectProxyReference* rw_ref;
		NameContainer::Reference*          rw_reft;
		::mfs::client::DirInode*           dinode;
		::mfs::client::FileInode*          child;

		EVENT("BeforeMapObjects");
		CHECK(MapObjects<NameContainer::Object>(session, SELF, OID, 0, 16) == 0);
		CHECK(MapObjects<ByteContainer::Object>(session, SELF, OID, 16, 16) == 0);
		EVENT("AfterMapObjects");

		std::vector<std::string> vec;
		vec.push_back("foo");
		vec.push_back("bar");
		vec.push_back("doc");
		CreateFilesDirect(session, OID[0], vec);
		
		CHECK(global_storage_system->omgr()->GetObject(session, OID[0], &rw_ref) == E_SUCCESS);
		dinode = new ::mfs::client::DirInode(rw_ref);
	
		EVENT("BeforeLock");
		dinode->Lock(session, lock_protocol::Mode::XR);
		CHECK(dinode->Lookup(session, "foo", 0, &inode) == E_SUCCESS);
		child = reinterpret_cast< ::mfs::client::FileInode*>(inode);
		child->Lock(session, dinode, lock_protocol::Mode::XL);
		// do the logical journaling ourselves because we bypass the file system interface
		session->journal()->TransactionBegin();
		session->journal() << Publisher::Messages::LogicalOperation::Write(child->ino());
		strcpy(buf, "FOO");
		CHECK(child->Write(session, buf, 0, strlen(buf)+1) == strlen(buf) + 1);
		session->journal()->TransactionEnd();
		child->Unlock(session);
		dinode->Unlock(session);
		libfs_sync();
		EVENT("AfterUnlock");
		EVENT("End");
	}
}
