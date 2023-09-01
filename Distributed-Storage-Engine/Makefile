cc = g++ -g 

largefile :test.o file_op.o mmap_file.o mmap_file_op.o  index_handle.o
					g++ -o hash_index test.o file_op.o mmap_file.o mmap_file_op.o  index_handle.o

    test.o : test.cpp common.h mmap_file_op.h index_handle.h

    file_op.o : file_op.cpp common.h file_op.h 
		$(cc) -c file_op.cpp
    mmap_file.o : mmap_file.cpp common.h mmap_file.h
		$(cc) -c mmap_file.cpp
    mmap_file_op.o : mmap_file_op.cpp common.h mmap_file_op.h
		$(cc) -c mmap_file_op.cpp
    index_handle.o : index_handle.cpp common.h index_handle.h
		$(cc) -c index_handle.cpp
    clean :
		rm hash_index file_op.o mmap_file.o mmap_file_op.o  index_handle.o