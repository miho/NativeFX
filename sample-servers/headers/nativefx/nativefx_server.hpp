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


// ---------------------------------------- VERSION ----------------------------------------

/// major version number
#define NATIVEFX_VERSION_MAJOR 0

/// minor version number
#define NATIVEFX_VERSION_MINOR 1

/// patch number
#define NATIVEFX_VERSION_PATCH 0

/// complete version number
#define NATIVEFX_VERSION_CODE (NATIVEFX_VERSION_MAJOR * 10000 + NATIVEFX_VERSION_MINOR * 100 + NATIVEFX_VERSION_PATCH)

/// version number as string
#define NATIVEFX_VERSION_STRING "0.1.0"

// ---------------------------------------- INCLUDES ----------------------------------------

#include <iostream>
#include <chrono>
#include <thread>

// force boost to be included as header only, also on windows
#define BOOST_ALL_NO_LIB 1

#include <boost/thread/xtime.hpp>

#define _USE_MATH_DEFINES
#include <cmath>

// in case M_PI hasn't been defined we do it manually
// see https://github.com/miho/NativeFX/issues/12
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // M_PI

#include "args/args.hxx"
#include "shared_memory.h"

// ---------------------------------------- NATIVEFX CODE ----------------------------------------

namespace nativefx {

namespace ipc  = boost::interprocess;

ipc::shared_memory_object shm_buffer;
ipc::mapped_region buffer_region;
ipc::message_queue* evt_mq;

// deprecated
uchar* create_shared_buffer(std::string buffer_name, int w, int h) {

    // create the shared memory buffer object.
    shm_buffer = ipc::shared_memory_object (
                ipc::create_only,
                buffer_name.c_str(),
                ipc::read_write
    );

    // set the size of the shared image buffer (w*h*#channels*sizeof(uchar))
    shm_buffer.truncate( w*h // TODO properly resize shared memory
                        */*#channels*/4
                        */*channel size*/sizeof(uchar)
    );

    // map the shared memory buffer object in this process
    buffer_region = ipc::mapped_region(
                shm_buffer,
                ipc::read_write
    );

    // get the address of the shared image buffer
    void * buffer_addr = buffer_region.get_address();

    // cast shared memory pointer to correct uchar type
    uchar* buffer_data = (uchar*) buffer_addr;

    return buffer_data;
}

typedef  std::function<void (std::string const &name, uchar* buffer_data, int W, int H)> redraw_callback;
typedef  std::function<void (std::string const &name, event* evt)> event_callback;

/**
 * Test function to verify buffer data functionality.
 */
void set_rgba(uchar* buffer_data, int buffer_w, int buffer_h, int x, int y, uchar r, uchar g, uchar b, uchar a) {
    buffer_data[y*buffer_w*4+x*4+0] = b; // B
    buffer_data[y*buffer_w*4+x*4+1] = g; // G
    buffer_data[y*buffer_w*4+x*4+2] = r; // R
    buffer_data[y*buffer_w*4+x*4+3] = a; // A
}

/**
 * Deletes pre-existing shared memory objects.
 * 
 * @param name name of the shared memory object to delete
 * @return status of this operation
 */
int delete_shared_mem(std::string const &name) {

    std::string info_name          = name + IPC_INFO_NAME;
    std::string buffer_name        = name + IPC_BUFF_NAME;
    std::string evt_mq_name        = name + IPC_EVT_MQ_NAME;
    std::string evt_mq_native_name = name + IPC_EVT_MQ_NATIVE_NAME;

    ipc::shared_memory_object::remove(info_name.c_str());
    ipc::shared_memory_object::remove(buffer_name.c_str());
    ipc::message_queue::remove(evt_mq_name.c_str());
    ipc::message_queue::remove(evt_mq_native_name.c_str());

    std::cout << "> deleted shared-mem" <<std::endl;
    std::cout << "  -> name:   " << name<<std::endl;

    return NFX_SUCCESS;
}

class shared_canvas final {

    private: 

        std::string name;

        std::string info_name;
        std::string buffer_name;
        std::string evt_mq_name;
        std::string evt_mq_native_name;
        
        uchar* buffer_data;
        shared_memory_info* info_data;
        ipc::shared_memory_object* shm_info;
        ipc::message_queue* evt_mq;
        ipc::message_queue* evt_mq_native;
        void* evt_mq_msg_buff;
        STATUS status;
        int W;
        int H;

        std::size_t MAX_SIZE;

