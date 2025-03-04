/*
 * Copyright (C) 2014 The Android Open Source Project
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

package com.android.bluetooth.mapclient;

import android.util.Log;

import java.text.ParseException;
import java.util.Objects;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public final class BmsgTokenizer {
    private static final String TAG = BmsgTokenizer.class.getSimpleName();

    private final String mStr;

    private final Matcher mMatcher;
    private final int mOffset;
    private int mPos = 0;

    public BmsgTokenizer(String str) {
        this(str, 0);
    }

    public BmsgTokenizer(String str, int offset) {
        mStr = str;
        mOffset = offset;
        mMatcher = Pattern.compile("(([^:]*):(.*))?\r\n").matcher(str);
        mPos = mMatcher.regionStart();
    }

    public Property next(boolean alwaysReturn) throws ParseException {
        boolean found = false;

        do {
            mMatcher.region(mPos, mMatcher.regionEnd());

            if (!mMatcher.lookingAt()) {
                if (alwaysReturn) {
                    return null;
                }

                throw new ParseException("Property or empty line expected", pos());
            }

            mPos = mMatcher.end();

            if (mMatcher.group(1) != null) {
                found = true;
            }
        } while (!found);

        return new Property(mMatcher.group(2), mMatcher.group(3));
    }

    public Property next() throws ParseException {
        return next(false);
    }

    public String remaining() {
        return mStr.substring(mPos);
    }

    public int pos() {
        return mPos + mOffset;
    }

    public static class Property {
        public final String name;
        public final String value;

        public Property(String name, String value) {
            if (name == null || value == null) {
                throw new IllegalArgumentException();
            }

            this.name = name;
            this.value = value;

            Log.v(TAG, toString());
        }

        @Override
        public String toString() {
            return name + ":" + value;
        }

        @Override
        public boolean equals(Object obj) {
            if (!(obj instanceof Property other)) {
                return false;
            }
            return other.name.equals(name) && other.value.equals(value);
        }

        @Override
        public int hashCode() {
            return Objects.hash(name, value);
        }
    }
}
