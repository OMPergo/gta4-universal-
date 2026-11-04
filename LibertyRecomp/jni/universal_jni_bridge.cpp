#include "../universal_runtime.h"

#include <jni.h>
#include <string>

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_libertyrecomp_LibertyActivity_nativeInitRuntime(JNIEnv* env, jobject /*thiz*/, jstring storage_dir) {
    const char* native_storage = env->GetStringUTFChars(storage_dir, nullptr);
    std::string path = native_storage ? native_storage : "";
    if (native_storage) {
        env->ReleaseStringUTFChars(storage_dir, native_storage);
    }

    return LibertyRecomp::UniversalRuntime::GetInstance().Initialize(path) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_com_libertyrecomp_LibertyActivity_nativeGetRuntimeStatus(JNIEnv* env, jobject /*thiz*/) {
    const auto& status = LibertyRecomp::UniversalRuntime::GetInstance().GetStatus();
    return env->NewStringUTF(status.status_message.c_str());
}

JNIEXPORT jint JNICALL
Java_com_libertyrecomp_LibertyActivity_nativeGetRuntimeState(JNIEnv* /*env*/, jobject /*thiz*/) {
    return static_cast<jint>(LibertyRecomp::UniversalRuntime::GetInstance().GetStatus().state);
}

JNIEXPORT jstring JNICALL
Java_com_libertyrecomp_LibertyActivity_nativeGetHardwareReportJson(JNIEnv* env, jobject /*thiz*/) {
    std::string json = LibertyRecomp::UniversalRuntime::GetInstance().GetStatus().hardware_report.ToJsonString();
    return env->NewStringUTF(json.c_str());
}

}
