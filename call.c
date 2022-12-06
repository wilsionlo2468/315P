#include "call.h"
#include <string.h>
const char *HD = "HD";

/*
inode* read_inode(int fd, int inode_number);
void print_inode_info(inode* ip);
void print_inode_region(int fd, int inode_number);
void print_dir_mappings(int fd, int inode_number);
*/
//{"/", "/dir5", "/dir5/dir1", "/dir5/dir1/file1", "/dir5/dir8/dir12/file2"};


int open_t(char *pathname){
	int inode_number = 0;
	// write your code here.
	int fd = open(HD, O_RDWR);


	char *copy = malloc(sizeof(char) * strlen(pathname));
	strcpy(copy,pathname);
	printf("pathname:%s\n" , copy);

	char delimitor[] = "/";
    char *part = strtok(copy , delimitor);

    if(part == NULL){//root path
        return 0;
    }

	int cur_inode = 0;
	DIR_NODE* p_block = (DIR_NODE* )malloc(BLK_SIZE);
    while(part != NULL){
        printf("str = %s , cur_inode = %d\n" , part , cur_inode);
		inode *cur_in = read_inode(fd,cur_inode);
		print_inode_info(cur_in);
		if(cur_in->f_type == 0){
			return cur_in->i_number; 
		}else{//go to next dir
			// Consider that SFS only supports at most 100 inodes so that only direct_blk[0] will be used,
			// the implementation is much easier
			int blk_num = cur_in->direct_blk[0];
			int currpos=lseek(fd, D_OFFSET + blk_num * BLK_SIZE, SEEK_SET);
			read(fd, p_block, BLK_SIZE);

			for(int file_idx = 0; file_idx < cur_in->sub_f_num; file_idx++){
				printf("the %d loop.... \n",file_idx);
				printf("%s \t\t %d\n", p_block[file_idx].f_name, p_block[file_idx].i_number);
				if(strcmp(part, p_block[file_idx].f_name) == 0){
					printf("inode found\n");
					cur_inode = p_block[file_idx].i_number;
					break;
				}
			}
		}
		part = strtok(NULL , delimitor);
		printf("str = %s , cur_inode = %d\n" , part , cur_inode);
	}//end while

	inode_number = cur_inode;
	free(p_block);
	free(copy);
	return inode_number;
}



int read_t(int i_number, int offset, void *buf, int count){
	int read_bytes = 0;
	// write your code here.

	//from inode.c read_inode
	int fd = open(HD, O_RDWR);
	inode* ip = malloc(sizeof(inode));
	int currpos=lseek(fd, I_OFFSET + i_number * sizeof(inode), SEEK_SET);
	if(currpos<0){
		printf("Error: lseek()\n");
	}
	//read inode from disk
	int ret = read(fd, ip, sizeof(inode));
	if(ret != sizeof (inode) ){
		printf("Error: read()\n");
	}
	//direct  0,1 part
	int block_start = offset / BLK_SIZE;
	int block_end = offset + count - 1 / BLK_SIZE ;
	int block_count = block_end - block_start + 1;
	
	while (block_count > 0){
		/* code */
		
		if(block_start < 2){
			//goto direct
			int currpos = lseek(fd, D_OFFSET + ip->direct_blk[block_start] * sizeof(BLK_SIZE), SEEK_SET);
			read_bytes += read(fd,buf,BLK_SIZE);
		}else{
			//indirect part
			int data_pointer = block_start - 2;
			int indirect_bpt = ip->indirect_blk;
			int datapos = lseek(fd, D_OFFSET + indirect_bpt * sizeof(BLK_SIZE), SEEK_SET);
			//data block size 4KB,1K pointer
			inode* indirect_block = malloc(sizeof(BLK_SIZE));
			// 4 bype for the block pointer
			int block_number = read(fd, indirect_block, sizeof(char) * 4 );
			int currpos = lseek(fd, D_OFFSET + block_number * sizeof(BLK_SIZE), SEEK_SET);
			//reading the data
			read_bytes += read(fd,buf,BLK_SIZE);
		}
		block_count--;
		block_start++;
	}

	


	return read_bytes; 
}


/*

functions

1.check pathname is file or diretory
	start from root,search inode
		if dir:
			set go to mapping
			go to inode of subpath
		if file:
			return data block;
2.return inode from mapping
3. 

superblock: define no.inode,datablock,size
inode: store all metadate of file or dir
*/
//lseek(fd , offset , whence) whence: set SEEK_SET = 0 ,SEEK_CUR = cur,SEEK_END = last

// you are allowed to create any auxiliary functions that can help your implementation. But only "open_t()" and "read_t()" are allowed to call these auxiliary functions.

/*
//{"/", "/dir5", "/dir5/dir1", "/dir5/dir1/file1", "/dir5/dir8/dir12/file2"};
int open_t(char *pathname)
{
	int inode_number;
	//if path == "/"(root) , not enter null
	char *copy = malloc(sizeof(char)  * strlen(pathname));
	strcpy(copy,pathname);
	printf("pathname:%s\n" , copy);

	
    char delimitor[] = "/";
    char *part = strtok(copy , delimitor);
    if(part == NULL){//root path
        inode_number = 0;
        return inode_number;
    }
    while(part != NULL){
        printf("%s\n" , part);
        part = strtok(NULL , delimitor);
    }
    

	return inode_number;
}
*/
