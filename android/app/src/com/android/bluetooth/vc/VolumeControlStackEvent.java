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

package com.android.bluetooth.vc;

import android.bluetooth.BluetoothDevice;

public class VolumeControlStackEvent {
    // Event types for STACK_EVENT message (coming from native)
    private static final int EVENT_TYPE_NONE = 0;
    public static final int EVENT_TYPE_CONNECTION_STATE_CHANGED = 1;
    public static final int EVENT_TYPE_VOLUME_STATE_CHANGED = 2;
    public static final int EVENT_TYPE_DEVICE_AVAILABLE = 3;
    public static final int EVENT_TYPE_EXT_AUDIO_OUT_VOL_OFFSET_CHANGED = 4;
    public static final int EVENT_TYPE_EXT_AUDIO_OUT_LOCATION_CHANGED = 5;
    public static final int EVENT_TYPE_EXT_AUDIO_OUT_DESCRIPTION_CHANGED = 6;
    public static final int EVENT_TYPE_EXT_AUDIO_IN_STATE_CHANGED = 7;
    public static final int EVENT_TYPE_EXT_AUDIO_IN_STATUS_CHANGED = 8;
    public static final int EVENT_TYPE_EXT_AUDIO_IN_TYPE_CHANGED = 9;
    public static final int EVENT_TYPE_EXT_AUDIO_IN_DESCR_CHANGED = 10;
    public static final int EVENT_TYPE_EXT_AUDIO_IN_GAIN_PROPS_CHANGED = 11;

    // Do not modify without updating the HAL bt_vc_aid.h files.
    // Match up with enum class ConnectionState of bt_vc_aid.h.
    static final int CONNECTION_STATE_DISCONNECTED = 0;
    static final int CONNECTION_STATE_CONNECTING = 1;
    static final int CONNECTION_STATE_CONNECTED = 2;
    static final int CONNECTION_STATE_DISCONNECTING = 3;

    public int type;
    public BluetoothDevice device;
    public int valueInt1;
    public int valueInt2;
    public int valueInt3;
    public int valueInt4;
    public boolean valueBool1;
    public boolean valueBool2;
    public String valueString1;

    /* Might need more for other callbacks*/

    VolumeControlStackEvent(int type) {
        this.type = type;
    }

    @Override
    public String toString() {
        // event dump
        StringBuilder result = new StringBuilder();
        result.append("VolumeControlStackEvent {type:").append(eventTypeToString(type));
        result.append(", device:").append(device);
        result.append(", valueInt1:").append(eventTypeValue1ToString(type, valueInt1));
        result.append(", valueInt2:").append(eventTypeValue2ToString(type, valueInt2));
        result.append(", valueInt3:").append(eventTypeValue3ToString(type, valueInt3));
        result.append(", valueInt4:").append(eventTypeValue4ToString(type, valueInt4));
        result.append(", valueBool1:").append(eventTypeValueBool1ToString(type, valueBool1));
        result.append(", valueBool2:").append(eventTypeValueBool2ToString(type, valueBool2));
        result.append(", valueString1:").append(eventTypeString1ToString(type, valueString1));
        result.append("}");
        return result.toString();
    }

    private static String eventTypeToString(int type) {
        switch (type) {
            case EVENT_TYPE_NONE:
                return "EVENT_TYPE_NONE";
            case EVENT_TYPE_CONNECTION_STATE_CHANGED:
                return "EVENT_TYPE_CONNECTION_STATE_CHANGED";
            case EVENT_TYPE_VOLUME_STATE_CHANGED:
                return "EVENT_TYPE_VOLUME_STATE_CHANGED";
            case EVENT_TYPE_DEVICE_AVAILABLE:
                return "EVENT_TYPE_DEVICE_AVAILABLE";
            case EVENT_TYPE_EXT_AUDIO_OUT_VOL_OFFSET_CHANGED:
                return "EVENT_TYPE_EXT_AUDIO_OUT_VOL_OFFSET_CHANGED";
            case EVENT_TYPE_EXT_AUDIO_OUT_LOCATION_CHANGED:
                return "EVENT_TYPE_EXT_AUDIO_OUT_LOCATION_CHANGED";
            case EVENT_TYPE_EXT_AUDIO_OUT_DESCRIPTION_CHANGED:
                return "EVENT_TYPE_EXT_AUDIO_OUT_DESCRIPTION_CHANGED";
            case EVENT_TYPE_EXT_AUDIO_IN_STATE_CHANGED:
                return "EVENT_TYPE_EXT_AUDIO_IN_STATE_CHANGED";
            case EVENT_TYPE_EXT_AUDIO_IN_STATUS_CHANGED:
                return "EVENT_TYPE_EXT_AUDIO_IN_STATUS_CHANGED";
            case EVENT_TYPE_EXT_AUDIO_IN_TYPE_CHANGED:
                return "EVENT_TYPE_EXT_AUDIO_IN_TYPE_CHANGED";
            case EVENT_TYPE_EXT_AUDIO_IN_DESCR_CHANGED:
                return "EVENT_TYPE_EXT_AUDIO_IN_DESCR_CHANGED";
            case EVENT_TYPE_EXT_AUDIO_IN_GAIN_PROPS_CHANGED:
                return "EVENT_TYPE_EXT_AUDIO_IN_GAIN_PROPS_CHANGED";
            default:
                return "EVENT_TYPE_UNKNOWN:" + type;
        }
    }

