// #include<iostream>
// #include <chrono>
// #include <thread>

// // force boost to be  included as header only, also on windows
// #define BOOST_ALL_NO_LIB 1

// #include <boost/thread/xtime.hpp>

// #define _USE_MATH_DEFINES
// #include <cmath>

// // in case M_PI hasn't been defined we do it manually
// // see https://github.com/miho/NativeFX/issues/12
// #ifndef M_PI
// #define M_PI 3.14159265358979323846
// #endif // M_PI

// #include "args/args.hxx"

// #include "shared_memory.h"

// namespace nativefx {

// namespace ipc  = boost::interprocess;

// ipc::shared_memory_object shm_buffer;
// ipc::mapped_region buffer_region;

// ipc::message_queue* evt_mq;

// typedef  std::function<void (uchar* buffer_data, int W, int H)> redraw_callback;
// typedef  std::function<void (event* evt)> event_callback;

// void set_rgba(uchar* buffer_data, int buffer_w, int buffer_h, int x, int y, uchar r, uchar g, uchar b, uchar a) {
//     buffer_data[y*buffer_w*4+x*4+0] = b; // B
//     buffer_data[y*buffer_w*4+x*4+1] = g; // G
//     buffer_data[y*buffer_w*4+x*4+2] = r; // R
//     buffer_data[y*buffer_w*4+x*4+3] = a; // A
// }

// uchar* create_shared_buffer(std::string buffer_name, int w, int h) {

//     // create the shared memory buffer object.
//     shm_buffer = ipc::shared_memory_object (
//                 ipc::create_only,
//                 buffer_name.c_str(),
//                 ipc::read_write
//     );

//     // set the size of the shared image buffer (w*h*#channels*sizeof(uchar))
//     shm_buffer.truncate( w*h // TODO properly resize shared memory
//                         */*#channels*/4
//                         */*channel size*/sizeof(uchar)
//     );

//     // map the shared memory buffer object in this process
//     buffer_region = ipc::mapped_region(
//                 shm_buffer,
//                 ipc::read_write
//     );

//     // get the address of the shared image buffer
//     void * buffer_addr = buffer_region.get_address();

//     // cast shared memory pointer to correct uchar type
//     uchar* buffer_data = (uchar*) buffer_addr;

//     return buffer_data;
// }

// void start(int argc, char *argv[], redraw_callback redraw, event_callback events)
// {
//     args::ArgumentParser parser("This is a NativeFX server program.", "---");
//     args::HelpFlag helpArg(parser, "help", "Display this help menu", {'h', "help"});
//     args::ValueFlag<std::string> nameArg(parser, "name", "Defines the name of the shared memory objects to be created by this program", {'n', "name"});
//     args::Flag deleteSharedMem(parser, "delete", "Indicates that existing shared memory with the specified name should be deleted", {'d', "delete"});

//     try
//     {
//         parser.ParseCLI(argc, argv);
//     }
//     catch (const args::Completion& e)
//     {
//         std::cout << e.what();
//         return;
//     }
//     catch (const args::Help&)
//     {
//         std::cout << parser;
//         return;
//     }
//     catch (const args::ParseError& e)
//     {
//         std::cerr << e.what() << std::endl;
//         std::cerr << parser;
//         return;
//     }

//     std::string name = args::get(nameArg);

//     std::string info_name = name + IPC_INFO_NAME;
//     std::string buffer_name = name + IPC_BUFF_NAME;
//     std::string evt_mq_name = name + IPC_EVT_MQ_NAME;

//     if(name.size() == 0) {
//         std::cerr << std::endl << std::endl << "ERROR: 'name' must be specified to create or delete shared memory!" << std::endl << std::endl;
//         std::cerr << parser;
//         return;
//     }


//     if(deleteSharedMem) {
//         // remove shared memory objects
//         ipc::shared_memory_object::remove(info_name.c_str());
//         ipc::shared_memory_object::remove(buffer_name.c_str());
//         ipc::message_queue::remove(evt_mq_name.c_str());

//         std::cout << "> deleted shared-mem" <<std::endl;
//         std::cout << "  -> name:   " << name<<std::endl;

//         return;
//     }

//     std::cout << "> creating shared-mem" <<std::endl;
//     std::cout << "  -> name:   " << name<<std::endl;

//     ipc::shared_memory_object shm_info;

//     try {
//         // create the shared memory info object.
//         shm_info = ipc::shared_memory_object(
//                     ipc::create_only,
//                     info_name.c_str(),
//                     ipc::read_write
//         );

