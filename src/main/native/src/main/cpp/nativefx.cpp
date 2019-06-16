#ifndef _Included_NativeFX_CPP
#define _Included_NativeFX_CPP

#include "eu_mihosoft_nativefx_NativeBinding.h"
#include "jnitypeconverter.h"

JNIEXPORT jstring JNICALL Java_eu_mihosoft_nativefx_NativeBinding_sendMsg
  (JNIEnv *env, jclass cls, jint key, jstring msg) {

      // TODO implement msg sending via shared memory

      return stringC2J(env, "hello from native!");
}

#endif /*_Included_NativeFX_CPP*/