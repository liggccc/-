
#ifndef DONGNAO_LARGEFILE_MMAPFILE_OP_H_
#define DONGNAO_LARGEFILE_MMAPFILE_OP_H_

#include "file_op.h"
#include "mmap_file.h"
#include "common.h"


namespace dongnao
{
  namespace largefile
  {
    
    class MMapFileOperation: public FileOperation
    {
      public:
        explicit MMapFileOperation(const std::string& file_name, int open_flags = O_CREAT | O_RDWR | O_LARGEFILE) :
          FileOperation(file_name, open_flags), is_mapped_(false), map_file_(NULL)
        {

        }

        ~MMapFileOperation()
        {
          if(is_mapped_){ 
            delete(map_file_);
          }
        }

        int pread_file(char* buf, const int32_t size, const int64_t offset);
        int pwrite_file(const char* buf, const int32_t size, const int64_t offset);

        int mmap_file(const MMapOption& mmap_option);
        int munmap_file();
        void* get_map_data() const;
        int flush_file();

      private:
        MMapFileOperation();
        DISALLOW_COPY_AND_ASSIGN(MMapFileOperation);

        bool is_mapped_;
        MMapFile* map_file_;
    };

  }
}
#endif //DONGNAO_BIGFILE_MMAPFILE_OP_H_
