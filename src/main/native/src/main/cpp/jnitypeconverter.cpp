#include "jnitypeconverter.h"


/**
 * Converts a native string to a Java string.
 * @param env JVM environment to operate on
 * @param s string to convert
 * @return a java string
 */
jstring stringC2J(JNIEnv *env, const char* s) {
    return env->NewStringUTF(s);
}

/**
 * Converts a native string to a Java string.
 * @param env JVM environment to operate on
 * @param s string to convert
 * @return a java string
 */
jstring stringC2J(JNIEnv *env, std::string const& s) {
    return env->NewStringUTF(s.c_str());
}

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
std::string stringJ2C(JNIEnv *env, jstring const& s) {
    const char* tmpStr = env->GetStringUTFChars(s, JNI_FALSE);
    std::string result(tmpStr);
    env->ReleaseStringUTFChars(s, tmpStr);
    return result;
}

jbyteArray ucharArray2JByteArray(JNIEnv* env, unsigned char* buf, int len) {
    jbyteArray array = env->NewByteArray(len);
    env->SetByteArrayRegion (array, 0, len, reinterpret_cast<jbyte*>(buf));
    return array;
}

unsigned char* byteArray2ucharArray(JNIEnv* env, jbyteArray array) {
    int len = env->GetArrayLength (array);
    unsigned char* buf = new unsigned char[len];
    env->GetByteArrayRegion (array, 0, len, reinterpret_cast<jbyte*>(buf));
    return buf;
}

namespace threading {

            /**
             * Exception type
             */
            enum ExceptionType {
                /**
                 * Attaching current thread to JVM failed.
                 */
                ATTACH_FAILED,
                /**
                 * Detaching current thread from JVM failed.
                 */
                DETACH_FAILED,
                /**
                 * Current thread not attached to JVM.
                 */
                NOT_ATTACHED
            };

            /**
             * Exception
             */
            class JNIThreadException {
            public:

                /**
                 * Constructor.
                 * @param type exception type
                 */
                JNIThreadException(ExceptionType type) {
                    this->type = type;

                    switch (type) {
                        case ATTACH_FAILED:
                        {
                            std::cerr << "Attaching thread failed!" << std::endl;

                        }
                            break;
                        case DETACH_FAILED:
                        {
                            std::cerr << "Detaching thread failed!" << std::endl;
                        }
                            break;
                        case NOT_ATTACHED:
                        {
                        std::cerr << "Thread not attached!" << std::endl;
                        }
                            break;
                    }
                }

                ExceptionType type;
            };

            /**
             * Attaches the current thread to the JVM. If the thread is already
             * attached this is equivalent to <code>getEnv()</code>.
             * @param javaVM Java VM to operate on
             * @return JVM environment of the current thread
             * @throws JNIThreadException
             */
            inline JNIEnv* attachThread(JavaVM* javaVM) {

                // The following code raised a warning in newer GCC versions:
                // "dereferencing type-punned pointer will break strict-aliasing rules"
                // That is why we do it differently now, although this code
                // is officially used:
                //				JNIEnv* localEnv = NULL;
                //
                //				int result = javaVM->AttachCurrentThread(
                //						(void **) (&localEnv), NULL);

                JNIEnv** localEnvPtrPtr;
                JNIEnv* localEnv = NULL;
                localEnvPtrPtr = &localEnv;

                int result = javaVM->AttachCurrentThread(
                        (void **) (localEnvPtrPtr), NULL);

                if (result < 0) {
                    throw JNIThreadException(ATTACH_FAILED);
                }

                return localEnv;
            }

            /**
             * Detaches the current thread from the JVM.
             * @param javaVM Java VM to operate on
             * @throws JNIThreadException
             */
            inline void detachThread(JavaVM* javaVM) {

                int result = javaVM->DetachCurrentThread();

                if (result < 0) {
                    throw JNIThreadException(DETACH_FAILED);
                }
            }

            /**
             * Returns the JVM environment of the current thread.
             * @param javaVM Java VM to operate on
             * @return JVM environment of the current thread
             * @throws JNIThreadException
             */
            inline JNIEnv* getEnv(JavaVM* javaVM) {

                // The following code raised a warning in newer GCC versions:
                // "dereferencing type-punned pointer will break strict-aliasing rules"
                // That is why we do it differently now, although this code
                // is officially used:
                //				JNIEnv* localEnv = NULL;
                //
                //				jint result = javaVM->GetEnv(
                //						(void **) (&localEnv), JNI_VERSION_1_2);

                JNIEnv** localEnvPtrPtr;
                JNIEnv* localEnv = NULL;
                localEnvPtrPtr = &localEnv;

                jint result = javaVM->GetEnv(
                        (void **) (localEnvPtrPtr), JNI_VERSION_1_2);

                if (result != JNI_OK) {
                    throw JNIThreadException(NOT_ATTACHED);
                }

                return localEnv;
            }
        } // threading::
