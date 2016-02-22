package de.devmil.legocontrol;

import com.sjl.util.Foreground;

import de.devmil.legologic.CarHandler;

public class Application extends android.app.Application {

    private CarHandler mCarHandler;

    public synchronized CarHandler getCarHandler() {
        return mCarHandler;
    }

    public synchronized void setActiveCar(String carAddress) {
        if(mCarHandler != null) {
            if(carAddress != null
                    && carAddress.equals(mCarHandler.getAddress())
                    && mCarHandler.isConnected()) {
                //nothing to do, car handler is already in place and active
                return;
            }
            mCarHandler.disconnect();
            mCarHandler = null;
        }
        if(carAddress != null) {
            mCarHandler = new CarHandler(this, carAddress);
            mCarHandler.connect(this);
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Foreground.get(this).addListener(new Foreground.Listener() {
            @Override
            public void onBecameForeground() {
            }

            @Override
            public void onBecameBackground() {
                setActiveCar(null);
            }
        });
    }
}
