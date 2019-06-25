#include<iostream>
#include <chrono>
#include <thread>

// force boost to be  included as header only, also on windows
#define BOOST_ALL_NO_LIB 1

#define _USE_MATH_DEFINES
#include <cmath>

#include "args/args.hxx"

#include "shared_memory.h"

namespace ipc  = boost::interprocess;

ipc::shared_memory_object shm_buffer;
ipc::mapped_region buffer_region;


void set_rgba(uchar* buffer_data, int buffer_w, int buffer_h, int x, int y, uchar r, uchar g, uchar b, uchar a) {
    buffer_data[y*buffer_w*4+x*4+0] = b; // B
    buffer_data[y*buffer_w*4+x*4+1] = g; // G
    buffer_data[y*buffer_w*4+x*4+2] = r; // R
    buffer_data[y*buffer_w*4+x*4+3] = a; // A
}

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

int main(int argc, char *argv[])
{
    args::ArgumentParser parser("This is a NativeFX test program.", "---");
    args::HelpFlag helpArg(parser, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<std::string> infoNameArg(parser, "info-name", "Defines the name of the shared memory info object created by this program", {'i', "info"});
    args::ValueFlag<std::string> bufferNameArg(parser, "buffer-name", "Defines the name of the shared memory buffer object created by this program", {'b', "buffer"});
    args::Flag deleteSharedMem(parser, "delete", "Indicates that existing shared memory with the specified name should be deleted", {'d', "delete"});

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (const args::Completion& e)
    {
        std::cout << e.what();
        return 0;
    }
    catch (const args::Help&)
    {
        std::cout << parser;
        return 0;
    }
    catch (const args::ParseError& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    std::string infoName = args::get(infoNameArg);
    std::string bufferName = args::get(bufferNameArg);

    if((infoName.size() == 0 || bufferName.size() == 0) && !deleteSharedMem) {
        std::cerr << std::endl << std::endl << "ERROR: 'info-name' and 'buffer-name' must be specified to create or delete shared memory!" << std::endl << std::endl;
        std::cerr << parser;
        return 1;
    }


    if(deleteSharedMem) {
        // remove shared memory objects
        ipc::shared_memory_object::remove(infoName.c_str());
        ipc::shared_memory_object::remove(bufferName.c_str());

        std::cout << "> deleted shared-mem" <<std::endl;
        std::cout << "  -> info:   " << infoName<<std::endl;
        std::cout << "  -> buffer: " << bufferName<<std::endl;

        return 0;
    }

    std::cout << "> creating shared-mem" <<std::endl;
    std::cout << "  -> info:   " << infoName<<std::endl;
    std::cout << "  -> buffer: " << bufferName<<std::endl;

    ipc::shared_memory_object shm_info;

    try {
        // create the shared memory info object.
        shm_info = ipc::shared_memory_object(
                    ipc::create_only,
                    infoName.c_str(),
                    ipc::read_write
        );
    } catch(ipc::interprocess_exception & ex) {
            // remove shared memory objects
            ipc::shared_memory_object::remove(infoName.c_str());
            ipc::shared_memory_object::remove(bufferName.c_str());

            std::cout << "> deleted pre-existing shared-mem" <<std::endl;
            std::cout << "  -> info:   " << infoName<<std::endl;
            std::cout << "  -> buffer: " << bufferName<<std::endl;

            // create the shared memory info object.
            shm_info = ipc::shared_memory_object(
                        ipc::create_only,
                        infoName.c_str(),
                        ipc::read_write
            );

            std::cout << "> created shared-mem" <<std::endl;
            std::cout << "  -> info:   " << infoName<<std::endl;
            std::cout << "  -> buffer: " << bufferName<<std::endl;
    }

    // set the shm size
    shm_info.truncate(sizeof(struct shared_memory_info));

    // map the shared memory info object in this process
    ipc::mapped_region info_region(shm_info, ipc::read_write);

    // get the adress of the info object
    void* info_addr = info_region.get_address();

    // construct the shared structure in memory
    shared_memory_info* info_data = new (info_addr) shared_memory_info;

    int W = info_data->w;
    int H = info_data->h;

    uchar* buffer_data = create_shared_buffer(bufferName, W, H);

    double full = W*H;

    int counter = 0;
    while(true) {

        info_data->mutex.lock();
        bool is_dirty = info_data->dirty;
        if(is_dirty) { 
            info_data->mutex.unlock();
            continue;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        for(int y = 0; y < H; ++y) {
            for(int x = 0; x < W;++x) {
                set_rgba(buffer_data,W,H,x,y, 
                        0, // R
                        0, // G
                        0, // B
                      255  // A
                );
            }
        }

    for(double alpha = 0; alpha < 2 * M_PI; alpha+=0.001) {
        int x = W/2+sin(alpha)*(counter%(W/2));
        int y = H/2+cos(alpha)*(counter%(W/2));
        if(x > 0 && x < W && y > 0 && y < H) {
            set_rgba(buffer_data,W,H,x,y, 
                255, // R
                0, // G
                0, // B
                255  // A
            );
        }
    }

    counter+=5;

    for(int x = 0; x < W;++x) {
        int y = (double)H/(double)W * x;

        set_rgba(buffer_data,W,H,x,y, 
            255, // R
              0, // G
              0, // B
            255  // A
        );

        int x_mirror = W-x;

        set_rgba(buffer_data,W,H,x_mirror,y, 
              0, // R
            255, // G
              0, // B
            255  // A
        );
    }

    info_data->dirty = true;

    int new_W = info_data->w;
    int new_H = info_data->h;

    if(new_W!=W || new_H != H) {
        //trigger buffer resize

        W = new_W;
        H = new_H;

        std::cout << "[" + infoName + "]" << "> resize to W: " << W << ", H: " << H << std::endl;

        ipc::shared_memory_object::remove(bufferName.c_str());
        buffer_data = create_shared_buffer(bufferName, W, H);
        info_data->buffer_ready = true;
    }

    info_data->mutex.unlock();

    // publish buffer changes
    //info_data->mutex.unlock();
    //info_data->buffer_semaphore.post();

    // process messages

        // receive msg from client
        // bool has_msg = info_data->client_to_server_msg_semaphore.try_wait();
        
        // if(has_msg) {
        //     std::string msg = "";
        //     msg = info_data->client_to_server_msg;

        //     // send response from server to client
        //     store_shared_string("sharing works 123!", info_data->client_to_server_res);
        //     info_data->client_to_server_res_semaphore.post();

        //     std::cout << "received: " << msg << std::endl;
        // }

    } // end while

    // remove shared memory objects
    ipc::shared_memory_object::remove(infoName.c_str());
    ipc::shared_memory_object::remove(bufferName.c_str());

    return 0;
}
