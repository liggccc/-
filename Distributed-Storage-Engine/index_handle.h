#ifndef DONGNAO_LARGEFILE_INDEXHANDLE_H_
#define DONGNAO_LARGEFILE_INDEXHANDLE_H_

#include "mmap_file_op.h"
#include "common.h"

namespace dongnao
{
  namespace largefile
  {
    struct IndexHeader
    {
      public:
        IndexHeader()
        {
          memset(this, 0, sizeof(IndexHeader));
        }

        BlockInfo block_info_; // meta block info
        //DirtyFlag flag_;               // dirty status flag
        int32_t bucket_size_;   // hash bucket size
        int32_t data_file_offset_; // offset to write next data in block(total data size of block)
        int32_t index_file_size_; // offset after: index_header + all buckets
        int32_t free_head_offset_; // free node list, for reuse

      private:
        DISALLOW_COPY_AND_ASSIGN(IndexHeader);
    };

    class IndexHandle
    {
      public:
        IndexHandle(const std::string& base_path, const uint32_t main_block_id);

        // for test
        IndexHandle(MMapFileOperation* mmap_op)
        {
          file_op_ = mmap_op;
          is_load_ = false;
        }
        ~IndexHandle();

        // create blockfile ,write index header and buckets info into the file
        int create(const uint32_t logic_block_id, const int32_t cfg_bucket_size, const MMapOption map_option);
        // load blockfile into memory, check block info
        int load(const uint32_t logic_block_id, const int32_t bucket_size, MMapOption map_option);
        // clear memory map, delete blockfile
        int remove(const uint32_t logic_block_id);
        // flush file to disk
        int flush();

        // find the next available key greater than key
        int find_avail_key(uint64_t& key);
        // update next available key
        void reset_avail_key(uint64_t key);


        // write meta into block, key must not be existed
        int write_segment_meta(const uint64_t key, MetaInfo& meta);
        // read meta info
        int read_segment_meta(const uint64_t key, MetaInfo& meta);
       
        // delete meta(key) from metainfo list, add it to free list
        int delete_segment_meta(const uint64_t key);


        // update block info after operation
        int update_block_info(const OperType oper_type, const uint32_t modify_size);

        int get_block_data_offset() const
        {
          return reinterpret_cast<IndexHeader*> (file_op_->get_map_data())->data_file_offset_;
        }

        void commit_block_data_offset(const int file_size)
        {
          reinterpret_cast<IndexHeader*> (file_op_->get_map_data())->data_file_offset_ += file_size;
        }

        IndexHeader* index_header()
        {
          return reinterpret_cast<IndexHeader*> (file_op_->get_map_data());
        }

        int32_t* bucket_slot()
        {
          return reinterpret_cast<int32_t*> (reinterpret_cast<char*> (file_op_->get_map_data()) + sizeof(IndexHeader));
        }

        int32_t bucket_size() const
        {
          return reinterpret_cast<IndexHeader*> (file_op_->get_map_data())->bucket_size_;
        }

        BlockInfo* block_info()
        {
          return reinterpret_cast<BlockInfo*> (file_op_->get_map_data());
        }

        int32_t data_file_size() const
        {
          return reinterpret_cast<IndexHeader*> (file_op_->get_map_data())->data_file_offset_;
        }

      private:
        bool hash_compare(const uint64_t left_key, const uint64_t right_key)
        {
          return (left_key == right_key);
        }

        int hash_find(const uint64_t key, int32_t& current_offset, int32_t& previous_offset);
        int hash_insert(const int32_t slot, const int32_t previous_offset, MetaInfo& meta);

      private:
        DISALLOW_COPY_AND_ASSIGN(IndexHandle);
        static const int32_t MAX_RETRY_TIMES = 3000;

      private:
        MMapFileOperation* file_op_;
        bool is_load_;
    };

  }
}
#endif //DONGNAO_LARGEFILE_INDEXHANDLE_H_
