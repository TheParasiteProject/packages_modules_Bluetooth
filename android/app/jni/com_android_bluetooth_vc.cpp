/*
 * Copyright 2021 HIMSA II K/S - www.himsa.com.
 * Represented by EHIMA - www.ehima.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "BluetoothVolumeControlServiceJni"

#include <string.h>

#include <shared_mutex>

#include "./com_android_bluetooth.h"
#include "hardware/bt_vc.h"

using bluetooth::vc::ConnectionState;
using bluetooth::vc::VolumeControlCallbacks;
using bluetooth::vc::VolumeControlInterface;

namespace android {
static jmethodID method_onConnectionStateChanged;
static jmethodID method_onVolumeStateChanged;
static jmethodID method_onGroupVolumeStateChanged;
static jmethodID method_onDeviceAvailable;
static jmethodID method_onExtAudioOutVolumeOffsetChanged;
static jmethodID method_onExtAudioOutLocationChanged;
static jmethodID method_onExtAudioOutDescriptionChanged;
static jmethodID method_onExtAudioInStateChanged;
static jmethodID method_onExtAudioInStatusChanged;
static jmethodID method_onExtAudioInTypeChanged;
static jmethodID method_onExtAudioInGainPropsChanged;
static jmethodID method_onExtAudioInDescriptionChanged;

static VolumeControlInterface* sVolumeControlInterface = nullptr;
static std::shared_timed_mutex interface_mutex;

static jobject mCallbacksObj = nullptr;
static std::shared_timed_mutex callbacks_mutex;

class VolumeControlCallbacksImpl : public VolumeControlCallbacks {
public:
  ~VolumeControlCallbacksImpl() = default;
  void OnConnectionState(ConnectionState state, const RawAddress& bd_addr) override {
    log::info("state:{}, addr: {}", static_cast<int>(state), bd_addr.ToRedactedStringForLogging());

    std::shared_lock<std::shared_timed_mutex> lock(callbacks_mutex);
    CallbackEnv sCallbackEnv(__func__);
    if (!sCallbackEnv.valid() || mCallbacksObj == nullptr) {
      return;
    }

    ScopedLocalRef<jbyteArray> addr(sCallbackEnv.get(),
                                    sCallbackEnv->NewByteArray(sizeof(RawAddress)));
    if (!addr.get()) {
      log::error("Failed to new jbyteArray bd addr for connection state");
      return;
    }

    sCallbackEnv->SetByteArrayRegion(addr.get(), 0, sizeof(RawAddress),
                                     reinterpret_cast<const jbyte*>(&bd_addr));
    sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onConnectionStateChanged, (jint)state,
                                 addr.get());
  }

  void OnVolumeStateChanged(const RawAddress& bd_addr, uint8_t volume, bool mute, uint8_t flags,
                            bool isAutonomous) override {
    log::info("");

    std::shared_lock<std::shared_timed_mutex> lock(callbacks_mutex);
    CallbackEnv sCallbackEnv(__func__);
    if (!sCallbackEnv.valid() || mCallbacksObj == nullptr) {
      return;
    }

    ScopedLocalRef<jbyteArray> addr(sCallbackEnv.get(),
                                    sCallbackEnv->NewByteArray(sizeof(RawAddress)));
    if (!addr.get()) {
      log::error("Failed to new jbyteArray bd addr for connection state");
      return;
    }

    sCallbackEnv->SetByteArrayRegion(addr.get(), 0, sizeof(RawAddress),
                                     reinterpret_cast<const jbyte*>(&bd_addr));
    sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onVolumeStateChanged, (jint)volume,
                                 (jboolean)mute, (jint)flags, addr.get(), (jboolean)isAutonomous);
  }

  void OnGroupVolumeStateChanged(int group_id, uint8_t volume, bool mute,
                                 bool isAutonomous) override {
    log::info("");

    std::shared_lock<std::shared_timed_mutex> lock(callbacks_mutex);
    CallbackEnv sCallbackEnv(__func__);
    if (!sCallbackEnv.valid() || mCallbacksObj == nullptr) {
      return;
    }

    sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onGroupVolumeStateChanged, (jint)volume,
                                 (jboolean)mute, group_id, (jboolean)isAutonomous);
  }

  void OnDeviceAvailable(const RawAddress& bd_addr, uint8_t num_offsets,
                         uint8_t num_inputs) override {
    log::info("");

    std::shared_lock<std::shared_timed_mutex> lock(callbacks_mutex);
    CallbackEnv sCallbackEnv(__func__);
    if (!sCallbackEnv.valid() || mCallbacksObj == nullptr) {
      return;
    }

    ScopedLocalRef<jbyteArray> addr(sCallbackEnv.get(),
                                    sCallbackEnv->NewByteArray(sizeof(RawAddress)));
    if (!addr.get()) {
      log::error("Failed to get addr for {}", bd_addr);
      return;
    }

    sCallbackEnv->SetByteArrayRegion(addr.get(), 0, sizeof(RawAddress),
                                     reinterpret_cast<const jbyte*>(&bd_addr));
    sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onDeviceAvailable, (jint)num_offsets,
                                 (jint)num_inputs, addr.get());
  }

  void OnExtAudioOutVolumeOffsetChanged(const RawAddress& bd_addr, uint8_t ext_output_id,
                                        int16_t offset) override {
    log::info("");

    std::shared_lock<std::shared_timed_mutex> lock(callbacks_mutex);
    CallbackEnv sCallbackEnv(__func__);
    if (!sCallbackEnv.valid() || mCallbacksObj == nullptr) {
      return;
    }

    ScopedLocalRef<jbyteArray> addr(sCallbackEnv.get(),
                                    sCallbackEnv->NewByteArray(sizeof(RawAddress)));
    if (!addr.get()) {
      log::error(
              "Failed to new jbyteArray bd addr for "
              "OnExtAudioOutVolumeOffsetChanged");
      return;
    }

    sCallbackEnv->SetByteArrayRegion(addr.get(), 0, sizeof(RawAddress),
                                     reinterpret_cast<const jbyte*>(&bd_addr));
    sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onExtAudioOutVolumeOffsetChanged,
                                 (jint)ext_output_id, (jint)offset, addr.get());
  }

  void OnExtAudioOutLocationChanged(const RawAddress& bd_addr, uint8_t ext_output_id,
                                    uint32_t location) override {
    log::info("");

    std::shared_lock<std::shared_timed_mutex> lock(callbacks_mutex);
    CallbackEnv sCallbackEnv(__func__);
    if (!sCallbackEnv.valid() || mCallbacksObj == nullptr) {
      return;
    }

    ScopedLocalRef<jbyteArray> addr(sCallbackEnv.get(),
                                    sCallbackEnv->NewByteArray(sizeof(RawAddress)));
    if (!addr.get()) {
      log::error("Failed to new jbyteArray bd addr for OnExtAudioOutLocationChanged");
      return;
    }

    sCallbackEnv->SetByteArrayRegion(addr.get(), 0, sizeof(RawAddress),
                                     reinterpret_cast<const jbyte*>(&bd_addr));
    sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onExtAudioOutLocationChanged,
                                 (jint)ext_output_id, (jint)location, addr.get());
  }

  void OnExtAudioOutDescriptionChanged(const RawAddress& bd_addr, uint8_t ext_output_id,
                                       std::string descr) override {
    log::info("");

    std::shared_lock<std::shared_timed_mutex> lock(callbacks_mutex);
    CallbackEnv sCallbackEnv(__func__);
    if (!sCallbackEnv.valid() || mCallbacksObj == nullptr) {
      return;
    }

    ScopedLocalRef<jbyteArray> addr(sCallbackEnv.get(),
                                    sCallbackEnv->NewByteArray(sizeof(RawAddress)));
    if (!addr.get()) {
      log::error(
              "Failed to new jbyteArray bd addr for "
              "OnExtAudioOutDescriptionChanged");
      return;
    }

    sCallbackEnv->SetByteArrayRegion(addr.get(), 0, sizeof(RawAddress),
                                     reinterpret_cast<const jbyte*>(&bd_addr));
    jstring description = sCallbackEnv->NewStringUTF(descr.c_str());
    sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onExtAudioOutDescriptionChanged,
                                 (jint)ext_output_id, description, addr.get());
  }

  void OnExtAudioInStateChanged(const RawAddress& bd_addr, uint8_t ext_input_id, int8_t gain_val,
                                uint8_t gain_mode, bool mute) override {
    log::info("");

    std::shared_lock<std::shared_timed_mutex> lock(callbacks_mutex);
    CallbackEnv sCallbackEnv(__func__);
    if (!sCallbackEnv.valid() || mCallbacksObj == nullptr) {
      return;
    }

    ScopedLocalRef<jbyteArray> addr(sCallbackEnv.get(),
                                    sCallbackEnv->NewByteArray(sizeof(RawAddress)));
    if (!addr.get()) {
      log::error("Failed to get addr for {}", bd_addr);
      return;
    }

    sCallbackEnv->SetByteArrayRegion(addr.get(), 0, sizeof(RawAddress),
                                     reinterpret_cast<const jbyte*>(&bd_addr));
    sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onExtAudioInStateChanged, (jint)ext_input_id,
                                 (jint)gain_val, (jint)gain_mode, (jboolean)mute, addr.get());
  }

  void OnExtAudioInStatusChanged(const RawAddress& bd_addr, uint8_t ext_input_id,
                                 bluetooth::vc::VolumeInputStatus status) override {
    log::info("");

    std::shared_lock<std::shared_timed_mutex> lock(callbacks_mutex);
    CallbackEnv sCallbackEnv(__func__);
    if (!sCallbackEnv.valid() || mCallbacksObj == nullptr) {
      return;
    }

    ScopedLocalRef<jbyteArray> addr(sCallbackEnv.get(),
                                    sCallbackEnv->NewByteArray(sizeof(RawAddress)));
    if (!addr.get()) {
      log::error("Failed to get addr for {}", bd_addr);
      return;
    }

    sCallbackEnv->SetByteArrayRegion(addr.get(), 0, sizeof(RawAddress),
                                     reinterpret_cast<const jbyte*>(&bd_addr));
    sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onExtAudioInStatusChanged,
                                 (jint)ext_input_id, (jint)status, addr.get());
  }

  void OnExtAudioInTypeChanged(const RawAddress& bd_addr, uint8_t ext_input_id,
                               bluetooth::vc::VolumeInputType type) override {
    log::info("");

    std::shared_lock<std::shared_timed_mutex> lock(callbacks_mutex);
    CallbackEnv sCallbackEnv(__func__);
    if (!sCallbackEnv.valid() || mCallbacksObj == nullptr) {
      return;
    }

    ScopedLocalRef<jbyteArray> addr(sCallbackEnv.get(),
                                    sCallbackEnv->NewByteArray(sizeof(RawAddress)));
    if (!addr.get()) {
      log::error("Failed to get addr for {}", bd_addr);
      return;
    }

    sCallbackEnv->SetByteArrayRegion(addr.get(), 0, sizeof(RawAddress),
                                     reinterpret_cast<const jbyte*>(&bd_addr));
    sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onExtAudioInTypeChanged, (jint)ext_input_id,
                                 (jint)type, addr.get());
  }

  void OnExtAudioInGainPropsChanged(const RawAddress& bd_addr, uint8_t ext_input_id, uint8_t unit,
                                    int8_t min, int8_t max) override {
    log::info("");

    std::shared_lock<std::shared_timed_mutex> lock(callbacks_mutex);
    CallbackEnv sCallbackEnv(__func__);
    if (!sCallbackEnv.valid() || mCallbacksObj == nullptr) {
      return;
    }

    ScopedLocalRef<jbyteArray> addr(sCallbackEnv.get(),
                                    sCallbackEnv->NewByteArray(sizeof(RawAddress)));
    if (!addr.get()) {
      log::error("Failed to get addr for {}", bd_addr);
      return;
    }

    sCallbackEnv->SetByteArrayRegion(addr.get(), 0, sizeof(RawAddress),
                                     reinterpret_cast<const jbyte*>(&bd_addr));
    sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onExtAudioInGainPropsChanged,
                                 (jint)ext_input_id, (jint)unit, (jint)min, (jint)max, addr.get());
  }

  void OnExtAudioInDescriptionChanged(const RawAddress& bd_addr, uint8_t ext_input_id,
                                      std::string descr) override {
    log::info("");

    std::shared_lock<std::shared_timed_mutex> lock(callbacks_mutex);
    CallbackEnv sCallbackEnv(__func__);
    if (!sCallbackEnv.valid() || mCallbacksObj == nullptr) {
      return;
    }

    ScopedLocalRef<jbyteArray> addr(sCallbackEnv.get(),
                                    sCallbackEnv->NewByteArray(sizeof(RawAddress)));
    if (!addr.get()) {
      log::error("Failed to get addr for {}", bd_addr);
      return;
    }

    sCallbackEnv->SetByteArrayRegion(addr.get(), 0, sizeof(RawAddress),
                                     reinterpret_cast<const jbyte*>(&bd_addr));
    jstring description = sCallbackEnv->NewStringUTF(descr.c_str());
    sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onExtAudioInDescriptionChanged,
                                 (jint)ext_input_id, description, addr.get());
  }
};

static VolumeControlCallbacksImpl sVolumeControlCallbacks;

static void initNative(JNIEnv* env, jobject object) {
  std::unique_lock<std::shared_timed_mutex> interface_lock(interface_mutex);
  std::unique_lock<std::shared_timed_mutex> callbacks_lock(callbacks_mutex);

  const bt_interface_t* btInf = getBluetoothInterface();
  if (btInf == nullptr) {
    log::error("Bluetooth module is not loaded");
    return;
  }

  if (sVolumeControlInterface != nullptr) {
    log::info("Cleaning up VolumeControl Interface before initializing...");
    sVolumeControlInterface->Cleanup();
    sVolumeControlInterface = nullptr;
  }

  if (mCallbacksObj != nullptr) {
    log::info("Cleaning up VolumeControl callback object");
    env->DeleteGlobalRef(mCallbacksObj);
    mCallbacksObj = nullptr;
  }

  if ((mCallbacksObj = env->NewGlobalRef(object)) == nullptr) {
    log::error("Failed to allocate Global Ref for Volume control Callbacks");
    return;
  }

  sVolumeControlInterface =
          const_cast<VolumeControlInterface*>(reinterpret_cast<const VolumeControlInterface*>(
                  btInf->get_profile_interface(BT_PROFILE_VC_ID)));

  if (sVolumeControlInterface == nullptr) {
    log::error("Failed to get Bluetooth Volume Control Interface");
    return;
  }

  sVolumeControlInterface->Init(&sVolumeControlCallbacks);
}

static void cleanupNative(JNIEnv* env, jobject /* object */) {
  std::unique_lock<std::shared_timed_mutex> interface_lock(interface_mutex);
  std::unique_lock<std::shared_timed_mutex> callbacks_lock(callbacks_mutex);

  const bt_interface_t* btInf = getBluetoothInterface();
  if (btInf == nullptr) {
    log::error("Bluetooth module is not loaded");
    return;
  }

  if (sVolumeControlInterface != nullptr) {
    sVolumeControlInterface->Cleanup();
    sVolumeControlInterface = nullptr;
  }

  if (mCallbacksObj != nullptr) {
    env->DeleteGlobalRef(mCallbacksObj);
    mCallbacksObj = nullptr;
  }
}

