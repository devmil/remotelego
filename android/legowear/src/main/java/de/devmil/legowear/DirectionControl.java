package de.devmil.legowear;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.Outline;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewOutlineProvider;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;

public class DirectionControl extends RelativeLayout {

    private LinearLayout mHandle;
    private boolean mIsActive;
    private int mHorizontalValue;
    private int mVerticalValue;
    private IDirectionChangedListener mDirectionChangedListener;

    private static final int HANDLE_WIDTH_DIP = 60;
    private static final int HANDLE_MARGIN_DIP = 4;

    public interface IDirectionChangedListener {
        void onDirectionChanged(int percentSpeed, int percentSteering);
    }

    public DirectionControl(Context context, AttributeSet attrs) {
        super(context, attrs);
        initialize(context);
    }

    public DirectionControl(Context context) {
        super(context);
        initialize(context);
    }

    public void setDirectionChangedListener(IDirectionChangedListener directionChangedListener) {
        this.mDirectionChangedListener = directionChangedListener;
    }

    class RoundOutlineProvider extends ViewOutlineProvider {

        @Override
        public void getOutline(View view, Outline outline) {
            outline.setOval(0, 0, view.getWidth(), view.getHeight());
        }
    }

    private void initialize(Context context) {
        mIsActive = false;

        mHandle = new LinearLayout(context);
        mHandle.setElevation(6);
        mHandle.setBackgroundColor(Color.WHITE);

        Resources r = getResources();
        float px = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, HANDLE_WIDTH_DIP, r.getDisplayMetrics());

        mHandle.setMinimumHeight((int) px);
        mHandle.setMinimumWidth((int) px);

        mHandle.setOutlineProvider(new RoundOutlineProvider());
        mHandle.setClipToOutline(true);

        LayoutParams params = new LayoutParams((int)px, (int)px);
        params.addRule(RelativeLayout.CENTER_VERTICAL);
        params.addRule(RelativeLayout.CENTER_HORIZONTAL);
        this.addView(mHandle, params);

        setBackgroundColor(Color.LTGRAY);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        boolean result = super.onTouchEvent(event);
        if(event.getAction() == MotionEvent.ACTION_UP) {
            mIsActive = false;
            setAxisValues(0, 0);
        } else if(event.getAction() == MotionEvent.ACTION_DOWN) {
            mIsActive = true;
            int horizontal =  (int)(event.getX() - getWidth() / 2);
            int vertical = (int)(event.getY() - getHeight() / 2);
            setAxisValues(horizontal, vertical);
        } else if(event.getAction() == MotionEvent.ACTION_MOVE) {
            int horizontal =  (int)(event.getX() - getWidth() / 2);
            int vertical = (int)(event.getY() - getHeight() / 2);
            setAxisValues(horizontal, vertical);
        }
        return true;
    }

    private void setAxisValues(int horizontal, int vertical) {
        Resources r = getResources();
        float pxHandleWidth = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, HANDLE_WIDTH_DIP, r.getDisplayMetrics());
        float pxMargin = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, HANDLE_MARGIN_DIP, r.getDisplayMetrics());

        int maxHor = (getWidth() - (int)pxHandleWidth) / 2 - (int)pxMargin;
        int maxVert = (getHeight() - (int)pxHandleWidth) / 2 - (int)pxMargin;

        if(horizontal < -maxHor) {
            horizontal = -maxHor;
        } else if(horizontal > maxHor) {
            horizontal = maxHor;
        }
        if(vertical < -maxVert) {
            vertical = -maxVert;
        } else if(vertical > maxVert) {
            vertical = maxVert;
        }
        mHorizontalValue = horizontal;
        mVerticalValue = vertical;

        mHandle.setTranslationX(mHorizontalValue);
        mHandle.setTranslationY(mVerticalValue);

        if(this.mDirectionChangedListener != null) {
            int percentSteering = ((horizontal * 100) / maxHor);
            int percentSpeed = ((-vertical * 100) / maxVert);

            this.mDirectionChangedListener.onDirectionChanged(percentSpeed, percentSteering);
        }
    }
}
