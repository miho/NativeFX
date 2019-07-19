#include "nativefx/nativefx_server.hpp"

int main(int argc, char *argv[]) {

    using namespace nativefx;

    int counter = 0;

    int numValues = 10;
    double fpsValues[10];
    auto frameTimeStamp = std::chrono::high_resolution_clock::now();
    int fpsCounter = 0;

    auto redraw = [&counter, numValues, &fpsValues, &frameTimeStamp, &fpsCounter](std::string const& name, uchar* buffer_data, int W, int H) {

        auto currentTimeStamp = std::chrono::high_resolution_clock::now();

        // std::this_thread::sleep_for(std::chrono::milliseconds(10));

        for(int y = 0; y < H; ++y) {
            for(int x = 0; x < W;++x) {
                nativefx::set_rgba(buffer_data,W,H,x,y, 
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
            nativefx::set_rgba(buffer_data,W,H,x,y, 
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

        nativefx::set_rgba(buffer_data,W,H,x,y, 
            255, // R
              0, // G
              0, // B
            255  // A
        );

        int x_mirror = W-x;

        nativefx::set_rgba(buffer_data,W,H,x_mirror,y, 
              0, // R
            255, // G
              0, // B
            255  // A
        );
    }

    long durationNano = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTimeStamp-frameTimeStamp).count();

    double fps = 1e9 / durationNano;

    fpsValues[fpsCounter] = fps;

    if(fpsCounter == numValues -1) {
        double fpsAverage = 0;
                        
        for(double fpsVal : fpsValues) {
            fpsAverage+=fpsVal;
        }

        fpsAverage/=numValues;

        std::cout << "[" << name << "]> fps: " << fpsAverage << std::endl;

        fpsCounter = 0;
    }

    fpsCounter++;

    frameTimeStamp = currentTimeStamp;
    

    };

    auto evt = [](std::string const &name, event* evt) {
        std::cout << "[" + name + "] " << "event received: type=" << evt->type << ", ";

        if(evt->type & NFX_MOUSE_EVENT) {
            mouse_event* evt_mouse_evt = static_cast<mouse_event*>((void*)evt);
            std::cout << "x: " << evt_mouse_evt->x << ", y: " << evt_mouse_evt->x;
        } 
        
        if(evt->type & NFX_MOUSE_PRESSED) {
            mouse_event* evt_mouse_evt = static_cast<mouse_event*>((void*)evt);
            std::cout << ", evt_name: PRESSED" << std::endl;
        } else if(evt->type & NFX_MOUSE_RELEASED) {
            mouse_event* evt_mouse_evt = static_cast<mouse_event*>((void*)evt);
            std::cout << ", evt_name: RLEASED" << std::endl;
        }  else if(evt->type & NFX_MOUSE_ENTERED) {
            mouse_event* evt_mouse_evt = static_cast<mouse_event*>((void*)evt);
            std::cout << ", evt_name: ENTERED" << std::endl;
        }  else if(evt->type & NFX_MOUSE_EXITED) {
            mouse_event* evt_mouse_evt = static_cast<mouse_event*>((void*)evt);
            std::cout << ", evt_name: EXITED" << std::endl;
        }  else if(evt->type & NFX_MOUSE_CLICKED) {
            mouse_event* evt_mouse_evt = static_cast<mouse_event*>((void*)evt);
            std::cout << ", #clicks: " << evt_mouse_evt->click_count << std::endl;
        } else {
            std::cout << std::endl;
        }
    };

    nativefx::start_server(argc, argv, redraw, evt);
}