static jboolean connectVolumeControlNative(JNIEnv* env, jobject /* object */, jbyteArray address) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);

  if (!sVolumeControlInterface) {
    log::error("Failed to get the Bluetooth Volume Control Interface");
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->Connect(*tmpraw);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

static jboolean disconnectVolumeControlNative(JNIEnv* env, jobject /* object */,
                                              jbyteArray address) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);

  if (!sVolumeControlInterface) {
    log::error("Failed to get the Bluetooth Volume Control Interface");
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->Disconnect(*tmpraw);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

static void setVolumeNative(JNIEnv* env, jobject /* object */, jbyteArray address, jint volume) {
  if (!sVolumeControlInterface) {
    log::error("Failed to get the Bluetooth Volume Control Interface");
    return;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->SetVolume(*tmpraw, volume);
  env->ReleaseByteArrayElements(address, addr, 0);
}

static void setGroupVolumeNative(JNIEnv* /* env */, jobject /* object */, jint group_id,
                                 jint volume) {
  if (!sVolumeControlInterface) {
    log::error("Failed to get the Bluetooth Volume Control Interface");
    return;
  }

  sVolumeControlInterface->SetVolume(group_id, volume);
}

static void muteNative(JNIEnv* env, jobject /* object */, jbyteArray address) {
  if (!sVolumeControlInterface) {
    log::error("Failed to get the Bluetooth Volume Control Interface");
    return;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->Mute(*tmpraw);
  env->ReleaseByteArrayElements(address, addr, 0);
}

static void muteGroupNative(JNIEnv* /* env */, jobject /* object */, jint group_id) {
  if (!sVolumeControlInterface) {
    log::error("Failed to get the Bluetooth Volume Control Interface");
    return;
  }
  sVolumeControlInterface->Mute(group_id);
}

static void unmuteNative(JNIEnv* env, jobject /* object */, jbyteArray address) {
  if (!sVolumeControlInterface) {
    log::error("Failed to get the Bluetooth Volume Control Interface");
    return;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->Unmute(*tmpraw);
  env->ReleaseByteArrayElements(address, addr, 0);
}

static void unmuteGroupNative(JNIEnv* /* env */, jobject /* object */, jint group_id) {
  if (!sVolumeControlInterface) {
    log::error("Failed to get the Bluetooth Volume Control Interface");
    return;
  }
  sVolumeControlInterface->Unmute(group_id);
}

/* Native methods for exterbak audio outputs */
static jboolean getExtAudioOutVolumeOffsetNative(JNIEnv* env, jobject /* object */,
                                                 jbyteArray address, jint ext_output_id) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);
  if (!sVolumeControlInterface) {
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->GetExtAudioOutVolumeOffset(*tmpraw, ext_output_id);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

static jboolean setExtAudioOutVolumeOffsetNative(JNIEnv* env, jobject /* object */,
                                                 jbyteArray address, jint ext_output_id,
                                                 jint offset) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);
  if (!sVolumeControlInterface) {
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->SetExtAudioOutVolumeOffset(*tmpraw, ext_output_id, offset);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

static jboolean getExtAudioOutLocationNative(JNIEnv* env, jobject /* object */, jbyteArray address,
                                             jint ext_output_id) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);
  if (!sVolumeControlInterface) {
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->GetExtAudioOutLocation(*tmpraw, ext_output_id);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

static jboolean setExtAudioOutLocationNative(JNIEnv* env, jobject /* object */, jbyteArray address,
                                             jint ext_output_id, jint location) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);
  if (!sVolumeControlInterface) {
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->SetExtAudioOutLocation(*tmpraw, ext_output_id, location);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

static jboolean getExtAudioOutDescriptionNative(JNIEnv* env, jobject /* object */,
                                                jbyteArray address, jint ext_output_id) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);
  if (!sVolumeControlInterface) {
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->GetExtAudioOutDescription(*tmpraw, ext_output_id);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

static jboolean setExtAudioOutDescriptionNative(JNIEnv* env, jobject /* object */,
                                                jbyteArray address, jint ext_output_id,
                                                jstring descr) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);
  if (!sVolumeControlInterface) {
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  std::string description;
  if (descr != nullptr) {
    const char* value = env->GetStringUTFChars(descr, nullptr);
    description = std::string(value);
    env->ReleaseStringUTFChars(descr, value);
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->SetExtAudioOutDescription(*tmpraw, ext_output_id, description);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

/* Native methods for external audio inputs */
static jboolean getExtAudioInStateNative(JNIEnv* env, jobject /* object */, jbyteArray address,
                                         jint ext_input_id) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);
  if (!sVolumeControlInterface) {
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->GetExtAudioInState(*tmpraw, ext_input_id);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

static jboolean getExtAudioInStatusNative(JNIEnv* env, jobject /* object */, jbyteArray address,
                                          jint ext_input_id) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);
  if (!sVolumeControlInterface) {
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->GetExtAudioInStatus(*tmpraw, ext_input_id);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

static jboolean getExtAudioInTypeNative(JNIEnv* env, jobject /* object */, jbyteArray address,
                                        jint ext_input_id) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);
  if (!sVolumeControlInterface) {
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->GetExtAudioInType(*tmpraw, ext_input_id);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

static jboolean getExtAudioInGainPropsNative(JNIEnv* env, jobject /* object */, jbyteArray address,
                                             jint ext_input_id) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);
  if (!sVolumeControlInterface) {
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->GetExtAudioInGainProps(*tmpraw, ext_input_id);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

static jboolean getExtAudioInDescriptionNative(JNIEnv* env, jobject /* object */,
                                               jbyteArray address, jint ext_input_id) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);
  if (!sVolumeControlInterface) {
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->GetExtAudioInDescription(*tmpraw, ext_input_id);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

static jboolean setExtAudioInDescriptionNative(JNIEnv* env, jobject /* object */,
                                               jbyteArray address, jint ext_input_id,
                                               jstring descr) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);
  if (!sVolumeControlInterface) {
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  std::string description;
  if (descr != nullptr) {
    const char* value = env->GetStringUTFChars(descr, nullptr);
    description = std::string(value);
    env->ReleaseStringUTFChars(descr, value);
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->SetExtAudioInDescription(*tmpraw, ext_input_id, description);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

static jboolean setExtAudioInGainValueNative(JNIEnv* env, jobject /* object */, jbyteArray address,
                                             jint ext_input_id, jint gain_val) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);
  if (!sVolumeControlInterface) {
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->SetExtAudioInGainValue(*tmpraw, ext_input_id, gain_val);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

static jboolean setExtAudioInGainModeNative(JNIEnv* env, jobject /* object */, jbyteArray address,
                                            jint ext_input_id, jboolean mode_auto) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);
  if (!sVolumeControlInterface) {
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->SetExtAudioInGainMode(*tmpraw, ext_input_id, mode_auto);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

static jboolean setExtAudioInGainMuteNative(JNIEnv* env, jobject /* object */, jbyteArray address,
                                            jint ext_input_id, jboolean mute) {
  log::info("");
  std::shared_lock<std::shared_timed_mutex> lock(interface_mutex);
  if (!sVolumeControlInterface) {
    return JNI_FALSE;
  }

  jbyte* addr = env->GetByteArrayElements(address, nullptr);
  if (!addr) {
    jniThrowIOException(env, EINVAL);
    return JNI_FALSE;
  }

  RawAddress* tmpraw = reinterpret_cast<RawAddress*>(addr);
  sVolumeControlInterface->SetExtAudioInGainMute(*tmpraw, ext_input_id, mute);
  env->ReleaseByteArrayElements(address, addr, 0);
  return JNI_TRUE;
}

int register_com_android_bluetooth_vc(JNIEnv* env) {
  const JNINativeMethod methods[] = {
          {"initNative", "()V", reinterpret_cast<void*>(initNative)},
          {"cleanupNative", "()V", reinterpret_cast<void*>(cleanupNative)},
          {"connectVolumeControlNative", "([B)Z",
           reinterpret_cast<void*>(connectVolumeControlNative)},
          {"disconnectVolumeControlNative", "([B)Z",
           reinterpret_cast<void*>(disconnectVolumeControlNative)},
          {"setVolumeNative", "([BI)V", reinterpret_cast<void*>(setVolumeNative)},
          {"setGroupVolumeNative", "(II)V", reinterpret_cast<void*>(setGroupVolumeNative)},
          {"muteNative", "([B)V", reinterpret_cast<void*>(muteNative)},
          {"muteGroupNative", "(I)V", reinterpret_cast<void*>(muteGroupNative)},
          {"unmuteNative", "([B)V", reinterpret_cast<void*>(unmuteNative)},
          {"unmuteGroupNative", "(I)V", reinterpret_cast<void*>(unmuteGroupNative)},
          {"getExtAudioOutVolumeOffsetNative", "([BI)Z",
           reinterpret_cast<void*>(getExtAudioOutVolumeOffsetNative)},
          {"setExtAudioOutVolumeOffsetNative", "([BII)Z",
           reinterpret_cast<void*>(setExtAudioOutVolumeOffsetNative)},
          {"getExtAudioOutLocationNative", "([BI)Z",
           reinterpret_cast<void*>(getExtAudioOutLocationNative)},
          {"setExtAudioOutLocationNative", "([BII)Z",
           reinterpret_cast<void*>(setExtAudioOutLocationNative)},
          {"getExtAudioOutDescriptionNative", "([BI)Z",
           reinterpret_cast<void*>(getExtAudioOutDescriptionNative)},
          {"setExtAudioOutDescriptionNative", "([BILjava/lang/String;)Z",
           reinterpret_cast<void*>(setExtAudioOutDescriptionNative)},
  };
  const int result = REGISTER_NATIVE_METHODS(
          env, "com/android/bluetooth/vc/VolumeControlNativeInterface", methods);
  if (result != 0) {
    return result;
  }

  const JNIJavaMethod javaMethods[] = {
          {"onConnectionStateChanged", "(I[B)V", &method_onConnectionStateChanged},
          {"onVolumeStateChanged", "(IZI[BZ)V", &method_onVolumeStateChanged},
          {"onGroupVolumeStateChanged", "(IZIZ)V", &method_onGroupVolumeStateChanged},
          {"onDeviceAvailable", "(I[B)V", &method_onDeviceAvailable},
          {"onExtAudioOutVolumeOffsetChanged", "(II[B)V", &method_onExtAudioOutVolumeOffsetChanged},
          {"onExtAudioOutLocationChanged", "(II[B)V", &method_onExtAudioOutLocationChanged},
          {"onExtAudioOutDescriptionChanged", "(ILjava/lang/String;[B)V",
           &method_onExtAudioOutDescriptionChanged},
  };
  GET_JAVA_METHODS(env, "com/android/bluetooth/vc/VolumeControlNativeInterface", javaMethods);

  return 0;
}
}  // namespace android