        shared_canvas( std::string const &name, uchar* buffer_data, ipc::shared_memory_object* shm_info, shared_memory_info* info_data, ipc::message_queue* evt_mq, void* evt_mq_msg_buff, ipc::message_queue* evt_mq_native, int w, int h, STATUS status) {
            this->name            = name;
            this->buffer_data     = buffer_data;
            this->shm_info        = shm_info;
            this->info_data       = info_data;
            this->evt_mq          = evt_mq;
            this->evt_mq_msg_buff = evt_mq_msg_buff;
            this->evt_mq_native   = evt_mq_native;

            this->W               = w;
            this->H               = h;

            this->status          = status;


            this->info_name          = name + IPC_INFO_NAME;
            this->buffer_name        = name + IPC_BUFF_NAME;
            this->evt_mq_name        = name + IPC_EVT_MQ_NAME;
            this->evt_mq_native_name = name + IPC_EVT_MQ_NAME;

            this->MAX_SIZE           = max_event_message_size();
        }

    public:

        static shared_canvas* create(std::string const &name) {

            std::string info_name = name + IPC_INFO_NAME;
            std::string buffer_name = name + IPC_BUFF_NAME;
            std::string evt_mq_name = name + IPC_EVT_MQ_NAME;
            std::string evt_mq_native_name = name + IPC_EVT_MQ_NATIVE_NAME;

            std::cout << "> creating shared-mem" <<std::endl;
            std::cout << "  -> name:   " << name<<std::endl;

            ipc::shared_memory_object* shm_info;
            ipc::message_queue* evt_mq;
            ipc::message_queue* evt_mq_native;

            try {

                // create the shared memory info object.
                shm_info = new ipc::shared_memory_object(
                            ipc::create_only,
                            info_name.c_str(),
                            ipc::read_write
                );

                evt_mq = create_evt_mq(evt_mq_name);
                evt_mq_native = create_evt_mq_native(evt_mq_native_name);
            } catch(ipc::interprocess_exception const & ex) {

                    // remove shared memory objects
                    ipc::shared_memory_object::remove(info_name.c_str());
                    ipc::shared_memory_object::remove(buffer_name.c_str());
                    ipc::message_queue::remove(evt_mq_name.c_str());

                    std::cout << "> deleted pre-existing shared-mem" <<std::endl;
                    std::cout << "  -> name:   " << name<<std::endl;

                    // create the shared memory info object.
                    shm_info = new ipc::shared_memory_object(
                                ipc::create_only,
                                info_name.c_str(),
                                ipc::read_write
                    );

                    evt_mq        = create_evt_mq(evt_mq_name);
                    evt_mq_native = create_evt_mq_native(evt_mq_native_name);

                    std::cout << "> created shared-mem"  << std::endl;
                    std::cout << "  -> name:   " << name << std::endl;
            }

            // set the shm size
            shm_info->truncate(sizeof(struct shared_memory_info));

            // map the shared memory info object in this process
            ipc::mapped_region* info_region = new ipc::mapped_region(*shm_info, ipc::read_write);

            // get the adress of the info object
            void* info_addr = info_region->get_address();

            // construct the shared structure in memory
            shared_memory_info* info_data = new (info_addr) shared_memory_info;

            // init c-strings of info_data struct
            strcpy(info_data->client_to_server_msg, "");
            strcpy(info_data->client_to_server_res, "");
            strcpy(info_data->server_to_client_msg, "");
            strcpy(info_data->server_to_client_res, "");

            int W = info_data->w;
            int H = info_data->h;

            uchar* buffer_data = create_shared_buffer(buffer_name, W, H);

            double full = W*H;

            std::size_t MAX_SIZE = max_event_message_size();
            void* evt_mq_msg_buff = malloc(MAX_SIZE);

            return new shared_canvas(name, buffer_data, shm_info, info_data, evt_mq, evt_mq_msg_buff, evt_mq_native, W, H, NFX_SUCCESS);
        }

        int terminate(std::string const &name) {
            return delete_shared_mem(name);
        }

        bool is_buffer_ready() {
            // timed locking of resources
            boost::system_time const timeout=
            boost::get_system_time()+ boost::posix_time::milliseconds(LOCK_TIMEOUT);
            bool locking_success = info_data->mutex.timed_lock(timeout);

            if(!locking_success) {
                std::cerr << "[" + name + "] " << "ERROR: cannot connect to '" << name << "':" << std::endl;
                std::cerr << " -> But we are unable to lock the resources." << std::endl;
                std::cerr << " -> Client not running?." << std::endl;
                return false;
            }

            bool result = info_data->buffer_ready;

            info_data->mutex.unlock();

            return result;
        }

