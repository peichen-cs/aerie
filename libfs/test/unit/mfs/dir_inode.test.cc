#include <stdio.h>
#include <stdlib.h>
#include "tool/testfw/unittest.h"
#include "mfs/client/dir_inode.h"
#include "common/errno.h"
#include "unit/client/client.fixture.h"
#include "mfs/client/sb.h"

using namespace mfs;

SUITE(MFSDirInode)
{
	void InitNames(client::Session* session, DirPnode<client::Session>* dirpnode) 
	{
		DirPnode<client::Session>* child;
		
		child = new(session) DirPnode<client::Session>;
		CHECK(dirpnode->Link(session, ".", (uint64_t) child) == 0);
		child = new(session) DirPnode<client::Session>;
		CHECK(dirpnode->Link(session, "..", (uint64_t) child) == 0);
		child = new(session) DirPnode<client::Session>;
		CHECK(dirpnode->Link(session, "foo", (uint64_t) child) == 0);
		child = new(session) DirPnode<client::Session>;
		CHECK(dirpnode->Link(session, "bar", (uint64_t) child) == 0);
		child = new(session) DirPnode<client::Session>;
		CHECK(dirpnode->Link(session, "doc", (uint64_t) child) == 0);
	}


	TEST_FIXTURE(ClientFixture, TestLink)
	{
		client::Inode*             inode;
		SuperBlock*                sb = new SuperBlock(session);
		DirPnode<client::Session>* dirpnode = new(session) DirPnode<client::Session>;
		DirPnode<client::Session>* child;
		
		InitNames(session, dirpnode);

		DirInodeMutable* dinode = new DirInodeMutable(sb, dirpnode); 
		CHECK(dinode->Lookup(session, "foo", &inode)==0);
		CHECK(dinode->Lookup(session, "bar", &inode)==0);
		CHECK(dinode->Lookup(session, "doc", &inode)==0);
		
		child = new(session) DirPnode<client::Session>;
		CHECK(dinode->Link(session, "media", (uint64_t) child, false)==0);
		CHECK(dinode->Lookup(session, "media", &inode) == 0);
	
	}

	TEST_FIXTURE(ClientFixture, TestUnlink)
	{
		client::Inode*             inode;
		SuperBlock*                sb = new SuperBlock(session);
		DirPnode<client::Session>* dirpnode = new(session) DirPnode<client::Session>;
		DirPnode<client::Session>* child;
		
		InitNames(session, dirpnode);

		DirInodeMutable* dinode = new DirInodeMutable(sb, dirpnode); 
		CHECK(dinode->Lookup(session, "foo", &inode)==0);
		CHECK(dinode->Lookup(session, "bar", &inode)==0);
		CHECK(dinode->Lookup(session, "doc", &inode)==0);
		
		CHECK(dinode->Unlink(session, "foo")==0);
		CHECK(dinode->Lookup(session, "foo", &inode) != 0);
		
		child = new(session) DirPnode<client::Session>;
		CHECK(dinode->Link(session, "foo", (uint64_t) child, false)==0);
		CHECK(dinode->Lookup(session, "foo", &inode) == 0);
		CHECK(inode->GetInodeNumber() == (uint64_t) child);
	}


	TEST_FIXTURE(ClientFixture, TestLinkPublish1)
	{
		client::Inode*             inode;
		SuperBlock*                sb = new SuperBlock(session);
		DirPnode<client::Session>* dirpnode = new(session) DirPnode<client::Session>;
		DirPnode<client::Session>* child;
		DirPnode<client::Session>* child1;
		DirPnode<client::Session>* child2;
		uint64_t                   ino;

		InitNames(session, dirpnode);

		DirInodeMutable* dinode = new DirInodeMutable(sb, dirpnode); 
		CHECK(dinode->Lookup(session, "foo", &inode)==0);
		CHECK(dinode->Lookup(session, "bar", &inode)==0);
		CHECK(dinode->Lookup(session, "doc", &inode)==0);
		
		child1 = new(session) DirPnode<client::Session>;
		CHECK(dinode->Link(session, "media", (uint64_t) child1, false)==0);
		CHECK(dinode->Lookup(session, "media", &inode) == 0);
	
		child2 = new(session) DirPnode<client::Session>;
		CHECK(dinode->Link(session, "music", (uint64_t) child2, false)==0);
		CHECK(dinode->Lookup(session, "music", &inode) == 0);
	
		CHECK(dinode->Publish(session) == 0);
		CHECK(dirpnode->Lookup(session, "media", &ino)==0);
		CHECK(ino == (uint64_t) child1);	
		CHECK(dirpnode->Lookup(session, "music", &ino)==0);
		CHECK(ino == (uint64_t) child2);	
	}

	
	TEST_FIXTURE(ClientFixture, TestLinkPublish2)
	{
		client::Inode*             inode;
		SuperBlock*                sb = new SuperBlock(session);
		DirPnode<client::Session>* dirpnode = new(session) DirPnode<client::Session>;
		DirPnode<client::Session>* child;
		DirPnode<client::Session>* child1;
		DirPnode<client::Session>* child2;
		uint64_t                   ino;

		InitNames(session, dirpnode);

		DirInodeMutable* dinode = new DirInodeMutable(sb, dirpnode); 
		CHECK(dinode->Lookup(session, "foo", &inode)==0);
		CHECK(dinode->Lookup(session, "bar", &inode)==0);
		CHECK(dinode->Lookup(session, "doc", &inode)==0);
		
		CHECK(dinode->Unlink(session, "foo")==0);
		CHECK(dinode->Lookup(session, "foo", &inode) != 0);
		
		child1 = new(session) DirPnode<client::Session>;
		CHECK(dinode->Link(session, "media", (uint64_t) child1, false)==0);
		CHECK(dinode->Lookup(session, "media", &inode) == 0);
	
		CHECK(dinode->Publish(session) == 0);
		CHECK(dirpnode->Lookup(session, "media", &ino)==0);
		CHECK(ino == (uint64_t) child1);	
	}


	// checks that publish handles correctly the case when we unlink and 
	// then link the same name
	TEST_FIXTURE(ClientFixture, TestLinkPublish3)
	{
		client::Inode*             inode;
		SuperBlock*                sb = new SuperBlock(session);
		DirPnode<client::Session>* dirpnode = new(session) DirPnode<client::Session>;
		DirPnode<client::Session>* child;
		DirPnode<client::Session>* child1;
		DirPnode<client::Session>* child2;
		uint64_t                   ino;

		InitNames(session, dirpnode);

		DirInodeMutable* dinode = new DirInodeMutable(sb, dirpnode); 
		CHECK(dinode->Lookup(session, "foo", &inode)==0);
		CHECK(dinode->Lookup(session, "bar", &inode)==0);
		CHECK(dinode->Lookup(session, "doc", &inode)==0);
		
		CHECK(dinode->Unlink(session, "foo")==0);
		CHECK(dinode->Lookup(session, "foo", &inode) != 0);
		
		child1 = new(session) DirPnode<client::Session>;
		CHECK(dinode->Link(session, "foo", (uint64_t) child1, false)==0);
		CHECK(dinode->Lookup(session, "foo", &inode) == 0);
	
		CHECK(dinode->Publish(session) == 0);
		CHECK(dirpnode->Lookup(session, "foo", &ino)==0);
		CHECK(ino == (uint64_t) child1);	
	}


}