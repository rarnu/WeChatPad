@file:Suppress("PLATFORM_CLASS_MAPPED_TO_KOTLIN")

package com.rarnu.dex

import java.io.Closeable
import java.lang.reflect.Field
import java.lang.reflect.Member

class DexHelper(private val classLoader: ClassLoader) : Object(), AutoCloseable, Closeable {

    companion object {
        @JvmStatic
        val NO_CLASS_INDEX = -1
    }

    private val token: Long = load(classLoader)

    external fun findMethodUsingString(str: String, matchPrefix: Boolean, returnType: Long, parameterCount: Short, parameterShorty: String?, declaringClass: Long, parameterTypes: LongArray?, containsParameterTypes: LongArray?, dexPriority: IntArray?, findFirst: Boolean): LongArray

    external fun findMethodInvoking(methodIndex: Long, returnType: Long, parameterCount: Short, parameterShorty: String?, declaringClass: Long, parameterTypes: LongArray?, containsParameterTypes: LongArray?, dexPriority: IntArray?, findFirst: Boolean): LongArray

    external fun findMethodInvoked(methodIndex: Long, returnType: Long, parameterCount: Short, parameterShorty: String?, declaringClass: Long, parameterTypes: LongArray?, containsParameterTypes: LongArray?, dexPriority: IntArray?, findFirst: Boolean): LongArray

    external fun findMethodSettingField(fieldIndex: Long, returnType: Long, parameterCount: Short, parameterShorty: String?, declaringClass: Long, parameterTypes: LongArray?, containsParameterTypes: LongArray?, dexPriority: IntArray?, findFirst: Boolean): LongArray

    external fun findMethodGettingField(fieldIndex: Long, returnType: Long, parameterCount: Short, parameterShorty: String?, declaringClass: Long, parameterTypes: LongArray?, containsParameterTypes: LongArray?, dexPriority: IntArray?, findFirst: Boolean): LongArray

    external fun findField(type: Long, dexPriority: IntArray?, findFirst: Boolean): LongArray

    external fun decodeMethodIndex(methodIndex: Long): Member?

    external fun encodeMethodIndex(method: Member): Long

    external fun decodeFieldIndex(fieldIndex: Long): Class<*>?

    external fun encodeFieldIndex(field: Field): Long

    external fun encodeClassIndex(clazz: Class<*>): Long

    external fun decodeClassIndex(classIndex: Long): Class<*>?

    external fun createFullCache()

    private external fun load(classLoader: ClassLoader): Long

    external override fun close()

    override fun finalize() {
        close()
    }

}