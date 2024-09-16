#ifndef RPC_H
#define RPC_H

enum {
	/* Server -> Client */
    RPC_SEND_FILES_LIST = 1,
	RPC_SEND_FILE,
	RPC_UPLOAD_FILE_FINISHED,
	
    /* Client -> Server */
	RPC_CHANGE_DIRECTORY,
    RPC_DOWNLOAD_FILE,
    RPC_UPLOAD_FILE,
    RPC_DELETE_FILE,
    RPC_CREATE_FILE,
    RPC_RENAME_FILE,
};

#endif // RPC_H
