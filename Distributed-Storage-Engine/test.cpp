#include "common.h"
#include "mmap_file_op.h"
#include "index_handle.h"
#include <unistd.h>
#include <sstream>
#include <iostream>

using namespace  dongnao;
using namespace  std;

#define BUF_SIZE   1024
#define FILE_NUM   (1024*100)

const static largefile::MMapOption mmap_option={10240000,4096,1024}; //内存映射的参数
const static uint32_t main_blocksize = 1024*1024*100;//大文件的大小
static uint32_t bucket_size = 100;  //
static int32_t block_id = 1;

static int debug = 0;


int main(int argc, char **argv){

  
  int32_t ch = 0,ret = 0;
  char * write_buf=NULL;
	 
  write_buf = (char *)malloc(BUF_SIZE);
  memset(write_buf,'8',BUF_SIZE);

  largefile::IndexHandle * index_handle_ = new largefile::IndexHandle(".", block_id);//索引的句柄

  std::stringstream tmp_stream;
  tmp_stream <<"."<<largefile::MAINBLOCK_DIR_PREFIX << block_id;
  std::string mainblock_path;
  tmp_stream >> mainblock_path;
  std::cout<<"path: "<<mainblock_path<<std::endl;
  //./mainblock/1	
  largefile::FileOperation * mainblock = new largefile::FileOperation(mainblock_path,O_RDWR | O_LARGEFILE | O_CREAT);


  
  if(argc >=2 && strncasecmp("init",argv[1],4)==0){//init
    printf("init .....\n");
    ret = index_handle_->create(block_id, bucket_size, mmap_option);
	if(ret != largefile::TFS_SUCCESS){
      fprintf(stderr,"create index %d failed.\n",block_id);
	  delete index_handle_;
	  return -1;
	}
	
	ret = mainblock->ftruncate_file(main_blocksize);
	if(ret != 0){
      fprintf(stderr,"create main block %s failed.reason:%s\n",mainblock_path.c_str(),strerror(errno));
	  index_handle_->remove(block_id);
	  delete mainblock;
	  return -1;
    }

	
    mainblock->close_file();
    index_handle_->flush();

    delete mainblock;
    delete index_handle_;
	
    return 0;
  }


  {//
      int32_t data_offset=0;
      uint32_t write_len=0;

      ret=index_handle_->load(block_id, bucket_size, mmap_option);//
      if(ret!=largefile::TFS_SUCCESS){
	    fprintf(stderr,"load failed. block_id :%d\n",block_id);
	    mainblock->close_file();
	    
	    delete mainblock;
	    delete index_handle_;
    }
    
    for(int i=0;i<FILE_NUM;i++)
    {
	data_offset = index_handle_->get_block_data_offset();
	write_len = BUF_SIZE;
        uint32_t file_no = index_handle_->block_info()->seq_no_;

	if((ret=mainblock->pwrite_file(write_buf, write_len, data_offset)) != largefile::TFS_SUCCESS){
            fprintf(stderr,"write to main block failed. ret:%d, reason: %s\n",ret,strerror(errno));

	    mainblock->close_file();
	
	    delete mainblock;
	    delete index_handle_;
	    return -1;
	}

	largefile::MetaInfo meta;
	meta.set_file_id(file_no);
	meta.set_offset(data_offset);
	meta.set_size(write_len);
	ret = 0;
	
	index_handle_->commit_block_data_offset(write_len);//
        if(index_handle_->write_segment_meta(meta.get_key(), meta)==largefile::TFS_SUCCESS){
	    index_handle_->update_block_info(largefile::C_OPER_INSERT, write_len);
	  
	    if(index_handle_->flush()!=largefile::TFS_SUCCESS){
                fprintf(stderr,"flush mainblock %d failed. file no:%d \n",block_id,file_no);
                ret = -1;
	    }

        }else{
             fprintf(stderr,"write_segment_meta  mainblock %d failed. file no:%d \n",block_id,file_no);
             ret = -1;
        }

    
        if(ret == -1){
            fprintf(stderr,"write to mainblock %d failed. file no:%d \n",block_id,file_no);
        }else if(debug) printf("file no: %d  write  successfully.\n",file_no);
    }

    mainblock->close_file();
	    
    delete mainblock;
    delete index_handle_;
  }

  return ret;
}



