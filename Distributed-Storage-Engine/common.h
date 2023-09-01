#ifndef _COMMON_H_INCLUDED_
#define _COMMON_H_INCLUDED_


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <error.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <errno.h>
#include <assert.h>


//////////////////////////////////
#define DISALLOW_COPY_AND_ASSIGN(TypeName)      \
     TypeName(const TypeName&);                  \
     void operator=(const TypeName&)
  


namespace dongnao
{
  namespace largefile
  {  
  	static const int TFS_SUCCESS = 0;
    static const int TFS_ERROR = -1;
  	
  	const int32_t EXIT_BLOCKID_ZERO_ERROR = -8003; // block id is zero, fatal error
  	const int32_t EXIT_CREATE_FILEID_ERROR = -8008; // cat find unused fileid in limited times
  	const int32_t EXIT_BLOCKID_CONFLICT_ERROR = -8009; // block id conflict
  	const int32_t EXIT_DISK_OPER_INCOMPLETE = -8012; // read or write length is less than required;
  	const int32_t EXIT_INDEX_ALREADY_LOADED_ERROR = -8024; // index is loaded when create or load
    const int32_t EXIT_META_NOT_FOUND_ERROR = -8025; // meta not found in index
    const int32_t EXIT_META_UNEXPECT_FOUND_ERROR = -8026; // meta found in index when insert
    const int32_t EXIT_META_OFFSET_ERROR = -8027; // require offset is out of index size
    const int32_t EXIT_BUCKET_CONFIGURE_ERROR = -8028; // bucket size is conflict with before
    const int32_t EXIT_INDEX_UNEXPECT_EXIST_ERROR = -8029; // index already exist when create index
    const int32_t EXIT_INDEX_CORRUPT_ERROR = -8030; // index is corrupted, and index is created

	static const std::string MAINBLOCK_DIR_PREFIX = "/mainblock/";
    static const std::string EXTENDBLOCK_DIR_PREFIX = "/extend/";
    static const std::string INDEX_DIR_PREFIX = "/index/";
	static const mode_t DIR_MODE = 0755;

	enum OperType
    {
      C_OPER_INSERT = 1,
      C_OPER_DELETE
    };
  	
  	struct MMapOption
    {
      int32_t max_mmap_size_;
      int32_t first_mmap_size_;
      int32_t per_mmap_size_;
    };
    
    
    struct BlockInfo
    {
      uint32_t block_id_;
      int32_t version_;
      int32_t file_count_;
      int32_t size_;
      int32_t del_file_count_;
      int32_t del_size_;
      uint32_t seq_no_;

      BlockInfo()
      {
        memset(this, 0, sizeof(BlockInfo));
      }
      inline bool operator==(const BlockInfo& rhs) const
      {
        return block_id_ == rhs.block_id_ && version_ == rhs.version_ && file_count_ == rhs.file_count_ && size_
          == rhs.size_ && del_file_count_ == rhs.del_file_count_ && del_size_ == rhs.del_size_ && seq_no_
          == rhs.seq_no_;
      }
    };

  
    struct MetaInfo
    {
      public:
        MetaInfo()
        {
          init();
        }

        MetaInfo(const uint64_t file_id, const int32_t in_offset, const int32_t file_size, const int32_t next_meta_offset)
        {
          fileid_ = file_id;
          location_.inner_offset_ = in_offset;
          location_.size_ = file_size;
          next_meta_offset_ = next_meta_offset;
        }

        MetaInfo(const MetaInfo& meta_info)
        {
          memcpy(this, &meta_info, sizeof(MetaInfo));
        }

        /*explicit MetaInfo(const common::RawMeta& raw_meta)
        {
          raw_meta_ = raw_meta;
          next_meta_offset_ = 0;
        }*/

        uint64_t get_key() const
        {
          return fileid_;
        }

        void set_key(const uint64_t key)
        {
          fileid_ = key;
        }

        uint64_t get_file_id() const
        {
          return fileid_;
        }

        void set_file_id(const uint64_t file_id)
        {
          fileid_ = file_id;
        }

        int32_t get_offset() const
        {
          return location_.inner_offset_;
        }

        void set_offset(const int32_t offset)
        {
          location_.inner_offset_ = offset;
        }

        int32_t get_size() const
        {
          return location_.size_;
        }

        void set_size(const int32_t file_size)
        {
          location_.size_ = file_size;
        }


        int32_t get_next_meta_offset() const
        {
          return next_meta_offset_;
        }

        void set_next_meta_offset(const int32_t offset)
        {
          next_meta_offset_ = offset;
        }

        MetaInfo& operator=(const MetaInfo& meta_info)
        {
          if (this == &meta_info)
          {
            return *this;
          }
          fileid_ = meta_info.fileid_;
          location_.inner_offset_ = meta_info.location_.inner_offset_;
          location_.size_ = meta_info.location_.size_;
          next_meta_offset_ = meta_info.next_meta_offset_;
          return *this;
        }

        MetaInfo& clone(const MetaInfo& meta_info)
        {
          assert(this != &meta_info);
          fileid_ = meta_info.fileid_;
          location_.inner_offset_ = meta_info.location_.inner_offset_;
          location_.size_ = meta_info.location_.size_;
          next_meta_offset_ = meta_info.next_meta_offset_;
          return *this;
        }

        bool operator ==(const MetaInfo& rhs) const
        {
          return fileid_ == rhs.fileid_ && location_.inner_offset_ == rhs.location_.inner_offset_ && location_.size_ == rhs.location_.size_ && next_meta_offset_ == rhs.next_meta_offset_;
        }

        uint64_t fileid_;
        
        struct
        {
          int32_t inner_offset_;
          int32_t size_;
        } location_;
        
        int32_t next_meta_offset_;

      private:
        void init()
        {
          fileid_ = 0;
          location_.inner_offset_ = 0;
          location_.size_ = 0;
          next_meta_offset_ = 0;
        }
    };

	static const int32_t META_INFO_SIZE = sizeof(MetaInfo);
    
  }
  
}



#endif /* _COMMON_H_INCLUDED_ */