        int draw(redraw_callback redraw, redraw_callback resized) {

            // timed locking of resources
            boost::system_time const timeout=
            boost::get_system_time()+ boost::posix_time::milliseconds(LOCK_TIMEOUT);
            bool locking_success = info_data->mutex.timed_lock(timeout);

            if(!locking_success) {
                std::cerr << "[" + name + "] " << "ERROR: cannot connect to '" << name << "':" << std::endl;
                std::cerr << " -> But we are unable to lock the resources." << std::endl;
                std::cerr << " -> Client not running?." << std::endl;
                return NFX_ERROR | NFX_CONNECTION_ERROR;
            }

            bool is_dirty = info_data->dirty;
            // if still is dirty it means that the client hasn't drawn the previous
            // frame. in this case we just wait with updating the buffer until the
            // client draws the content.
            if(is_dirty) { 
                info_data->mutex.unlock();
                // continue; we still might want to process events
            } else {

                redraw(name, buffer_data, W, H);

                info_data->dirty = true;

                int new_W = info_data->w;
                int new_H = info_data->h;

                if(new_W!=W || new_H != H) {

                    // trigger buffer resize

                    W = new_W;
                    H = new_H;

                    std::cout << "[" + name + "]" << "> resize to W: " << W << ", H: " << H << std::endl;

                    ipc::shared_memory_object::remove(buffer_name.c_str());
                    buffer_data = create_shared_buffer(buffer_name, W, H);
                    info_data->buffer_ready = true;

                    resized(name, buffer_data, W, H);
                }

                info_data->mutex.unlock();
            }

            return NFX_SUCCESS;
        }

        void send_native_event(std::string type, std::string evt) {
            // process events
            ipc::message_queue::size_type recvd_size;
            unsigned int priority;

            // timed locking of resources
            boost::system_time const timeout=
            boost::get_system_time() + boost::posix_time::milliseconds(LOCK_TIMEOUT);

            native_event nevt;

            store_shared_string(type,     nevt.type,     IPC_NUM_NATIVE_EVT_TYPE_SIZE);
            store_shared_string(evt,      nevt.evt_msg,  IPC_NUM_NATIVE_EVT_MSG_SIZE );

            bool result = evt_mq->timed_send(&nevt, sizeof(native_event), priority, timeout);

            if(!result) {
                std::cerr << "[" + name + "] ERROR: can't send messages, message queue not accessible." << std::endl; 
            }
        }

        void process_events(event_callback events) {
            // process events
            ipc::message_queue::size_type recvd_size;
            unsigned int priority;

            while(evt_mq->get_num_msg() > 0) {

                // timed locking of resources
                boost::system_time const timeout=
                boost::get_system_time() + boost::posix_time::milliseconds(LOCK_TIMEOUT);

                bool result = evt_mq->timed_receive(evt_mq_msg_buff, MAX_SIZE, recvd_size, priority, timeout);

                if(!result) {
                    std::cerr << "[" + name + "] ERROR: can't read messages, message queue not accessible." << std::endl; 
                }

                event* evt = static_cast<event*>(evt_mq_msg_buff);

                events(name, evt);
                
            } // end while has event messages
        }

        int get_status() {
            return this->status;
        }

