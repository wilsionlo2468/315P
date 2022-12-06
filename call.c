@ -27,7 +27,6 @@ int open_t(char *pathname){
    if(part == NULL){//root path
        return 0;
    }
		printf("=============\n");

	int cur_inode = 0;
	DIR_NODE* p_block = (DIR_NODE* )malloc(BLK_SIZE);
@ -67,8 +66,50 @@ int open_t(char *pathname){


int read_t(int i_number, int offset, void *buf, int count){
	int read_bytes;
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

	



