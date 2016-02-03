package de.devmil.legologic;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.content.Context;

import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;

public class CarHandler extends BluetoothGattCallback {

    public static final UUID UUID_SERVICE = UUID.fromString("40480f29-7bad-4ea5-8bf8-499405c9b324");
    private static final UUID UUID_CHARACTERISTIC_SPEED = UUID.fromString("8d8ba32b-96be-4590-910b-c756c5222c9f");
    private static final UUID UUID_CHARACTERISTIC_STEER = UUID.fromString("7baf8dca-2bfc-47fb-af29-042fccc180eb");

    private boolean mIsConnected;
    private boolean mShouldDisconnect;
    private BluetoothAdapter mAdapter;
    private BluetoothDevice mDevice;
    private BluetoothGatt mGatt;
    private BluetoothGattService mService;


    private String mCarAddress;

    private int mSpeed;
    private int mLastSentSpeed;
    private int mSteering;
    private int mLastSentSteering;
    private Timer mSendTimer;

    public CarHandler(Context context, String carAddress) {
        this.mSpeed = 0;
        this.mSteering = 0;
        mIsConnected = false;
        mShouldDisconnect = false;
        mCarAddress = carAddress;
        BluetoothManager bm = (BluetoothManager)context.getSystemService(Context.BLUETOOTH_SERVICE);
        mAdapter = bm.getAdapter();
        mDevice = mAdapter.getRemoteDevice(carAddress);
        mSendTimer = new Timer();
        mSendTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                if (mIsConnected && isDirty()) {
                    transmitData();
                }
            }
        }, 500, 300);
    }

    public boolean isConnected() {
        return mIsConnected;
    }

    public void setSpeed(int speedPercent) {
        this.mSpeed = speedPercent;
        transmitData();
    }

    public void setSteering(int percentage) {
        this.mSteering = percentage;
        transmitData();
    }

    public void connect(Context context) {
        mShouldDisconnect = false;
        mGatt = mDevice.connectGatt(context, false, this);
        mGatt.connect();
    }

    public void disconnect() {
        mShouldDisconnect = true;
        if(mGatt != null) {
            mGatt.disconnect();
        }
        mGatt = null;
        mService = null;
        mIsConnected = false;
        mLastSentSteering = 0;
        mLastSentSpeed = 0;
    }

    private synchronized boolean isDirty() {
        return mLastSentSpeed != mSpeed
                || mLastSentSteering != mSteering;
    }

    private synchronized boolean transmitData() {
        boolean result = true;
        if(mService != null & mIsConnected) {
            if(mLastSentSpeed != mSpeed) {
                BluetoothGattCharacteristic characteristicSpeed = mService.getCharacteristic(UUID_CHARACTERISTIC_SPEED);
                characteristicSpeed.setValue(new byte[]{(byte) mSpeed});
                if(mGatt.writeCharacteristic(characteristicSpeed)) {
                    mLastSentSpeed = mSpeed;
                } else {
                    result = false;
                }
            }
            if(mLastSentSteering != mSteering) {
                BluetoothGattCharacteristic characteristicSteer = mService.getCharacteristic(UUID_CHARACTERISTIC_STEER);
                characteristicSteer.setValue(new byte[] { (byte)mSteering });
                if(mGatt.writeCharacteristic(characteristicSteer)) {
                    mLastSentSteering = mSteering;
                } else {
                    result = false;
                }
            }
        }
        return result;
    }

    @Override
    public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
        if(newState != BluetoothGatt.STATE_CONNECTED) {
            if(status != 0) {
                try {
                    Thread.sleep(500);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                if(!mShouldDisconnect) {
                    gatt.connect();
                }
            } else {
                mGatt = null;
                disconnect();
            }
        } else {
            mGatt.discoverServices();
        }
    }

    @Override
    public void onServicesDiscovered(BluetoothGatt gatt, int status) {
        if(status != BluetoothGatt.GATT_SUCCESS) {
            mIsConnected = false;
            mService = null;
            return;
        }
        mIsConnected = true;
        mService = gatt.getService(UUID_SERVICE);
        transmitData();
    }
}
