#ifndef JNITYPECONVERTER_H
#define JNITYPECONVERTER_H

#include<string>
#include <iostream>
#include <jni.h>

/**
 * Converts a Java boolean to a native boolean.
 * @param b Java boolean to convert
 * @return native boolean
 */
inline bool boolJ2C(jboolean b) {
    return b == JNI_TRUE;
}

/**
 * Converts a native boolean to a Java boolean.
 * @param b native boolean to convert
 * @return Java boolean
 */
inline jboolean boolC2J(bool b) {
    return b ? JNI_TRUE : JNI_FALSE;
}

/**
 * Converts a native string to a Java string.
 * @param env JVM environment to operate on
 * @param s string to convert
 * @return a java string
 */
jstring stringC2J(JNIEnv *env, const char* s);

/**
 * Converts a native string to a Java string.
 * @param env JVM environment to operate on
 * @param s string to convert
 * @return a java string
 */
jstring stringC2J(JNIEnv *env, std::string const& s);

/**
 * <p>
 * Converts a Java string to a native string.
 * </p>
 * <p>
 * <b>Note:</b> this function must not be used to convert large amounts
 * of strings because of inefficient memory handling.
 * </p>
 * @param env JVM environment to operate on
 * @param s string to convert
 * @return a native string
 */
std::string stringJ2C(JNIEnv *env, jstring const& s) ;

jbyteArray ucharArray2JByteArray(JNIEnv* env, unsigned char* buf, int len);

unsigned char* byteArray2ucharArray(JNIEnv* env, jbyteArray array);

#endif // JNITYPECONVERTER_H

