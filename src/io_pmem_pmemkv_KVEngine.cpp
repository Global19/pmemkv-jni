/*
 * Copyright 2017, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <jni.h>
#include <libpmemkv.h>

using pmemkv::KVEngine;

extern "C" JNIEXPORT jlong JNICALL Java_io_pmem_pmemkv_KVEngine_kvengine_1open
        (JNIEnv* env, jobject obj, jstring engine, jstring path, jlong size) {

    const char* cengine = env->GetStringUTFChars(engine, NULL);
    const char* cpath = env->GetStringUTFChars(path, NULL);
    KVEngine* result = pmemkv::kvengine_open(cengine, cpath, (size_t) size);
    env->ReleaseStringUTFChars(path, cpath);
    if (result == NULL) {
        env->ThrowNew(env->FindClass("java/lang/IllegalArgumentException"),
                      "unable to open persistent pool");
    }
    return (jlong) result;
}

extern "C" JNIEXPORT void JNICALL Java_io_pmem_pmemkv_KVEngine_kvengine_1close
        (JNIEnv* env, jobject obj, jlong pointer) {

    pmemkv::kvengine_close((KVEngine*) pointer);
}

extern "C" JNIEXPORT jstring JNICALL Java_io_pmem_pmemkv_KVEngine_kvengine_1get
        (JNIEnv* env, jobject obj, jlong pointer, jstring key) {

    const jsize limit = 1024;
    char* cvalue = new char[limit];
    for (int i = 0; i < limit; i++) cvalue[i] = 0;
    int32_t cvaluebytes = 0;

    const char* ckey = env->GetStringUTFChars(key, NULL);
    int8_t res = pmemkv::kvengine_get((KVEngine*) pointer, ckey, limit, cvalue, &cvaluebytes);
    env->ReleaseStringUTFChars(key, ckey);

    if (res == 0) {
        return NULL;
    } else if (res > 0) {
        return env->NewStringUTF(cvalue);
    } else {
        env->ThrowNew(env->FindClass("java/lang/RuntimeException"), "unable to get value");
        return NULL;
    }
}

extern "C" JNIEXPORT void JNICALL Java_io_pmem_pmemkv_KVEngine_kvengine_1put
        (JNIEnv* env, jobject obj, jlong pointer, jstring key, jstring value) {

    const char* ckey = env->GetStringUTFChars(key, NULL);
    const char* cvalue = env->GetStringUTFChars(value, NULL);
    int32_t cvaluebytes = env->GetStringUTFLength(value);
    int8_t res = pmemkv::kvengine_put((KVEngine*) pointer, ckey, cvalue, &cvaluebytes);
    env->ReleaseStringUTFChars(key, ckey);
    env->ReleaseStringUTFChars(value, cvalue);
    if (res != 1) {
        env->ThrowNew(env->FindClass("java/lang/RuntimeException"), "unable to put value");
    }
}

extern "C" JNIEXPORT void JNICALL Java_io_pmem_pmemkv_KVEngine_kvengine_1remove
        (JNIEnv* env, jobject obj, jlong pointer, jstring key) {

    const char* ckey = env->GetStringUTFChars(key, NULL);
    pmemkv::kvengine_remove((KVEngine*) pointer, ckey);
}