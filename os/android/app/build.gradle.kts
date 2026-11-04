plugins {
    id("com.android.application")
}

val sdlJavaCustom = rootProject.file("sdl3-java")
val sdlJavaUpstream = rootProject.file("../../vendor/XHYN-IV-recomp/glue/rexglue-sdk-main/thirdparty/sdl3/android-project/app/src/main/java")
val sdlJavaDir = if (sdlJavaUpstream.exists()) sdlJavaUpstream else sdlJavaCustom

android {
    namespace   = "com.libertyrecomp"
    compileSdk  = 35
    buildToolsVersion = "35.0.0"
    ndkVersion  = "29.0.14206865"

    defaultConfig {
        applicationId = "com.xhyn.ivrecomp"
        minSdk        = 29
        targetSdk     = 35
        versionCode   = 40
        versionName   = "0.5.9.3-universal-alpha"

        ndk {
            abiFilters += listOf("arm64-v8a")
        }
    }

    sourceSets["main"].java.srcDirs("src/main/java", sdlJavaDir)

    buildTypes {
        debug {
            isDebuggable    = true
            isJniDebuggable = true
            isMinifyEnabled = false
        }
        release {
            isMinifyEnabled = false
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }

    packaging {
        jniLibs {
            useLegacyPackaging = true
        }
    }

    lint {
        checkReleaseBuilds = false
        abortOnError = false
    }
}
