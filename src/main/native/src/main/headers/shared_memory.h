#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/containers/string.hpp>

// instead of Qt stuff, we use plain c++ & boost
// for the client lib
// therefore, we need to declare uchar (was provided by qt before)
typedef unsigned char uchar;

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/offset_ptr.hpp>

#include <string>

typedef boost::interprocess::basic_string<char> shared_string;

struct shared_memory_info
{

   shared_memory_info()
      : img_buffer_size(0),
        w(0), h(0), msg("")
   {}

   //Mutex to protect access
   boost::interprocess::interprocess_mutex mutex;

   int img_buffer_size;
   
   int w;
   int h;

   char msg[4096];
   
   //shared_string msg;
   
};


struct shared_memory_buffer
{

};

#endif // SHARED_MEMORY_H
