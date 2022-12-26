#include "dir.h"
#include "utils.h"
#include "trace.h"

void show_direntry(dir_entry_t *dir) {
    trace("==========dir entry============\n");
    trace("      inode:%d, rec_en:%d, type:%x, name:%s\n", 
        dir->inode, dir->rec_len, dir->file_type, dir->name);
    trace("==========dir entry end============\n");
}
