plugins {
    id("com.android.application") version "8.1.2"
    id("org.jetbrains.kotlin.android") version "1.9.20"
    id("org.lsposed.lsplugin.resopt") version "1.5"
    id("org.lsposed.lsplugin.apksign") version "1.4"
    id("org.lsposed.lsplugin.apktransform") version "1.2"
    id("org.lsposed.lsplugin.cmaker") version "1.2"
}

val appVerCode = 1
val appVerName: String by rootProject

apksign {
    storeFileProperty = "releaseStoreFile"
    storePasswordProperty = "releaseStorePassword"
    keyAliasProperty = "releaseKeyAlias"
    keyPasswordProperty = "releaseKeyPassword"
}

apktransform {
    copy {
        when (it.buildType) {
            "release" -> file("${it.name}/WeChatPad_${appVerName}.apk")
            else -> null
        }
    }
}

cmaker {
    default {
        targets("dexhelper")
        abiFilters("armeabi-v7a", "arm64-v8a", "x86", "x86_64")
        arguments += "-DANDROID_STL=none"
        cppFlags += "-Wno-c++2b-extensions"
    }

    buildTypes {
        arguments += "-DDEBUG_SYMBOLS_PATH=${layout.buildDirectory.file("symbols/${it.name}").get().asFile.absolutePath}"
    }
}

android {
    namespace = "com.rarnu.wechatpad"
    compileSdk = 34
    buildToolsVersion = "34.0.0"
    ndkVersion = "26.0.10792818"

    buildFeatures {
        prefab = true
        buildConfig = true
    }

    defaultConfig {
        applicationId = "com.rarnu.wechatpad"
        minSdk = 24
        targetSdk = 34  // Target Android U
        versionCode = appVerCode
        versionName = appVerName
    }

    buildTypes {
        release {
            isMinifyEnabled = false
        }
    }

    compileOptions {
        sourceCompatibility(JavaVersion.VERSION_1_8)
        targetCompatibility(JavaVersion.VERSION_1_8)
    }

    kotlinOptions {
        jvmTarget = "1.8"
        freeCompilerArgs = listOf(
            "-Xno-param-assertions",
            "-Xno-call-assertions",
            "-Xno-receiver-assertions",
            "-language-version=1.9",
        )
    }

    packaging {
        resources {
            excludes += "/META-INF/{AL2.0,LGPL2.1}"
        }
    }

    androidResources {
        additionalParameters += arrayOf("--allow-reserved-package-id", "--package-id", "0x23")
    }

    externalNativeBuild {
        cmake {
            path("src/main/jni/CMakeLists.txt")
            version = "3.22.1+"
        }
    }
}

dependencies {
    compileOnly("de.robv.android.xposed:api:82")
    implementation("org.jetbrains.kotlin:kotlin-stdlib:1.9.20")
    implementation("dev.rikka.ndk.thirdparty:cxx:1.2.0")
}
