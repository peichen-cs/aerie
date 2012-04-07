#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tool/testfw/integrationtest.h"
#include "tool/testfw/testfw.h"
#include "pxfs/client/client_i.h"
#include "pxfs/client/libfs.h"
#include "mfs.fixture.h"

using namespace client;

static const char* storage_pool_path = "/tmp/stamnos_pool";
static const char* test_str1 = "TEST_CREATE";

SUITE(MFSFile)
{
	TEST_FIXTURE(MFSFixture, TestCreate)
	{
		int  fd;
		char buf[512];

		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK(libfs_mkdir("/home/hvolos/dir", 0) == 0);
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_CREAT|O_RDWR)) > 0);
		CHECK(libfs_write(fd, test_str1, strlen(test_str1)+1) > 0);
		CHECK(libfs_close(fd) == 0);
		EVENT("E2");
		EVENT("E3");
	}

	TEST_FIXTURE(MFSFixture, TestOpen)
	{
		int  fd;
		char buf[512];

		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_RDWR)) > 0);
		CHECK(libfs_read(fd, buf, strlen(test_str1)+1) > 0);
		CHECK(strcmp(buf, test_str1) == 0);
		CHECK(libfs_close(fd) == 0);
		EVENT("E2");
		EVENT("E3");
	}
}