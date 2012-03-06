package it.sephiroth.demo.slider;

import it.sephiroth.demo.slider.widget.MultiDirectionSlidingDrawer;
import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.Button;

public class MainActivity extends Activity {

	Button mCloseButton;
	Button mOpenButton;
	MultiDirectionSlidingDrawer mDrawer;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature( Window.FEATURE_NO_TITLE );
        setContentView(R.layout.main);
        
        mCloseButton.setOnClickListener( new OnClickListener() {
			@Override
			public void onClick( View v )
			{
				mDrawer.animateClose();
			}
		});
        
        mOpenButton.setOnClickListener( new OnClickListener() {
			
			@Override
			public void onClick( View v )
			{
				if( !mDrawer.isOpened() )
					mDrawer.animateOpen();
			}
		});
    }
    
    @Override
   public void onContentChanged()
   {
   	super.onContentChanged();
   	mCloseButton = (Button) findViewById( R.id.button_close );
   	mOpenButton = (Button) findViewById( R.id.button_open );
   	mDrawer = (MultiDirectionSlidingDrawer) findViewById( R.id.drawer );
   }
}