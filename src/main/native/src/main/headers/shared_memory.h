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
   MOUSE_MOVED    = 1,
   MOUSE_PRESSED  = 2,
   MOUSE_RELEASED = 4,

   MOUSE_WHEEL    = 8,

   KEY_PRESSED    = 16,
   KEY_RELEASED   = 32,
   KEY_TYPED      = 64,

   REDRAW         = 128
};

struct mouse_event {

   mouse_event() : type(NO_EVENT), buttons(NO_BTN), modifiers(NO_KEY),
                   x(0),y(0),timestamp(0) {}

   int type;
   int buttons;
   int modifiers;
   double x;
   double y;
   long timestamp;
};

struct mouse_wheel_event {
   mouse_wheel_event() : type(NO_EVENT), buttons(NO_BTN), modifiers(NO_KEY),
                   amount(0),timestamp(0) {}
   int type;
   int buttons;
   int modifiers;
   double amount;
   long timestamp;
};

struct key_event {
   key_event() : type(NO_EVENT), modifiers(NO_KEY),
                   chars(""),timestamp(0) {}
   int type;
   int modifiers;
   char chars[128];
   long timestamp;
};

struct redraw_event {
   redraw_event() : type(NO_EVENT),
                   x(0),y(0),w(0),h(0),timestamp(0) {}
   int type;
   double x;
   double y;
   double w;
   double h;
   long timestamp;
};

struct shared_memory_info {
   shared_memory_info()
      : img_buffer_size(0),
        w(0), h(0), dirty(false), msg(""){//,
        //r_event(), m_event(), m_wheel_event(), k_event() {

   }

   //Mutex to protect access
   boost::interprocess::interprocess_mutex mutex;
   boost::interprocess::interprocess_mutex evt_mutex;
   boost::interprocess::interprocess_mutex buffer_mutex;

   int img_buffer_size;
   
   int w;
   int h;
   bool dirty;

   char msg[4096];
   
   //shared_string msg;

   redraw_event r_event;
   mouse_event  m_event;
   mouse_wheel_event m_wheel_event;
   key_event k_event;
   
};


struct shared_memory_buffer
{

};

#endif // SHARED_MEMORY_H
