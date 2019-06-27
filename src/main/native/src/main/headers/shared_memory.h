#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/containers/string.hpp>

#define IPC_MSG_SIZE 4096
#define IPC_KEY_EVT_NUM_CHARS 128

// instead of Qt stuff, we use plain c++ & boost
// for the client lib
// therefore, we need to declare uchar (was provided by qt before)
typedef unsigned char uchar;

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/offset_ptr.hpp>

#include <string>

typedef boost::interprocess::basic_string<char> shared_string;


#define LOCK_TIMEOUT 1000 // milliseconds 

enum MOUSE_BTN {
   NO_BTN        = 0,
   PRIMARY_BTN   = 1,
   SECONDARY_BTN = 2,
   MIDDLE_BTN    = 4
};

enum MODIFIER {
   NO_KEY      = 0,
   SHIFT_KEY   = 1,
   ALT_KEY     = 2,
   META_KEY    = 4,
};

enum EVENT_TYPE {
   NO_EVENT       = 0,
   EVENT          = 1,
   MOUSE_EVENT    = 2,
   MOUSE_MOVED    = 4,
   MOUSE_PRESSED  = 8,
   MOUSE_RELEASED = 16,

   MOUSE_WHEEL    = 32,

   KEY_EVENT      = 64,
   KEY_PRESSED    = 128,
   KEY_RELEASED   = 256,
   KEY_TYPED      = 512,

   REDRAW_EVENT   = 1024
};

struct event {
   int type       = {0};
   long timestamp = {0};
};

struct mouse_event {
   int type       = {MOUSE_EVENT};
   long timestamp = {0};

   int buttons    = {NO_BTN};
   int modifiers  = {NO_KEY};
   double x       = {0};
   double y       = {0};
};

struct mouse_wheel_event {
   int type       = {MOUSE_WHEEL};
   long timestamp = {0};

   int buttons    = NO_BTN;
   int modifiers  = NO_KEY;
   double amount  = {0};
};

struct key_event {
   int type       = {KEY_EVENT};
   long timestamp = {0};

   int modifiers  = {NO_KEY};
   char chars[IPC_KEY_EVT_NUM_CHARS + 1]; // not initialized since it is not allowed
};

struct redraw_event : event{
   int type = {REDRAW_EVENT};
   long timestamp = {0}; 

   double x       = {0};
   double y       = {0};
   double w       = {0};
   double h       = {0};
};

void store_shared_string(std::string str, char* str_to_store_to) {
      // copy client_to_server_msg
      for(size_t idx = 0; idx < str.size();++idx) {
        str_to_store_to[idx] = str[idx];
      }
      // fill unused entries with '\0' 
      for(size_t idx = str.size(); idx < IPC_MSG_SIZE + 1;++idx) {
        str_to_store_to[idx] = '\0';
      }
}

struct shared_memory_info {
   shared_memory_info()
      : img_buffer_size(0),
        w(1024), h(768), dirty(false), buffer_ready(true), 
        client_to_server_msg_semaphore(0),
        client_to_server_res_semaphore(0),
        buffer_semaphore(0) {//,
        //r_event(), m_event(), m_wheel_event(), k_event() {
   }

   // mutex to protect access
   boost::interprocess::interprocess_mutex mutex;
   // boost::interprocess::interprocess_mutex client_to_server_msg_mutex;
   // boost::interprocess::interprocess_mutex client_to_server_res_mutex;
   // boost::interprocess::interprocess_mutex evt_mutex;
   // boost::interprocess::interprocess_mutex buffer_mutex;

   boost::interprocess::interprocess_semaphore buffer_semaphore;

   boost::interprocess::interprocess_semaphore client_to_server_msg_semaphore;
   boost::interprocess::interprocess_semaphore client_to_server_res_semaphore;

   int img_buffer_size;
   
   int w;
   int h;
   bool dirty;
   bool buffer_ready;

   char client_to_server_msg[IPC_MSG_SIZE+1]; // not initialized since it is not allowed
   char client_to_server_res[IPC_MSG_SIZE+1]; // not initialized since it is not allowed
   
   char server_to_client_msg[IPC_MSG_SIZE+1]; // not initialized since it is not allowed
   char server_to_client_res[IPC_MSG_SIZE+1]; // not initialized since it is not allowed

   // shared_string msg;

   redraw_event      r_event;
   mouse_event       m_event;
   mouse_wheel_event m_wheel_event;
   key_event         k_event;
   
};


struct shared_memory_buffer
{

};

std::string get_info_name(int key, std::string name) {
  return name + "_info_";// + std::to_string(key);
}

std::string get_buffer_name(int key, std::string name) {
  return name + "_buff_";// + std::to_string(key);
}

#endif // SHARED_MEMORY_H
