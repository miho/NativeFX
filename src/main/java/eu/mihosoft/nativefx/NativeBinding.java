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
package eu.mihosoft.nativefx;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.nio.file.Paths;

public final class NativeBinding {

    private static boolean initialized;

    // {
    //     init();
    // }

    private NativeBinding() {
        throw new AssertionError("Dont instantiate me!");
    }

    static String libEnding() {
        if(isOS("windows")) return ".dll";
        if(isOS("linux")) return ".so";
        if(isOS("macos")) return ".dylib";
        return ".so";
    }

    static String archName() {
        String osArch = System.getProperty("os.arch");

        if(osArch.toLowerCase().contains("x64") 
        || osArch.toLowerCase().contains("amd64")
        || osArch.toLowerCase().contains("x86_64")) {
            return "x64";
        } else if(osArch.toLowerCase().contains("x86")) {
            return "x86";
        } else if(osArch.toLowerCase().contains("aarch64")) {
            return "aarch64";
        }

        return osArch;
    }

    static String osName() {
        String osName = System.getProperty("os.name");

        if(osName.toLowerCase().contains("windows")) {
            return "windows";
        } else if(osName.toLowerCase().contains("linux")) {
            return "linux";
        } else if(osName.toLowerCase().contains("mac") 
        || osName.toLowerCase().contains("darwin")) {
            return "macos";
        }

        return osName;
    }

    static boolean isOS(String os) {
        return os.equals(osName());
    }

    static final void init() {

        if(initialized) {
            // throw new RuntimeException("Already initialized.");

            return;
        }

        initialized = true;

        String path = "/eu/mihosoft/nativefx/nativelibs/";
        String vcredistPath1 = path;
        String vcredistPath2 = path;
        String libName = "nativefx"+libEnding();

        if(isOS("windows")) {
            path+="windows/"+ archName()+ "/"+libName;
            vcredistPath1+="windows/"+ archName()+ "/vcruntime140.dll";
            vcredistPath2+="windows/"+ archName()+ "/msvcp140.dll";
        } else if(isOS("linux")) {
            libName="lib"+libName;
            path+="linux/"+ archName()+ "/"+libName;
        } else if(isOS("macos")) {
            libName="lib"+libName;
            path+="macos/"+ archName()+ "/"+libName;
        }

        try {
            Path libPath = Files.createTempDirectory("nativefx-libs");

            if(isOS("windows")) {
                resourceToFile(vcredistPath1, Paths.get(libPath.toFile().getAbsolutePath(), "vcruntime140.dll"));
                resourceToFile(vcredistPath2, Paths.get(libPath.toFile().getAbsolutePath(), "msvcp140.dll"));

                System.out.println("> loading " + libPath.toFile().getAbsolutePath()+"/vcruntime140.dll");
                System.load(libPath.toFile().getAbsolutePath()+"/vcruntime140.dll");
            }

            resourceToFile(path, Paths.get(libPath.toFile().getAbsolutePath(), libName));

            System.out.println("> loading " + libPath.toFile().getAbsolutePath()+"/"+libName);
            System.load(libPath.toFile().getAbsolutePath()+"/"+libName);
        } catch (IOException e) {
            e.printStackTrace(System.err);
        }
    }

    private static void resourceToFile(String resource, Path destination) throws IOException{
        try (InputStream is = NativeBinding.class.getResourceAsStream(resource)) {

            System.out.println("> unpacking resource '" + resource+"' to file '"+destination + "'");

            Files.copy(is, destination,
                        StandardCopyOption.REPLACE_EXISTING);
        } catch (NullPointerException e) {
            throw new FileNotFoundException("Resource '" + resource + "' was not found.");
        }
    }

    static native int     nextKey  ();

    static native int     connectTo  (String name);
    static native boolean terminate  (int key);
    static native boolean isConnected(int key);

    static native String  sendMsg(int key, String msg);

    // native void addEventListener   (int key, NativeEventListener l);
    // native void removeEventListener(int key, NativeEventListener l);

    static native void    resize   (int key, int w, int h);
    static native boolean isDirty  (int key);
    static native void    redraw   (int key, int x, int y, int w, int h);
    static native void    setDirty (int key, boolean value);
    static native int     getW     (int key);
    static native int     getH     (int key);

    // native void fireMousePressedEvent (int key, double x, double y,                int buttons, int modifiers, long timestamp);
    // native void fireMouseReleasedEvent(int key, double x, double y,                int buttons, int modifiers, long timestamp);
    // native void fireMouseMoveEvent    (int key, double x, double y,                int buttons, int modifiers, long timestamp);
    // native void fireMouseWheelEvent   (int key, double x, double y, double amount, int buttons, int modifiers, long timestamp);

    // native void fireKeyPressedEvent (int key, String characters, int modifiers, long timestamp);
    // native void fireKeyReleasedEvent(int key, String characters, int modifiers, long timestamp);
    // native void fireKeyTypedEvent   (int key, String characters, int modifiers, long timestamp);

    static native ByteBuffer getBuffer(int key);

    static native void       lock     (int key);
    static native void       unlock   (int key);

    static native void waitForBufferChanges(int key);
    
    static native void       lockBuffer     (int key);
    static native void       unlockBuffer   (int key);


}