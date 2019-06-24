#ifndef _Included_NativeFX_CPP
#define _Included_NativeFX_CPP

#include<vector>
#include<string>

#include <boost/thread/xtime.hpp>

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
  (JNIEnv *env, jclass cls, jint key, jstring jmsg) {

      shared_memory_info* info_data = NULL;

      if(key >= connections.size()) {
        return stringC2J(env, "ERROR: key not available");
      }

      info_data = connections[key];

      std::string msg = stringJ2C(env, jmsg);

      // send a message to server
      store_shared_string(msg, info_data->client_to_server_msg);
      info_data->client_to_server_msg_semaphore.post();

      // return result from server
      info_data->client_to_server_res_semaphore.wait();
      return stringC2J(env, info_data->client_to_server_res);
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
      std::string info_name = get_info_name(key, name);
      std::string buffer_name = get_buffer_name(key,name);
      names.push_back(name);

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

JNIEXPORT jboolean JNICALL Java_eu_mihosoft_nativefx_NativeBinding_isConnected
  (JNIEnv *env, jclass cls, jint key) {

    namespace ipc = boost::interprocess;
    // try
    // {
    //     if(key >= connections.size()) {
    //       return false;
    //     }

    //     ipc::shared_memory_object shm_info(ipc::open_only, get_info_name(key, names[key]).c_str(),
    //        ipc::read_only
    //     );

    //     return true;
    // } 
    // catch (const std::exception &ex) {
    //     //
    // }
    // return false;

    return boolC2J(key < connections.size() && connections[key] != NULL);
}

JNIEXPORT jboolean JNICALL Java_eu_mihosoft_nativefx_NativeBinding_terminate
  (JNIEnv *env, jclass cls, jint key) {

    namespace ipc = boost::interprocess;

    if(key >= connections.size()) {
      std::cerr << "ERROR: key not available" << std::endl;
      return false;
    }

    ipc::shared_memory_object::remove(get_info_name  (key, names[key]).c_str());
    ipc::shared_memory_object::remove(get_buffer_name(key, names[key]).c_str());

    names[key]          = ""; // NULL?

    delete connections[key];
    // delete buffers[key];
    delete shm_infos[key];
    delete info_regions[key];
    delete shm_buffers[key];
    delete buffer_regions[key];

    connections[key]    = NULL;
    buffers[key]        = NULL;
    shm_infos[key]      = NULL;
    info_regions[key]   = NULL;
    shm_buffers[key]    = NULL;
    buffer_regions[key] = NULL;

    return boolC2J(true);
}

JNIEXPORT jobject JNICALL Java_eu_mihosoft_nativefx_NativeBinding_getBuffer
  (JNIEnv *env, jclass cls, jint key) {

  if(key >= connections.size() || connections[key] == NULL) {
      std::cerr << "ERROR: key not available" << std::endl;
      return NULL;
  }

  void* buffer_addr = buffers[key];

  jobject result = env->NewDirectByteBuffer(buffer_addr, 1024*768*4);
  
  return result;
}

JNIEXPORT jint JNICALL Java_eu_mihosoft_nativefx_NativeBinding_getW
  (JNIEnv *env, jclass cls, jint key) {
  if(key >= connections.size() || connections[key] == NULL) {
      std::cerr << "ERROR: key not available" << std::endl;
      return -1;
  }

  return connections[key]->w;
}

JNIEXPORT jint JNICALL Java_eu_mihosoft_nativefx_NativeBinding_getH
  (JNIEnv *env, jclass cls, jint key) {
  if(key >= connections.size() || connections[key] == NULL) {
      std::cerr << "ERROR: key not available" << std::endl;
      return -1;
  }

  return connections[key]->h;
}

JNIEXPORT void JNICALL Java_eu_mihosoft_nativefx_NativeBinding_waitForBufferChanges
  (JNIEnv *env, jclass cls, jint key) {

  if(key >= connections.size() || connections[key] == NULL) {
      std::cerr << "ERROR: key not available" << std::endl;
  } else {
    
    // connections[key]->buffer_semaphore.try_wait();

    // try at least every second 
    // boost::system_time const timeout=
    //   boost::get_system_time()+ boost::posix_time::milliseconds(1000);
    // while(!connections[key]->buffer_semaphore.timed_wait(timeout)) {
    while(!connections[key]->buffer_semaphore.try_wait()) {  
      //
    }
  }

}


#endif /*_Included_NativeFX_CPP*/