    private static String eventTypeValue1ToString(int type, int value) {
        switch (type) {
            case EVENT_TYPE_CONNECTION_STATE_CHANGED:
                switch (value) {
                    case CONNECTION_STATE_DISCONNECTED:
                        return "CONNECTION_STATE_DISCONNECTED";
                    case CONNECTION_STATE_CONNECTING:
                        return "CONNECTION_STATE_CONNECTING";
                    case CONNECTION_STATE_CONNECTED:
                        return "CONNECTION_STATE_CONNECTED";
                    case CONNECTION_STATE_DISCONNECTING:
                        return "CONNECTION_STATE_DISCONNECTING";
                    default:
                        return "UNKNOWN";
                }
            case EVENT_TYPE_VOLUME_STATE_CHANGED:
                return "{group_id:" + value + "}";
            case EVENT_TYPE_DEVICE_AVAILABLE:
                return "{num_ext_outputs:" + value + "}";
            case EVENT_TYPE_EXT_AUDIO_OUT_VOL_OFFSET_CHANGED:
            case EVENT_TYPE_EXT_AUDIO_OUT_LOCATION_CHANGED:
            case EVENT_TYPE_EXT_AUDIO_OUT_DESCRIPTION_CHANGED:
                return "{ext output id:" + value + "}";
            case EVENT_TYPE_EXT_AUDIO_IN_STATE_CHANGED:
            case EVENT_TYPE_EXT_AUDIO_IN_STATUS_CHANGED:
            case EVENT_TYPE_EXT_AUDIO_IN_TYPE_CHANGED:
            case EVENT_TYPE_EXT_AUDIO_IN_DESCR_CHANGED:
            case EVENT_TYPE_EXT_AUDIO_IN_GAIN_PROPS_CHANGED:
                return "{ext input id:" + value + "}";
            default:
                break;
        }
        return Integer.toString(value);
    }

    private static String eventTypeValue2ToString(int type, int value) {
        switch (type) {
            case EVENT_TYPE_CONNECTION_STATE_CHANGED:
                switch (value) {
                    case CONNECTION_STATE_DISCONNECTED:
                        return "CONNECTION_STATE_DISCONNECTED";
                    case CONNECTION_STATE_CONNECTING:
                        return "CONNECTION_STATE_CONNECTING";
                    case CONNECTION_STATE_CONNECTED:
                        return "CONNECTION_STATE_CONNECTED";
                    case CONNECTION_STATE_DISCONNECTING:
                        return "CONNECTION_STATE_DISCONNECTING";
                    default:
                        return "UNKNOWN";
                }
            case EVENT_TYPE_VOLUME_STATE_CHANGED:
                return "{volume:" + value + "}";
            case EVENT_TYPE_DEVICE_AVAILABLE:
                return "{num_ext_inputs:" + value + "}";
            case EVENT_TYPE_EXT_AUDIO_IN_STATE_CHANGED:
                return "{ext gain val:" + value + "}";
            case EVENT_TYPE_EXT_AUDIO_IN_STATUS_CHANGED:
                return "{status:" + value + "}";
            case EVENT_TYPE_EXT_AUDIO_IN_TYPE_CHANGED:
                return "{type:" + value + "}";
            default:
                break;
        }
        return Integer.toString(value);
    }

    private static String eventTypeValue3ToString(int type, int value) {
        switch (type) {
            case EVENT_TYPE_EXT_AUDIO_IN_STATE_CHANGED:
                return "{ext gain mode:" + value + "}";
            case EVENT_TYPE_VOLUME_STATE_CHANGED:
                return "{flags:" + value + "}";
            default:
                break;
        }
        return Integer.toString(value);
    }

    private static String eventTypeValue4ToString(int type, int value) {
        switch (type) {
            case EVENT_TYPE_EXT_AUDIO_IN_GAIN_PROPS_CHANGED:
                return "{gain set max:" + value + "}";
            default:
                break;
        }
        return Integer.toString(value);
    }

    private static String eventTypeValueBool1ToString(int type, boolean value) {
        switch (type) {
            case EVENT_TYPE_VOLUME_STATE_CHANGED:
            case EVENT_TYPE_EXT_AUDIO_IN_STATE_CHANGED:
                return "{muted:" + value + "}";
            default:
                break;
        }
        return Boolean.toString(value);
    }

    private static String eventTypeValueBool2ToString(int type, boolean value) {
        switch (type) {
            case EVENT_TYPE_VOLUME_STATE_CHANGED:
                return "{isAutonomous:" + value + "}";
            default:
                break;
        }
        return Boolean.toString(value);
    }

    private static String eventTypeString1ToString(int type, String value) {
        switch (type) {
            case EVENT_TYPE_EXT_AUDIO_OUT_DESCRIPTION_CHANGED:
            case EVENT_TYPE_EXT_AUDIO_IN_DESCR_CHANGED:
                return "{description:" + value + "}";
            default:
                break;
        }
        return value;
    }
}
