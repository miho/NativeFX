#pragma once

/*
 * Copyright 2019-2019 Michael Hoffer <info@michaelhoffer.de>. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * If you use this software for scientific research then please cite the following publication(s):
 *
 * M. Hoffer, C. Poliwoda, & G. Wittum. (2013). Visual reflection library:
 * a framework for declarative GUI programming on the Java platform.
 * Computing and Visualization in Science, 2013, 16(4),
 * 181–192. http://doi.org/10.1007/s00791-014-0230-y
 */

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>

#define IPC_MSG_SIZE 4096
#define IPC_KEY_EVT_NUM_CHARS 8

#define IPC_NUM_NATIVE_EVT_TYPE_SIZE 128
#define IPC_NUM_NATIVE_EVT_MSG_SIZE 1024

#define IPC_NUM_EVT_MSGS 100

#define IPC_INFO_NAME "_info_"
#define IPC_BUFF_NAME "_buff_"
#define IPC_EVT_MQ_NAME "_evt_mq_"
#define IPC_EVT_MQ_NATIVE_NAME "_evt_mq_native_"

// instead of Qt stuff, we use plain c++ & boost
// for the client lib
// therefore, we need to declare uchar (was provided by qt before)
typedef unsigned char uchar;

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/offset_ptr.hpp>

#include <string>

#define LOCK_TIMEOUT 1000 // milliseconds 

namespace nativefx {

typedef boost::interprocess::basic_string<char> shared_string;

/**
 * Status indicates success as well as different types of errors.
 */
enum STATUS {
    NFX_SUCCESS          = 0,
    NFX_ERROR            = 1,
    NFX_CONNECTION_ERROR = 2,
    NFX_TIMEOUT_ERROR    = 4,
    NFX_ARGS_ERROR       = 8
};

enum MOUSE_BTN {
   NFX_NO_BTN        = 0,
   NFX_PRIMARY_BTN   = 1,
   NFX_SECONDARY_BTN = 2,
   NFX_MIDDLE_BTN    = 4
};

enum MODIFIER {
   NFX_NO_KEY      = 0,
   NFX_SHIFT_KEY   = 1,
   NFX_ALT_KEY     = 2,
   NFX_META_KEY    = 4,
   NFX_CONTROL_KEY = 8
};

enum EVENT_TYPE {
   NFX_NO_EVENT       = 0,
   NFX_EVENT          = 1,
   NFX_MOUSE_EVENT    = 2,
   NFX_MOUSE_MOVED    = 4,
   NFX_MOUSE_ENTERED  = 8,
   NFX_MOUSE_EXITED   = 16,
   NFX_MOUSE_RELEASED = 32,
   NFX_MOUSE_PRESSED  = 64,
   NFX_MOUSE_CLICKED  = 128,
   NFX_MOUSE_WHEEL    = 256,

   NFX_KEY_EVENT      = 512,
   NFX_KEY_PRESSED    = 1024,
   NFX_KEY_RELEASED   = 2048,
   NFX_KEY_TYPED      = 4096,

   NFX_REDRAW_EVENT   = 8192,
   NFX_TERMINATION_EVENT = 16384
};

struct event {
   int type       = 0;
   long timestamp = 0;
};

struct mouse_event {
   int type       = NFX_MOUSE_EVENT;
   long timestamp = 0;

   int buttons    = NFX_NO_BTN;
   int modifiers  = NFX_NO_KEY;
   int click_count= 0;
   double amount  = 0;
   double x       = 0;
   double y       = 0;
};

struct key_event {
   int type       = NFX_KEY_EVENT;
   long timestamp = 0;

   int modifiers  = NFX_NO_KEY;
   char chars[IPC_KEY_EVT_NUM_CHARS + 1]; // not initialized since it is not allowed
   int key_code   = 0;                    // 0 is defined as "unknown key"
};

struct redraw_event : event {
   int type = NFX_REDRAW_EVENT;
   long timestamp = 0; 

