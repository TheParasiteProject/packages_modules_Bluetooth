package com.android.bluetooth.bthelper;

import android.os.Bundle;

import com.android.settingslib.collapsingtoolbar.CollapsingToolbarBaseActivity;
import com.android.settingslib.widget.R;

public class BtHelperActivity extends CollapsingToolbarBaseActivity {

    private static final String TAG = "BtHelper";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getFragmentManager().beginTransaction().replace(R.id.content_frame,
                new BtHelperFragment(), TAG).commit();
    }
}
