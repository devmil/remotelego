package de.devmil.legowear;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.content.Context;
import android.os.Bundle;
import android.os.ParcelUuid;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.wearable.view.WatchViewStub;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends Activity {

    private RecyclerView mRecyclerView;

    private BluetoothAdapter mAdapter;
    private BluetoothLeScanner mScanner;
    private ScanSettings mScanSettings;
    private List<ScanFilter> mScanFilter;

    private EntriesAdapter mRecyclerAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mRecyclerAdapter = new EntriesAdapter();
        final WatchViewStub stub = (WatchViewStub) findViewById(R.id.watch_view_stub);
        stub.setOnLayoutInflatedListener(new WatchViewStub.OnLayoutInflatedListener() {
            @Override
            public void onLayoutInflated(WatchViewStub stub) {
                mRecyclerView = (RecyclerView)findViewById(R.id.activity_main_recyclerview);
                mRecyclerView.setLayoutManager(new LinearLayoutManager(stub.getContext()));
                mRecyclerView.setAdapter(mRecyclerAdapter);
            }
        });
        BluetoothManager bm = (BluetoothManager)getSystemService(Context.BLUETOOTH_SERVICE);
        mAdapter = bm.getAdapter();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mScanner = mAdapter.getBluetoothLeScanner();
        mScanSettings = new ScanSettings.Builder()
                .setScanMode(ScanSettings.SCAN_MODE_BALANCED)
                .build();
        mScanFilter = new ArrayList<>();
        mScanFilter.add(new ScanFilter.Builder().setServiceUuid(new ParcelUuid(CarHandler.UUID_SERVICE)).build());
        mScanner.startScan(mScanFilter, mScanSettings, new ScanCallback() {
            @Override
            public void onScanResult(int callbackType, ScanResult result) {
                super.onScanResult(callbackType, result);
                mRecyclerAdapter.addEntry(result.getDevice());
            }

            @Override
            public void onBatchScanResults(List<ScanResult> results) {
                super.onBatchScanResults(results);
            }

            @Override
            public void onScanFailed(int errorCode) {
                super.onScanFailed(errorCode);
            }
        });
    }

    @Override
    protected void onPause() {
        super.onPause();
        mScanner.stopScan(new ScanCallback() {
        });
    }
}

class Entry {
    private BluetoothDevice mDevice;
    private String mName;

    public Entry(BluetoothDevice device) {
        mDevice = device;
        mName = device.getName();
    }

    public String getName() {
        return mName;
    }

    public boolean is(BluetoothDevice otherDevice) {
        return otherDevice.getAddress().equals(mDevice.getAddress());
    }

    public BluetoothDevice getDevice() {
        return mDevice;
    }
}

class EntriesViewHolder extends RecyclerView.ViewHolder {

    private TextView mNameTextView;
    private Button mConnectButton;

    public EntriesViewHolder(View itemView) {
        super(itemView);
        mNameTextView = (TextView)itemView.findViewById(R.id.activity_main_entry_nametext);
        mConnectButton = (Button)itemView.findViewById(R.id.activity_main_entry_btn);
    }

    public void bind(final Entry entry) {
        mNameTextView.setText(entry.getName());
        mConnectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                v.getContext().startActivity(ControlActivity.getLaunchIntent(v.getContext(), entry.getDevice()));
            }
        });
    }
}

class EntriesAdapter extends RecyclerView.Adapter<EntriesViewHolder> {

    private List<Entry> mEntries;

    public EntriesAdapter() {
        mEntries = new ArrayList<>();
    }

    public void addEntry(BluetoothDevice device) {
        for(Entry e : mEntries) {
            if(e.is(device)) {
                return;
            }
        }
        mEntries.add(new Entry(device));
        notifyItemInserted(mEntries.size() - 1);
    }

    @Override
    public EntriesViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.activity_main_entry, parent, false);
        return new EntriesViewHolder(view);
    }

    @Override
    public void onBindViewHolder(EntriesViewHolder holder, int position) {
        holder.bind(mEntries.get(position));
    }

    @Override
    public int getItemCount() {
        return mEntries.size();
    }
}