        bool is_valid() {
            return this->status == NFX_SUCCESS;
        }
    

};

/**
 * Starts a NativeFX server.
 * 
 * @param name name of the shared memory object to delete
 * @param redraw should contain the draw commands (is called repetitively)
 * @param events should contain the event handling (is called repetitively whenever events are to be processed)
 * @return status of this operation
 */
int start_server(std::string const &name, redraw_callback redraw, event_callback events)
{
    std::string info_name = name + IPC_INFO_NAME;
    std::string buffer_name = name + IPC_BUFF_NAME;
    std::string evt_mq_name = name + IPC_EVT_MQ_NAME;

    std::cout << "> creating shared-mem" <<std::endl;
    std::cout << "  -> name:   " << name<<std::endl;

    ipc::shared_memory_object shm_info;

    try {

        // create the shared memory info object.
        shm_info = ipc::shared_memory_object(
                    ipc::create_only,
                    info_name.c_str(),
                    ipc::read_write
        );

        evt_mq = create_evt_mq(evt_mq_name);
    } catch(ipc::interprocess_exception const & ex) {

            // remove shared memory objects
            ipc::shared_memory_object::remove(info_name.c_str());
            ipc::shared_memory_object::remove(buffer_name.c_str());
            ipc::message_queue::remove(evt_mq_name.c_str());

            std::cout << "> deleted pre-existing shared-mem" <<std::endl;
            std::cout << "  -> name:   " << name<<std::endl;

            // create the shared memory info object.
            shm_info = ipc::shared_memory_object(
                        ipc::create_only,
                        info_name.c_str(),
                        ipc::read_write
            );
            evt_mq = create_evt_mq(evt_mq_name);

            std::cout << "> created shared-mem"  << std::endl;
            std::cout << "  -> name:   " << name << std::endl;
    }

    // set the shm size
    shm_info.truncate(sizeof(struct shared_memory_info));

    // map the shared memory info object in this process
    ipc::mapped_region info_region(shm_info, ipc::read_write);

    // get the adress of the info object
    void* info_addr = info_region.get_address();

    // construct the shared structure in memory
    shared_memory_info* info_data = new (info_addr) shared_memory_info;

    // init c-strings of info_data struct
    strcpy(info_data->client_to_server_msg, "");
    strcpy(info_data->client_to_server_res, "");
    strcpy(info_data->server_to_client_msg, "");
    strcpy(info_data->server_to_client_res, "");

    int W = info_data->w;
    int H = info_data->h;

    uchar* buffer_data = create_shared_buffer(buffer_name, W, H);

    double full = W*H;

    std::size_t MAX_SIZE = max_event_message_size();
    void* evt_mq_msg_buff = malloc(MAX_SIZE);

    int counter = 0;
    while(true) {

        // timed locking of resources
        boost::system_time const timeout=
        boost::get_system_time()+ boost::posix_time::milliseconds(LOCK_TIMEOUT);
        bool locking_success = info_data->mutex.timed_lock(timeout);

        if(!locking_success) {
            std::cerr << "[" + info_name + "] " << "ERROR: cannot connect to '" << info_name << "':" << std::endl;
            std::cerr << " -> But we are unable to lock the resources." << std::endl;
            std::cerr << " -> Client not running?." << std::endl;
            return NFX_ERROR | NFX_CONNECTION_ERROR;
        }

        bool is_dirty = info_data->dirty;
        // if still is dirty it means that the client hasn't drawn the previous
        // frame. in this case we just wait with updating the buffer until the
        // client draws the content.
        if(is_dirty) { 
            info_data->mutex.unlock();
            // continue; we still might want to process events
        } else {
            redraw(name, buffer_data, W, H);

            info_data->dirty = true;

            int new_W = info_data->w;
            int new_H = info_data->h;

            if(new_W!=W || new_H != H) {

                // trigger buffer resize

                W = new_W;
                H = new_H;

                std::cout << "[" + info_name + "]" << "> resize to W: " << W << ", H: " << H << std::endl;

                ipc::shared_memory_object::remove(buffer_name.c_str());
                buffer_data = create_shared_buffer(buffer_name, W, H);
                info_data->buffer_ready = true;
            }

            info_data->mutex.unlock();
        }

        // process events
        ipc::message_queue::size_type recvd_size;
        unsigned int priority;

        while(evt_mq->get_num_msg() > 0) {

            // timed locking of resources
            boost::system_time const timeout=
            boost::get_system_time() + boost::posix_time::milliseconds(LOCK_TIMEOUT);

            bool result = evt_mq->timed_receive(evt_mq_msg_buff, MAX_SIZE, recvd_size, priority, timeout);

            if(!result) {
                std::cerr << "[" + info_name + "] ERROR: can't read messages, message queue not accessible." << std::endl; 
            }

            event* evt = static_cast<event*>(evt_mq_msg_buff);

            events(name, evt);
            
        } // end while has event messages

    } // end while true

    // remove shared memory objects
    ipc::shared_memory_object::remove(info_name.c_str());
    ipc::shared_memory_object::remove(buffer_name.c_str());
    ipc::message_queue::remove(evt_mq_name.c_str());

    free(evt_mq_msg_buff);

    return NFX_SUCCESS;
}

/**
 * Starts a NativeFX server. It parses the specified CLI arguments and performs the requested commands.
 * 
 * @param name name of the shared memory object to delete
 * @param argc number of CLI arguments
 * @param argv CLI arguments
 * @param redraw should contain the draw commands (is called repetitively)
 * @param events should contain the event handling (is called repetitively whenever events are to be processed)
 * @return status of this operation
 */
int start_server(int argc, char *argv[], redraw_callback redraw, event_callback events)
{
    args::ArgumentParser parser("This is a NativeFX server program.", "---");
    args::HelpFlag helpArg(parser, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<std::string> nameArg(parser, "name", "Defines the name of the shared memory objects to be created by this program", {'n', "name"});
    args::Flag deleteSharedMem(parser, "delete", "Indicates that existing shared memory with the specified name should be deleted", {'d', "delete"});

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (const args::Completion& e)
    {
        std::cout << e.what();
        return NFX_SUCCESS;
    }
    catch (const args::Help&)
    {
        std::cerr << parser;
        return NFX_ERROR | NFX_ARGS_ERROR;
    }
    catch (const args::ParseError& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return NFX_ERROR | NFX_ARGS_ERROR;
    }

    std::string name = args::get(nameArg);

    if(name.size() == 0) {
        std::cerr << std::endl << std::endl << "ERROR: 'name' must be specified to create or delete shared memory!" << std::endl << std::endl;
        std::cerr << parser;
        return NFX_ERROR | NFX_ARGS_ERROR;
    }

    if(deleteSharedMem) {
        // remove shared memory objects
        return delete_shared_mem(name);
    }

    return start_server(name, redraw, events);
}



} // end namespace nativefx