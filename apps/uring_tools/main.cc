#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/utsname.h>
#include "3rdparty/liburing/src/include/liburing.h"
#include "3rdparty/liburing/src/include/liburing/io_uring.h"

// io_uring_get_probe_ring Available since 5.6
static const char *op_strs[] = {
    "IORING_OP_NOP",
    "IORING_OP_READV",
    "IORING_OP_WRITEV",
    "IORING_OP_FSYNC",
    "IORING_OP_READ_FIXED",
    "IORING_OP_WRITE_FIXED",
    "IORING_OP_POLL_ADD",
    "IORING_OP_POLL_REMOVE",
    "IORING_OP_SYNC_FILE_RANGE",
    "IORING_OP_SENDMSG",
    "IORING_OP_RECVMSG",
    "IORING_OP_TIMEOUT",
    "IORING_OP_TIMEOUT_REMOVE",
    "IORING_OP_ACCEPT",
    "IORING_OP_ASYNC_CANCEL",
    "IORING_OP_LINK_TIMEOUT",
    "IORING_OP_CONNECT",
    "IORING_OP_FALLOCATE",
    "IORING_OP_OPENAT",
    "IORING_OP_CLOSE",
    "IORING_OP_FILES_UPDATE",
    "IORING_OP_STATX",
    "IORING_OP_READ",
    "IORING_OP_WRITE",
    "IORING_OP_FADVISE",
    "IORING_OP_MADVISE",
    "IORING_OP_SEND",
    "IORING_OP_RECV",
    "IORING_OP_OPENAT2",
    "IORING_OP_EPOLL_CTL",
    "IORING_OP_SPLICE",
    "IORING_OP_PROVIDE_BUFFERS",
    "IORING_OP_REMOVE_BUFFERS",
    "IORING_OP_TEE",
    "IORING_OP_SHUTDOWN",
    "IORING_OP_RENAMEAT",
    "IORING_OP_UNLINKAT",
    "IORING_OP_MKDIRAT",
    "IORING_OP_SYMLINKAT",
    "IORING_OP_LINKAT",
};

int main() {
    struct utsname u;
    uname(&u);
    printf("kernel version: %s\n", u.release);
	struct io_uring ring;
    int ret(0);
	ret = io_uring_queue_init(1, &ring, 0);
    if (ret < 0) {
        printf("io_uring_queue_init error, ret %d\n", ret);
        return 1;
    }
    struct io_uring_probe *probe = io_uring_get_probe_ring(&ring);
    if (probe == nullptr) {
        printf("io_uring_get_probe error, ret nullptr, errno %d\n", errno);
        return 1;
    }
    printf("your kernel's list of supported io_uring operations:\n");
    for (char i = 0; i < IORING_OP_LAST; i++ ) {
        printf("%s: ", op_strs[i]);
        if(io_uring_opcode_supported(probe, i))
            printf("yes.\n");
        else
            printf("no.\n");

    }
    io_uring_free_probe(probe);
    return 0;
}
