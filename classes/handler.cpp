//
// Created by yura on 11/29/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include <wayland-server.h>
#include <wayland-client-protocol.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>

struct Handler{
    static void
    handle_ping(void *data, struct wl_shell_surface *shell_surface,
                uint32_t serial)
    {
        wl_shell_surface_pong(shell_surface, serial);
        fprintf(stderr, "Pinged and ponged\n");
    }

    static void
    handle_configure(void *data, struct wl_shell_surface *shell_surface,
                     uint32_t edges, int32_t width, int32_t height)
    {
        fprintf(stderr, "wl_shell_surface_listener handle_configure called.\n");
    }

    static void
    handle_popup_done(void *data, struct wl_shell_surface *shell_surface)
    {
        fprintf(stderr, "wl_shell_surface_listener handle_popup_done called.\n");
    }

};
static int
set_cloexec_or_close(int fd)
{
    long flags;

    if (fd == -1)
        return -1;

    flags = fcntl(fd, F_GETFD);
    if (flags == -1)
        goto err;

    if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1)
        goto err;

    return fd;

    err:
    close(fd);
    return -1;
}

static int
create_tmpfile_cloexec(char *tmpname)
{
    int fd;

#ifdef HAVE_MKOSTEMP
    fd = mkostemp(tmpname, O_CLOEXEC);
        if (fd >= 0)
                unlink(tmpname);
#else
    fd = mkstemp(tmpname);
    if (fd >= 0) {
        fd = set_cloexec_or_close(fd);
        unlink(tmpname);
    }
#endif

    return fd;
}

/*
 * Create a new, unique, anonymous file of the given size, and
 * return the file descriptor for it. The file descriptor is set
 * CLOEXEC. The file is immediately suitable for mmap()'ing
 * the given size at offset zero.
 *
 * The file should not have a permanent backing store like a disk,
 * but may have if XDG_RUNTIME_DIR is not properly implemented in OS.
 *
 * The file name is deleted from the file system.
 *
 * The file is suitable for buffer sharing between processes by
 * transmitting the file descriptor over Unix sockets using the
 * SCM_RIGHTS methods.
 */
int
os_create_anonymous_file(off_t size)
{
    static const char temp[] = "/weston-shared-XXXXXX";
    const char *path;
    char *name;
    int fd;

    path = getenv("XDG_RUNTIME_DIR");
    if (!path) {
        errno = ENOENT;
        return -1;
    }

    name = static_cast<char *>(malloc(strlen(path) + sizeof(temp)));
    if (!name)
        return -1;
    strcpy(name, path);
    strcat(name, temp);

    fd = create_tmpfile_cloexec(name);

    free(name);

    if (fd < 0)
        return -1;

    if (ftruncate(fd, size) < 0) {
        close(fd);
        return -1;
    }

    return fd;
}
void *shm_data;
static struct wl_buffer *
create_buffer(int WIDTH, int HEIGHT,uint32_t color,wl_shm *shm) {
    struct wl_shm_pool *pool;
    int stride = WIDTH * 4; // 4 bytes per pixel
    int size = stride * HEIGHT;
    int fd;
    struct wl_buffer *buff;

    fd = os_create_anonymous_file(size);
    if (fd < 0) {
        fprintf(stderr, "creating a buffer file for %d B failed: %m\n",
                size);
        exit(1);
    }

    shm_data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm_data == MAP_FAILED) {
        fprintf(stderr, "mmap failed: %m\n");
        close(fd);
        exit(1);
    }

    pool = wl_shm_create_pool(shm, fd, size);
    buff = wl_shm_pool_create_buffer(pool, 0,
                                     WIDTH, HEIGHT,
                                     stride,
                                     WL_SHM_FORMAT_XRGB8888);
    wl_shm_pool_destroy(pool);
    return buff;
}