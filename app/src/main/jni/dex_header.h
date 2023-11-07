#include <jni.h>

#ifndef __dex_header__
#define __dex_header__

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlongArray JNICALL Java_com_rarnu_dex_DexHelper_findMethodUsingString(
        JNIEnv *env, jobject thiz,
        jstring str, jboolean match_prefix, jlong return_type, jshort parameter_count, jstring parameter_shorty,
        jlong declaring_class, jlongArray parameter_types, jlongArray contains_parameter_types, jintArray dex_priority, jboolean find_first);

JNIEXPORT jlong JNICALL Java_com_rarnu_dex_DexHelper_load(JNIEnv *env, jobject thiz, jobject class_loader);

JNIEXPORT jlongArray JNICALL Java_com_rarnu_dex_DexHelper_findMethodInvoking(
        JNIEnv *env, jobject thiz,
        jlong method_index, jlong return_type, jshort parameter_count, jstring parameter_shorty, jlong declaring_class,
        jlongArray parameter_types, jlongArray contains_parameter_types, jintArray dex_priority, jboolean find_first);

JNIEXPORT jlongArray JNICALL Java_com_rarnu_dex_DexHelper_findMethodInvoked(
        JNIEnv *env, jobject thiz,
        jlong method_index, jlong return_type, jshort parameter_count, jstring parameter_shorty, jlong declaring_class,
        jlongArray parameter_types, jlongArray contains_parameter_types, jintArray dex_priority, jboolean find_first);

JNIEXPORT jlongArray JNICALL Java_com_rarnu_dex_DexHelper_findMethodSettingField(
        JNIEnv *env, jobject thiz,
        jlong field_index, jlong return_type, jshort parameter_count, jstring parameter_shorty, jlong declaring_class,
        jlongArray parameter_types, jlongArray contains_parameter_types, jintArray dex_priority, jboolean find_first);

JNIEXPORT jlongArray JNICALL Java_com_rarnu_dex_DexHelper_findMethodGettingField(
        JNIEnv *env, jobject thiz,
        jlong field_index, jlong return_type, jshort parameter_count, jstring parameter_shorty, jlong declaring_class,
        jlongArray parameter_types, jlongArray contains_parameter_types, jintArray dex_priority, jboolean find_first);

JNIEXPORT jlongArray JNICALL Java_com_rarnu_dex_DexHelper_findField(
        JNIEnv *env, jobject thiz,
        jlong type, jintArray dex_priority, jboolean find_first);

JNIEXPORT jobject JNICALL Java_com_rarnu_dex_DexHelper_decodeMethodIndex(JNIEnv *env, jobject thiz, jlong method_index);

JNIEXPORT jobject JNICALL Java_com_rarnu_dex_DexHelper_decodeFieldIndex(JNIEnv *env, jobject thiz, jlong field_index);

JNIEXPORT jlong JNICALL Java_com_rarnu_dex_DexHelper_encodeClassIndex(JNIEnv *env, jobject thiz, jclass clazz);

JNIEXPORT jlong JNICALL Java_com_rarnu_dex_DexHelper_encodeFieldIndex(JNIEnv *env, jobject thiz, jobject field);

JNIEXPORT jlong JNICALL Java_com_rarnu_dex_DexHelper_encodeMethodIndex(JNIEnv *env, jobject thiz, jobject method);

JNIEXPORT jclass JNICALL Java_com_rarnu_dex_DexHelper_decodeClassIndex(JNIEnv *env, jobject thiz, jlong class_index);

JNIEXPORT void JNICALL Java_com_rarnu_dex_DexHelper_close(JNIEnv *env, jobject thiz);

JNIEXPORT void JNICALL Java_com_rarnu_dex_DexHelper_createFullCache(JNIEnv *env, jobject thiz);

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *);

#ifdef __cplusplus
}
#endif

#endif // __dex_header__