#ifndef _Included_NativeFX_CPP
#define _Included_NativeFX_CPP

#include<vector>
#include<string>

#include "eu_mihosoft_nativefx_NativeBinding.h"
#include "jnitypeconverter.h"

#include "shared_memory.h"

std::vector<std::string> names;
std::vector<shared_memory_info*>   connections;
std::vector<void*> buffers;

std::vector<boost::interprocess::shared_memory_object*> shm_infos;
std::vector<boost::interprocess::mapped_region*> info_regions;
std::vector<boost::interprocess::shared_memory_object*> shm_buffers;
std::vector<boost::interprocess::mapped_region*> buffer_regions;

JNIEXPORT jstring JNICALL Java_eu_mihosoft_nativefx_NativeBinding_sendMsg
  (JNIEnv *env, jclass cls, jint key, jstring msg) {

      // TODO implement msg sending via shared memory

      return stringC2J(env, "hello from native!");
}

JNIEXPORT jint JNICALL Java_eu_mihosoft_nativefx_NativeBinding_nextKey
  (JNIEnv *env, jclass cls) {
    return connections.size();
}

JNIEXPORT jint JNICALL Java_eu_mihosoft_nativefx_NativeBinding_connectTo
  (JNIEnv *env, jclass cls, jstring jname) {

      using namespace boost::interprocess;
      std::string name = stringJ2C(env, jname);

      // setup key and names for new connection
      int key = connections.size();
      std::string info_name = name + "_info_" + std::to_string(key);
      std::string buffer_name = name + "_buffer_" + std::to_string(key);

      try {

        // open the shared memory object.
        shared_memory_object* shm_info = 
          new shared_memory_object(open_only,                // only open (don't create)
            info_name.c_str(),                               // name
            read_write                                       // read-write mode
        );

        shm_infos.push_back(shm_info);

        // map the whole shared memory in this process
        mapped_region* info_region = new mapped_region
                (*shm_info,                       // What to map
                  read_write                      // Map it as read-write
        );

        info_regions.push_back(info_region);

        // get the address of the mapped region
        void * info_addr = info_region->get_address();

        // construct the shared structure in memory
        shared_memory_info * info_data = static_cast<shared_memory_info*>(info_addr);
        connections.push_back(info_data);

        info_data->mutex.lock();

        // create a shared memory object.
        shared_memory_object* shm_buffer = 
          new shared_memory_object
                (open_only,             // only open
                 buffer_name.c_str(),   // name
                 read_write             // read-write mode
        );

        shm_buffers.push_back(shm_buffer);

        // map the whole shared memory in this process
        mapped_region* buffer_region = 
          new mapped_region(
                 *shm_buffer,    // What to map
                  read_write     // Map it as read-write
        );

        buffer_regions.push_back(buffer_region);

        // get the address of the mapped region
        void* buffer_addr = buffer_region->get_address();

        buffers.push_back(buffer_addr);

        info_data->mutex.unlock();
      } catch(...) {

        std::cerr << "ERROR: cannot connect to '" << info_name << "'. Server probably not running." << std::endl;

        return -1;
      }

      return key;
}


#endif /*_Included_NativeFX_CPP*/