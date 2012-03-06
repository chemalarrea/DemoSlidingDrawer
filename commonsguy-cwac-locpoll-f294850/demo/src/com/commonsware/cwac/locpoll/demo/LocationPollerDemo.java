/***
	Copyright (c) 2010 CommonsWare, LLC
	
	Licensed under the Apache License, Version 2.0 (the "License"); you may
	not use this file except in compliance with the License. You may obtain
	a copy of the License at
		http://www.apache.org/licenses/LICENSE-2.0
	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

package com.commonsware.cwac.locpoll.demo;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.SystemClock;
import android.view.View;
import android.widget.Toast;
import com.commonsware.cwac.locpoll.LocationPoller;

public class LocationPollerDemo extends Activity {
	private static final int PERIOD=1800000; 	// 30 minutes
	private PendingIntent pi=null;
	private AlarmManager mgr=null;
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
			
		mgr=(AlarmManager)getSystemService(ALARM_SERVICE);
		
		Intent i=new Intent(this, LocationPoller.class);
		
		i.putExtra(LocationPoller.EXTRA_INTENT,
							 new Intent(this, LocationReceiver.class));
		i.putExtra(LocationPoller.EXTRA_PROVIDER,
							 LocationManager.GPS_PROVIDER);
		
		pi=PendingIntent.getBroadcast(this, 0, i, 0);
		mgr.setRepeating(AlarmManager.ELAPSED_REALTIME_WAKEUP,
											SystemClock.elapsedRealtime(),
											PERIOD,
											pi);
		
		Toast
			.makeText(this,
								"Location polling every 30 minutes begun",
								Toast.LENGTH_LONG)
			.show();
  }
	
	public void omgPleaseStop(View v) {
		mgr.cancel(pi);
		finish();
	}
}
