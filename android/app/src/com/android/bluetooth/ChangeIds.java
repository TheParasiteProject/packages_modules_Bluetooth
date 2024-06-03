/*
 * Copyright (C) 2023 The Android Open Source Project
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

package com.android.bluetooth;

import android.compat.annotation.ChangeId;
import android.compat.annotation.EnabledSince;

/** All the {@link ChangeId} used for Bluetooth App. */
public class ChangeIds {
    /**
     * Starting with {@link android.os.Build.VERSION_CODES#UPSIDE_DOWN_CAKE}, BLUETOOTH_CONNECT
     * permission is enforced in getProfileConnectionState
     */
    @ChangeId
    @EnabledSince(targetSdkVersion = android.os.Build.VERSION_CODES.UPSIDE_DOWN_CAKE)
    public static final long ENFORCE_CONNECT = 211757425L;
}