   double x       = 0;
   double y       = 0;
   double w       = 0;
   double h       = 0;
};

struct termination_event {
   int type       = NFX_TERMINATION_EVENT;
   long timestamp = 0;
};

/**
 * Event that is used to communicate events from native servers back to the
 * client Java API. It's intended to be used in a boost message queue. That's
 * why we don't use more complex types such as std::string etc.
 */
struct native_event {
    char type[IPC_NUM_NATIVE_EVT_TYPE_SIZE + 1];   // not initialized since it is not allowed
    char evt_msg[IPC_NUM_NATIVE_EVT_MSG_SIZE + 1]; // not initialized since it is not allowed
};

void store_shared_string(std::string str, char* str_to_store_to, size_t size) {
      // copy client_to_server_msg
      for(size_t idx = 0; idx < str.size();++idx) {
        str_to_store_to[idx] = str[idx];
      }
      // fill unused entries with '\0' 
      for(size_t idx = str.size(); idx < size + 1;++idx) {
        str_to_store_to[idx] = '\0';
      }
}

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

void store_key_codes(std::string str, char* str_to_store_to) {
      // copy client_to_server_msg
      for(size_t idx = 0; idx < str.size();++idx) {
        str_to_store_to[idx] = str[idx];
      }
      // fill unused entries with '\0' 
      for(size_t idx = str.size(); idx < IPC_KEY_EVT_NUM_CHARS + 1;++idx) {
        str_to_store_to[idx] = '\0';
      }
}

struct shared_memory_info {
   shared_memory_info()
      : img_buffer_size(0),
        w(1024), h(768), dirty(false), buffer_ready(true), 
        client_to_server_msg_semaphore(0),
        client_to_server_res_semaphore(0),
        buffer_semaphore(0) {
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
   key_event         k_event;
   
};


struct shared_memory_buffer
{

};

std::string get_info_name(int key, std::string name) {
  return name + IPC_INFO_NAME;
}

std::string get_evt_msg_queue_name(int key, std::string name) {
  return name + IPC_EVT_MQ_NAME;
}

std::string get_evt_msg_queue_native_name(int key, std::string name) {
  return name + IPC_EVT_MQ_NATIVE_NAME;
}

std::string get_buffer_name(int key, std::string name) {
  return name + IPC_BUFF_NAME;
}

boost::interprocess::message_queue* open_evt_mq(std::string evt_msg_queue_name) {

   boost::interprocess::message_queue * evt_msg_queue = new boost::interprocess::message_queue(
     boost::interprocess::open_only,       // only open (don't create)
     evt_msg_queue_name.c_str()            // name
   );

   return evt_msg_queue;
}

std::size_t max_event_message_size() {
   return  std::max({
     sizeof(event), 
     sizeof(mouse_event), 
     sizeof(key_event), 
     sizeof(redraw_event)
   });
}

boost::interprocess::message_queue* create_evt_mq(std::string evt_msg_queue_name) {

   // find the maximum event message size
   std::size_t max_evt_struct_size = std::max({
     sizeof(event), 
     sizeof(mouse_event), 
     sizeof(key_event), 
     sizeof(redraw_event)
   });

   boost::interprocess::message_queue * evt_msg_queue = new boost::interprocess::message_queue(
     boost::interprocess::create_only,     // only open (don't create)
     evt_msg_queue_name.c_str(),           // name
     IPC_NUM_EVT_MSGS,                     // max message number
     max_evt_struct_size                   // max message size
   );

   return evt_msg_queue;
}

boost::interprocess::message_queue* create_evt_mq_native(std::string evt_msg_queue_name) {

   // find the maximum event message size
   std::size_t max_evt_struct_size = sizeof(native_event); 


   boost::interprocess::message_queue * evt_msg_queue = new boost::interprocess::message_queue(
     boost::interprocess::create_only,     // only open (don't create)
     evt_msg_queue_name.c_str(),           // name
     IPC_NUM_EVT_MSGS,                     // max message number
     max_evt_struct_size                   // max message size
   );

   return evt_msg_queue;
}

} // end namespace nativefx
