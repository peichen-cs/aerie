#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tool/testfw/integrationtest.h"
#include "tool/testfw/testfw.h"
#include "client/client_i.h"
#include "client/libfs.h"
#include "mfs.fixture.h"

using namespace client;

const char* storage_pool_path = "/tmp/stamnos_spool";

SUITE(Namespace)
{
	TEST_FIXTURE(MFSFixture, TestMkfs)
	{
		CHECK(libfs_mkfs(storage_pool_path, "mfs", 1024, 0) == 0);
		EVENT("MkfsAfter");
	}

	TEST_FIXTURE(MFSFixture, TestMount)
	{
		EVENT("MountBefore");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
	}

	TEST_FIXTURE(MFSFixture, TestMkfsMount)
	{
		CHECK(libfs_mkfs(storage_pool_path, "mfs", 1024, 0) == 0);
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		EVENT("MountAfter");
	}

	TEST_FIXTURE(MFSFixture, TestMkfsMkdir)
	{
		EVENT("E1");
		CHECK(libfs_mkfs(storage_pool_path, "mfs", 1024, 0) == 0);
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK(libfs_mkdir("/home/hvolos/dir", 0) == 0);
		CHECK(libfs_mkdir("/home/hvolos/dir/test", 0) == 0);
		EVENT("E2");
		EVENT("E3");
	}

	TEST_FIXTURE(MFSFixture, TestRmdir)
	{
		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		EVENT("E2");
		CHECK(libfs_rmdir("/home/hvolos/dir") != 0);
		CHECK(libfs_rmdir("/home/hvolos/dir/test") == 0);
		CHECK(libfs_rmdir("/home/hvolos/dir") == 0);
		EVENT("E3");
	}

}