//         evt_mq = create_evt_mq(evt_mq_name);
//     } catch(ipc::interprocess_exception & ex) {
//             // remove shared memory objects
//             ipc::shared_memory_object::remove(info_name.c_str());
//             ipc::shared_memory_object::remove(buffer_name.c_str());
//             ipc::message_queue::remove(evt_mq_name.c_str());

//             std::cout << "> deleted pre-existing shared-mem" <<std::endl;
//             std::cout << "  -> name:   " << name<<std::endl;

//             // create the shared memory info object.
//             shm_info = ipc::shared_memory_object(
//                         ipc::create_only,
//                         info_name.c_str(),
//                         ipc::read_write
//             );
//             evt_mq = create_evt_mq(evt_mq_name);

//             std::cout << "> created shared-mem"  << std::endl;
//             std::cout << "  -> name:   " << name << std::endl;
//     }

//     // set the shm size
//     shm_info.truncate(sizeof(struct shared_memory_info));

//     // map the shared memory info object in this process
//     ipc::mapped_region info_region(shm_info, ipc::read_write);

//     // get the adress of the info object
//     void* info_addr = info_region.get_address();

//     // construct the shared structure in memory
//     shared_memory_info* info_data = new (info_addr) shared_memory_info;

//     // init c-strings of info_data struct
//     strcpy(info_data->client_to_server_msg, "");
//     strcpy(info_data->client_to_server_res, "");
//     strcpy(info_data->server_to_client_msg, "");
//     strcpy(info_data->server_to_client_res, "");

//     int W = info_data->w;
//     int H = info_data->h;

//     uchar* buffer_data = create_shared_buffer(buffer_name, W, H);

//     double full = W*H;

//     int counter = 0;
//     while(true) {

        
//         // timed locking of resources
//         boost::system_time const timeout=
//         boost::get_system_time()+ boost::posix_time::milliseconds(LOCK_TIMEOUT);
//         bool locking_success = info_data->mutex.timed_lock(timeout);

//         if(!locking_success) {
//             std::cerr << "[" + info_name + "] " << "ERROR: cannot connect to '" << info_name << "':" << std::endl;
//             std::cerr << " -> But we are unable to lock the resources." << std::endl;
//             std::cerr << " -> Client not running?." << std::endl;
//             return;
//         }

//         bool is_dirty = info_data->dirty;
//         if(is_dirty) { 
//             info_data->mutex.unlock();
//             continue;
//         }

//         redraw(buffer_data, W, H);

//     info_data->dirty = true;

//     int new_W = info_data->w;
//     int new_H = info_data->h;

//     if(new_W!=W || new_H != H) {
//         //trigger buffer resize

//         W = new_W;
//         H = new_H;

//         std::cout << "[" + info_name + "]" << "> resize to W: " << W << ", H: " << H << std::endl;

//         ipc::shared_memory_object::remove(buffer_name.c_str());
//         buffer_data = create_shared_buffer(buffer_name, W, H);
//         info_data->buffer_ready = true;
//     }

//     info_data->mutex.unlock();


//     // process events
//     std::size_t MAX_SIZE = max_event_message_size();

//     void* evt_mq_msg_buff = malloc(MAX_SIZE);
//     ipc::message_queue::size_type recvd_size;
//     unsigned int priority;

//     while(evt_mq->get_num_msg()> 0) {

//         // timed locking of resources
//         boost::system_time const timeout=
//           boost::get_system_time() + boost::posix_time::milliseconds(LOCK_TIMEOUT);

//         bool result = evt_mq->timed_receive(evt_mq_msg_buff, MAX_SIZE, recvd_size, priority, timeout);

//         if(!result) {
//             std::cerr << "[" + info_name + "] ERROR: can't read messages, message queue not accessible." << std::endl; 
//         }

//         event* evt = static_cast<event*>(evt_mq_msg_buff);

//         events(evt);
        
//     } 

//     free(evt_mq_msg_buff);

//     // publish buffer changes
//     //info_data->mutex.unlock();
//     //info_data->buffer_semaphore.post();

//     // process messages

//         // receive msg from client
//         // bool has_msg = info_data->client_to_server_msg_semaphore.try_wait();
        
//         // if(has_msg) {
//         //     std::string msg = "";
//         //     msg = info_data->client_to_server_msg;

//         //     // send response from server to client
//         //     store_shared_string("sharing works 123!", info_data->client_to_server_res);
//         //     info_data->client_to_server_res_semaphore.post();

//         //     std::cout << "received: " << msg << std::endl;
//         // }

//     } // end while

//     // remove shared memory objects
//     ipc::shared_memory_object::remove(info_name.c_str());
//     ipc::shared_memory_object::remove(buffer_name.c_str());
//     ipc::message_queue::remove(evt_mq_name.c_str());

//     return;
// }

// } // end namespace nativefx