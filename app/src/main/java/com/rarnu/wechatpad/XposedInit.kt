package com.rarnu.wechatpad

import android.util.Log
import com.rarnu.dex.DexHelper
import dalvik.system.BaseDexClassLoader
import de.robv.android.xposed.IXposedHookLoadPackage
import de.robv.android.xposed.XC_MethodHook
import de.robv.android.xposed.XposedBridge
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.callbacks.XC_LoadPackage.LoadPackageParam

class XposedInit : IXposedHookLoadPackage {

    init {
        try {
            System.loadLibrary("dexhelper")
        } catch (e: Exception) {
            Log.e(TAG, "load library error: $e")
        }
    }

    override fun handleLoadPackage(lpparam: LoadPackageParam) {

        val findClassIfExists = XposedHelpers.findClassIfExists("com.tencent.tinker.loader.app.TinkerApplication", lpparam.classLoader)
        if (findClassIfExists != null) {
            try {
                XposedHelpers.findAndHookMethod(findClassIfExists, "getTinkerFlags", object: XC_MethodHook() {
                    override fun afterHookedMethod(param: MethodHookParam) {
                        try {
                            param.result = 0
                        } catch (th: Throwable) {
                            val member = param.method
                            Log.e(TAG, "Error occurred calling hooker on $member")
                        }
                    }
                })
            } catch (th: Throwable) {
                Log.e(TAG, "$th")
            }
        }
        val baseDexClassLoader: BaseDexClassLoader?
        var classLoader = lpparam.classLoader
        while (true) {
            if (classLoader !is BaseDexClassLoader) {
                if (classLoader.parent == null) {
                    baseDexClassLoader = null
                    break
                } else {
                    classLoader = classLoader.parent
                }
            } else {
                baseDexClassLoader = classLoader
                break
            }
        }
        if (baseDexClassLoader != null) {
            val dexHelper = DexHelper(baseDexClassLoader)
            val findMethodUsingString = dexHelper.findMethodUsingString("Lenovo TB-9707F", true, -1L, (-1).toShort(), null, -1L, null, null, null, true)
            val methodIdx = if (findMethodUsingString.isEmpty()) null else findMethodUsingString[0]
            if (methodIdx != null) {
                val decodeMethodIndex = dexHelper.decodeMethodIndex(methodIdx)
                XposedBridge.hookMethod(decodeMethodIndex, object: XC_MethodHook() {
                    override fun beforeHookedMethod(param: MethodHookParam) {
                        param.result = true
                    }
                })
            }
        }
    }

}
