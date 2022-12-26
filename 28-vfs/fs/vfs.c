#include "vfs.h"
#include "ext2.h"
#include "inode.h"
#include "fsmem.h"
#include "trace.h"

#define VS_NAME_LEN 64
#define VS_MAX_ITEMS 64

#define ROOT_INODE 2

typedef struct vfs_node{
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
} vfs_node_t ;

vfs_node_t root;
static vfs_node_t *cur = 0;

int read_inode_file(vfs_node_t *node, char *buf) {
    trace("read_inode_file, %d\n", node->inode_id);
    disk_t *disk = &node->disk;
    int inode = node->inode_id;
    int nz = get_inode_file(disk, inode, buf);
}

void sync_node(vfs_node_t *node) {
//    show_disk("sync_node: ", &node->disk);
    node->mode = get_inode_mode(&node->disk, node->inode_id);
    if (node->mode == INVALID_MODE) {
        trace("sync node failed. mode is invalid.\n");
        return;
    }
    node->is_file = is_inode_file(&node->disk, node->inode_id);
    node->is_dir =  is_inode_dir(&node->disk, node->inode_id);
    node->synced = 1;
    trace("sync node:%d, mode:%x, is_file:%d, is_dir:%x\n", 
        node->inode_id, node->mode, node->is_file, node->is_dir);
    if (node->is_dir) {
        int idx = 0;
        int ret_node = 1;
        do {
            char name[64];
            ret_node = get_next_dir_entry(&node->disk, node->inode_id, name);
            if (ret_node > 0) {
                trace("read dir entry id:%d.\n", ret_node);
                vfs_node_t *child = (vfs_node_t *)alloc_mem(sizeof(vfs_node_t));
                clear_mem((u8*)child, 0, sizeof(vfs_node_t));
                child->disk = node->disk;
                //show_disk("child disk: ", &child->disk);
                child->parent = node;
                node->childs[idx++] = child;
                move_str(child->name, name, 64);
                child->inode_id = ret_node;
                child->synced = 0;
            } else {
                trace("read dir entry failed.\n");
                break;
            }
        } while(ret_node > 0);
    }
}

int mount_root(disk_t *disk) {
//    show_disk("mount_root: ", disk);
    //enable_trace();
    trace("size of machine: le32: %d, le16: %d \n", 
        sizeof(__le32), sizeof(__le16));
    clear_mem(root.name, 0, VS_NAME_LEN);
    root.name[0] = '/';
    root.disk = *disk;
    root.inode_id = ROOT_INODE;
    root.synced = 0;
    root.parent = &root;
    for (int idx = 0; idx < VS_MAX_ITEMS; idx++) {
        root.childs[idx] = 0;
    }
    cur = &root;
    sync_node(cur);
    ls_cur_dir();
}

int mount(disk_t *disk, char *path) {
    return 0;
}

int umount(disk_t *disk) {
    return 0;
}

void ls_cur_dir() {
    trace("enter ls_cur_dir\n");
    if (cur ==0) {
        trace("no disk mounted.\n");
        return;
    }
    if (!cur->synced) {
        sync_node(cur);
    }
    for (int idx = 0; idx < 64; idx++) {
        if (cur->childs[idx] != 0) {
            stdoutput("%s  ", (cur->childs[idx])->name);
        }
    }
    stdoutput("\n");
}

void cd_dir(char *path) {
    if (0==cmp_str(path, "..", 2)) {
        cur = cur->parent;
        return;
    }

    for (int idx = 0; idx < 64; idx++) {
        if (cur->childs[idx] == 0)
            continue;
        if (0==cmp_str(cur->childs[idx]->name, path, 64)) {
            if (cur->childs[idx]->synced == 0) {
                sync_node(cur->childs[idx]);
                trace("sync done\n");
            }
            if (cur->childs[idx]->is_dir) {
                cur = cur->childs[idx];
                break;
            }
            else {
                trace("It is a file.\n");
                break;
            }
        }
    }
}

void more_file(char *path) {
    if ( path[0]=='.' ) {
        trace("DIR ./..\n");
        return;
    }

    for (int idx = 0; idx < 64; idx++) {
        if (cur->childs[idx] == 0)
            continue;
        if (0==cmp_str(cur->childs[idx]->name, path, 64)) {
            if (cur->childs[idx]->synced == 0) {
                sync_node(cur->childs[idx]);
                trace("more file sync node done\n");
            }
            if (cur->childs[idx]->is_dir) {
                trace("DIR ./..\n");
                return;
            }
            char buf[1024] = {0};
            int nz = read_inode_file(cur->childs[idx], buf);
            trace("file:%s, size:%d\n", path, nz);
            stdoutput("%s", buf);
        }
    }
}
