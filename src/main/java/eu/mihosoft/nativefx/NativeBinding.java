package eu.mihosoft.nativefx;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;

public final class NativeBinding {

    private NativeBinding() {
        throw new AssertionError("Dont instantiate me!");
    }

    static String libEnding() {
        if(isOS("windows")) return ".dll";
        if(isOS("linux")) return ".so";
        if(isOS("mac")) return ".dylib";
        return ".so";
    }

    static String archName() {
        String osArch = System.getProperty("os.arch");

        if(osArch.toLowerCase().contains("x64") || osArch.toLowerCase().contains("amd64")) {
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
        } else if(osName.toLowerCase().contains("mac")) {
            return "macos";
        }

        return osName;
    }

    static boolean isOS(String os) {
        return os.equals(osName());
    }

    static void init() {

        String path = "/eu/mihosoft/nativefx/nativelibs/";
        String libName = "nativefx"+libEnding();

        if(isOS("windows")) {
            path+="windows/"+ archName()+ "/"+libName;
        } else if(isOS("linux")) {
            path+="linux/"+ archName()+ "/"+libName;
        } else if(isOS("mac")) {
            path+="macos/"+ archName()+ "/"+libName;
        }

        try {
            Path libPath = Files.createTempDirectory("nativefx-libs");

            String libpath = System.getProperty("java.library.path");
            libpath = libpath + ";"+libPath;
            System.setProperty("java.library.path",libpath);

            try (InputStream is = NativeBinding.class.getResourceAsStream(path)) {
                Files.copy(is, libPath, StandardCopyOption.REPLACE_EXISTING);
            } catch (NullPointerException e) {
                throw new FileNotFoundException("Resource " + path + " was not found.");
            }
            System.load(libPath.toFile().getAbsolutePath()+"\\"+libName);
        } catch (IOException e) {
            e.printStackTrace(System.err);
        }
    }

    static native int     connectTo  (String name);
    static native void    terminate  (int key);
    static native boolean isConnected(int key);

    static native String  sendMsg(int key, String msg);



    // native void addEventListener   (int key, NativeEventListener l);
    // native void removeEventListener(int key, NativeEventListener l);

    // native void    resize   (int key, int w, int h);
    // native boolean isDirty  (int key);
    // native void    redraw   (int key, int x1, int y1, int w, int h);
    // native void    setDirty (int key, boolean value);
    // native int     getW     (int key);
    // native int     getH     (int key);

    // native void fireMousePressedEvent (int key, double x, double y,                int buttons, int modifiers, long timestamp);
    // native void fireMouseReleasedEvent(int key, double x, double y,                int buttons, int modifiers, long timestamp);
    // native void fireMouseMoveEvent    (int key, double x, double y,                int buttons, int modifiers, long timestamp);
    // native void fireMouseWheelEvent   (int key, double x, double y, double amount, int buttons, int modifiers, long timestamp);

    // native void fireKeyPressedEvent (int key, String characters, int modifiers, long timestamp);
    // native void fireKeyReleasedEvent(int key, String characters, int modifiers, long timestamp);
    // native void fireKeyTypedEvent   (int key, String characters, int modifiers, long timestamp);

    // native ByteBuffer getBuffer(int key);
    // native void       lock     (int key);
    // native void       unlock   (int key);

}