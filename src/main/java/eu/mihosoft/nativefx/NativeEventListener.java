package eu.mihosoft.nativefx;

@FunctionalInterface
public interface NativeEventListener {
    void event(String evt);
}