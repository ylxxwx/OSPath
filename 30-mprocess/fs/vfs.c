#include "vfs.h"
#include "ext2.h"
#include "inode.h"
#include "fsmem.h"
#include "trace.h"
#include "screen.h"
#include "string.h"
#include "mem.h"

#define VS_NAME_LEN 64
#define VS_MAX_ITEMS 64

#define ROOT_INODE 2

typedef struct vfs_node
{
    // vfs infor.
    char name[VS_NAME_LEN];
    u32 mode;
    u32 is_file;
    u32 is_dir;
    struct vfs_node *parent;
    struct vfs_node *childs[VS_MAX_ITEMS];

    // hw infor.
    disk_t disk;
    int inode_id;
    int synced;
} vfs_node_t;

vfs_node_t root;
static vfs_node_t *cur = 0;
void sync_node(vfs_node_t *node);

static vfs_node_t *path_to_node(char *path)
{
    vfs_node_t *target = cur;
    if (path[0] == '/')
        target = &root;

    char path_stack[16][80];
    kmemset((u8 *)path_stack, 0, 16 * 80);
    int num = split(path_stack, path, "/");
    for (int idx = 0; idx < num; idx++)
    {
        if (0 == strcmp("..", path_stack[idx]))
        {
            target = target->parent;
        }
        else if (0 == strcmp(".", path_stack[idx]))
        {
            continue;
        }
        else
        {
            bool found = 0;
            for (int i = 0; i < 64; i++)
            {
                if (target->childs[i] == 0)
                    continue;
                if (0 == cmp_str(target->childs[i]->name, path_stack[idx], 64))
                {
                    if (target->childs[i]->synced == 0)
                    {
                        sync_node(target->childs[i]);
                    }
                    target = target->childs[i];
                    found = 1;
                    break;
                }
            }
            if (found == 0)
                return 0;
        }
    }
    return target;
}

int read_inode_file(vfs_node_t *node, char *buf)
{
    trace("read_inode_file, %d\n", node->inode_id);
    disk_t *disk = &node->disk;
    int inode = node->inode_id;
    int nz = get_inode_file(disk, inode, buf);
}

void sync_node(vfs_node_t *node)
{
    //    show_disk("sync_node: ", &node->disk);
    node->mode = get_inode_mode(&node->disk, node->inode_id);
    if (node->mode == INVALID_MODE)
    {
        trace("sync node failed. mode is invalid.\n");
        return;
    }
    node->is_file = is_inode_file(&node->disk, node->inode_id);
    node->is_dir = is_inode_dir(&node->disk, node->inode_id);
    node->synced = 1;
    trace("sync node:%d, mode:%x, is_file:%d, is_dir:%x\n",
          node->inode_id, node->mode, node->is_file, node->is_dir);
    if (node->is_dir)
    {
        int idx = 0;
        int ret_node = 1;
        do
        {
            char name[64];
            ret_node = get_next_dir_entry(&node->disk, node->inode_id, name);
            if (ret_node > 0)
            {
                trace("read dir entry id:%d.\n", ret_node);
                vfs_node_t *child = (vfs_node_t *)alloc_mem(sizeof(vfs_node_t));
                clear_mem((u8 *)child, 0, sizeof(vfs_node_t));
                child->disk = node->disk;
                // show_disk("child disk: ", &child->disk);
                child->parent = node;
                node->childs[idx++] = child;
                move_str(child->name, name, 64);
                child->inode_id = ret_node;
                child->synced = 0;
            }
            else
            {
                trace("read dir entry failed.\n");
                break;
            }
        } while (ret_node > 0);
    }
}

int mount_root(disk_t *disk)
{
    // enable_trace();
    clear_mem(root.name, 0, VS_NAME_LEN);
    root.name[0] = '/';
    root.disk = *disk;
    root.inode_id = ROOT_INODE;
    root.synced = 0;
    root.parent = &root;
    for (int idx = 0; idx < VS_MAX_ITEMS; idx++)
    {
        root.childs[idx] = 0;
    }
    cur = &root;
    sync_node(cur);
    ls_dir(".");
}

int mount(disk_t *disk, char *path)
{
    return 0;
}

int umount(disk_t *disk)
{
    return 0;
}

void ls_dir(char *dir)
{
    vfs_node_t *target = cur;
    if (cur == 0)
    {
        trace("no disk mounted.\n");
        return;
    }

    target = path_to_node(dir);
    if (target == 0)
        return;

    if (!target->synced)
    {
        sync_node(target);
    }
    for (int idx = 0; idx < 64; idx++)
    {
        if (target->childs[idx] != 0)
        {
            stdoutput("%s  ", (target->childs[idx])->name);
        }
    }
    stdoutput("\n");
}

void cd_dir(char *path)
{
    vfs_node_t *target = path_to_node(path);
    if (!target->is_dir)
    {
        trace("can't cd to a file\n");
        return;
    }

    cur = target;
}

int vfs_read_file(char *path, u8 *buf)
{
    vfs_node_t *target = path_to_node(path);
    if (target->is_dir)
    {
        trace("can't read DIR\n");
        return 0;
    }
    int nz = read_inode_file(target, buf);
    return nz;
}

void vfs_pwd()
{
    if (cur == 0)
    {
        kprintf("no disk mounted. \n");
        return;
    }
    if (cur == &root)
    {
        kprintf("/\n");
        return;
    }

    vfs_node_t *node_stack[64];
    int idx = 0;
    vfs_node_t *vfs_node = cur;

    do
    {
        node_stack[idx] = vfs_node;
        idx++;
        vfs_node = vfs_node->parent;
    } while (vfs_node != &root);

    while (idx > 0)
    {
        idx--;
        kprintf("/%s", node_stack[idx]->name);
    }
    kprintln();
}
