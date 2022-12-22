package com.android.bluetooth.bthelper;

import android.app.ActionBar;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;

import androidx.preference.ListPreference;
import androidx.preference.Preference;
import androidx.preference.Preference.OnPreferenceChangeListener;
import androidx.preference.PreferenceFragment;
import androidx.preference.PreferenceManager;
import androidx.preference.SwitchPreference;

import com.android.bluetooth.bthelper.R;

public class BtHelperFragment extends PreferenceFragment implements
        OnPreferenceChangeListener {

    private SharedPreferences mSharedPrefs;
    private SwitchPreference mLowLatencyAudioSwitchPref;
    private ListPreference mScanModeListPref;
    private Preference mScanModeInfoPref;

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        addPreferencesFromResource(R.xml.bthelper);
        final ActionBar mActionBar = getActivity().getActionBar();
        mActionBar.setDisplayHomeAsUpEnabled(true);

        mLowLatencyAudioSwitchPref = (SwitchPreference) findPreference(Constants.KEY_LOW_LATENCY_AUDIO);
        mLowLatencyAudioSwitchPref.setEnabled(true);
        mLowLatencyAudioSwitchPref.setOnPreferenceChangeListener(this);

        mScanModeListPref = (ListPreference) findPreference(Constants.KEY_SCAN_MODE);
        mScanModeListPref.setEnabled(true);
        mScanModeListPref.setOnPreferenceChangeListener(this);

        updateModeDescription();
        mScanModeInfoPref = (Preference) findPreference(Constants.KEY_SCAN_MODE_INFO);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        final View view = LayoutInflater.from(getContext()).inflate(R.layout.bthelper,
                container, false);
        ((ViewGroup) view).addView(super.onCreateView(inflater, container, savedInstanceState));
        return view;
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        if ((Constants.KEY_SCAN_MODE).equals(preference.getKey())) {
            updateModeDescription();
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            getActivity().onBackPressed();
            return true;
        }
        return false;
    }

    private String setModeDescription() {
        mSharedPrefs = PreferenceManager.getDefaultSharedPreferences(getContext());
        String scanMode = mSharedPrefs.getString(Constants.KEY_SCAN_MODE, "0");
        String scanModeDescrpition = null;
        switch (scanMode) {
            case "-1":
                scanModeDescrpition = getString(R.string.scan_mode_opportunistic_description);
                break;
            case "4":
                scanModeDescrpition = getString(R.string.scan_mode_screen_off_description);
                break;
            case "5":
                scanModeDescrpition = getString(R.string.scan_mode_screen_off_balanced_description);
                break;
            case "0":
                scanModeDescrpition = getString(R.string.scan_mode_low_power_description);
                break;
            case "1":
                scanModeDescrpition = getString(R.string.scan_mode_balanced_description);
                break;
            case "3":
                scanModeDescrpition = getString(R.string.scan_mode_ambient_discovery_description);
                break;
            case "2":
                scanModeDescrpition = getString(R.string.scan_mode_low_latency_description);
                break;
        }
        return scanModeDescrpition;
    }

    private void updateModeDescription() {
        Handler.getMain().post(() -> {
            mScanModeInfoPref.setTitle(
                String.format(getString(R.string.scan_mode_description),
                    setModeDescription()));
        });
    }
}
