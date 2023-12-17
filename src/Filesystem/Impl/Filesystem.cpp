//
// Created by Adam Makswiej on 10/12/2023.
//

#include "./Filesystem.hpp"
#include "./File.hpp"
#include "./Dir.hpp"

FileImplPtr Filesystem::Impl::Filesystem::open(const char *path, OpenMode openMode, AccessMode accessMode) {
    if (!_mounted) {
        return FileImplPtr{};
    }
    if (!path || !path[0]) {
        return FileImplPtr{};
    }
    if (!LittleFSImpl::pathValid(path)) {
        return FileImplPtr{};
    }

    int flags = _getFlags(openMode, accessMode);
    auto fd = std::make_shared<lfs_file_t>();

    if ((openMode & OM_CREATE) && strchr(path, '/')) {
        // For file creation, silently make subdirs as needed.  If any fail,
        // it will be caught by the real file open later on
        char *pathStr = strdup(path);
        if (pathStr) {
            // Make dirs up to the final fnamepart
            char *ptr = strchr(pathStr, '/');
            while (ptr) {
                *ptr = 0;
                lfs_mkdir(&_lfs, pathStr);
                *ptr = '/';
                ptr = strchr(ptr + 1, '/');
            }
        }
        free(pathStr);
    }

    time_t creation = 0;
    if (_timeCallback && (openMode & OM_CREATE)) {
        // O_CREATE means we *may* make the file, but not if it already exists.
        // See if it exists, and only if not update the creation time
        int rc = lfs_file_open(&_lfs, fd.get(), path, LFS_O_RDONLY);
        if (rc == 0) {
            lfs_file_close(&_lfs, fd.get()); // It exists, don't update create time
        } else {
            creation = _timeCallback(); // File didn't exist or otherwise, so we're going to create this time
        }
    }

    int rc = lfs_file_open(&_lfs, fd.get(), path, flags);
    if (rc == LFS_ERR_ISDIR) {
        // To support the SD.openNextFile, a null FD indicates to the LittleFSFile this is just
        // a directory whose name we are carrying around but which cannot be read or written
        return std::make_shared<Impl::File>(this, path, nullptr, flags, creation);
    } else if (rc == 0) {
        lfs_file_sync(&_lfs, fd.get());
        return std::make_shared<Impl::File>(this, path, fd, flags, creation);
    } else {
        DEBUGV(
            "LittleFSDirImpl::openFile: rc=%d fd=%p path=`%s` openMode=%d accessMode=%d err=%d\n",
            rc,
            fd.get(),
            path,
            openMode,
            accessMode,
            rc
            );
        return FileImplPtr{};
    }
}

DirImplPtr Filesystem::Impl::Filesystem::openDir(const char *path) {
    if (!_mounted || !path) {
        return DirImplPtr{};
    }
    char *pathStr = strdup(path); // Allow edits on our scratch copy
    // Get rid of any trailing slashes
    while (strlen(pathStr) && (pathStr[strlen(pathStr) - 1] == '/')) {
        pathStr[strlen(pathStr) - 1] = 0;
    }
    // At this point we have a name of "blah/blah/blah" or "blah" or ""
    // If that references a directory, just open it and we're done.
    lfs_info info;
    auto dir = std::make_shared<lfs_dir_t>();
    int rc;
    const char *filter = "";
    if (!pathStr[0]) {
        // openDir("") === openDir("/")
        rc = lfs_dir_open(&_lfs, dir.get(), "/");
        filter = "";
    } else if (lfs_stat(&_lfs, pathStr, &info) >= 0) {
        if (info.type == LFS_TYPE_DIR) {
            // Easy peasy, path specifies an existing dir!
            rc = lfs_dir_open(&_lfs, dir.get(), pathStr);
            filter = "";
        } else {
            // This is a file, so open the containing dir
            char *ptr = strrchr(pathStr, '/');
            if (!ptr) {
                // No slashes, open the root dir
                rc = lfs_dir_open(&_lfs, dir.get(), "/");
                filter = pathStr;
            } else {
                // We've got slashes, open the dir one up
                *ptr = 0; // Remove slash, truncate string
                rc = lfs_dir_open(&_lfs, dir.get(), pathStr);
                filter = ptr + 1;
            }
        }
    } else {
        // Name doesn't exist, so use the parent dir of whatever was sent in
        // This is a file, so open the containing dir
        char *ptr = strrchr(pathStr, '/');
        if (!ptr) {
            // No slashes, open the root dir
            rc = lfs_dir_open(&_lfs, dir.get(), "/");
            filter = pathStr;
        } else {
            // We've got slashes, open the dir one up
            *ptr = 0; // Remove slash, truncate string
            rc = lfs_dir_open(&_lfs, dir.get(), pathStr);
            filter = ptr + 1;
        }
    }
    if (rc < 0) {
        DEBUGV("LittleFSImpl::openDir: path=`%s` err=%d\n", path, rc);
        free(pathStr);
        return DirImplPtr{};
    }
    // Skip the . and .. entries
    lfs_info dirent;
    lfs_dir_read(&_lfs, dir.get(), &dirent);
    lfs_dir_read(&_lfs, dir.get(), &dirent);

    auto ret = std::make_shared<Dir>(filter, this, dir, pathStr);
    free(pathStr);
    return ret;
}

lfs_t * Filesystem::Impl::Filesystem::getRawFS() {
    return &_lfs;
}
