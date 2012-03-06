CWAC LocationPoller: Asking "Where Am I?" Over and Over Again
=============================================================

Some applications wish to know where the device is at on a
periodic basis, whether the device is awake or asleep. This
is a bit challenging, since you have to keep the device awake
(via a `WakeLock`) long enough to get the location fix, but
not forever in case the location simply is unavailable (e.g.,
user is in a place where GPS signals cannot be received).

`LocationPoller` wraps up some code to implement this in a
reusable package. You simply set up an `AlarmManager` alarm
to contact `LocationPoller` on whatever frequency you wish,
and it will handle all of the location work from there, sending
you the results via a broadcast `Intent`. Your `BroadcastReceiver`
can then use the location data as needed.

This is available as a JAR file from the downloads area of this GitHub repo.
The project itself is set up as an Android library project,
in case you wish to use the source code in that fashion.

Usage
-----
First, you need to add the JAR or Android library project to
your project.

Second, you need to add the following to your manifest:

			<receiver android:name="com.commonsware.cwac.locpoll.LocationPoller" />
			<service android:name="com.commonsware.cwac.locpoll.LocationPollerService" />

Your manifest will also need the `WAKE_LOCK` permission along with
whatever permission is required for the location provider
you wish to use (e.g., `ACCESS_FINE_LOCATION`).

Next, you need to create an alarm via `AlarmManager`, so you
can control how frequently the location is retrieved and whether
it should wake up the device if it is asleep. Please do not
request location updates frequently, as this will drain the user's
battery, particularly if you are using GPS.

The `PendingIntent` you use for the alarm should be a getBroadcast()
`PendingIntent`, wrapping an `Intent` destined for
`com.commonsware.cwac.locpoll.LocationPoller`. That `Intent` should
have one extra, keyed by `LocationPoller.EXTRA_INTENT`, that
represents an `Intent` to be "broadcast" when a location is
found. It should have another extra, keyed by
`LocationPoller.EXTRA_PROVIDER`, with the name of the location
provider you wish to use.

For example, this sets up such an alarm:

		mgr=(AlarmManager)getSystemService(ALARM_SERVICE);
		
		Intent i=new Intent(this, LocationPoller.class);
		
		i.putExtra(LocationPoller.EXTRA_INTENT,
							 new Intent(this, LocationReceiver.class));
		i.putExtra(LocationPoller.EXTRA_PROVIDER,
							 LocationManager.NETWORK_PROVIDER);
		
		pi=PendingIntent.getBroadcast(this, 0, i, 0);
		mgr.setRepeating(AlarmManager.ELAPSED_REALTIME_WAKEUP,
											SystemClock.elapsedRealtime(),
											PERIOD,
											pi);

Finally, you need a `BroadcastReceiver` set up to respond to the
`Intent` you supplied via the `LocationPoller.EXTRA_INTENT`
extra. The `Intent` received by the receiver in `onReceive()`
will have either a `LocationPoller.EXTRA_LOCATION` extra
(containing a `Location`) or a `LocationPoller.EXTRA_ERROR`
extra (containing a `String` with an error message). For example:

			Location loc=(Location)intent.getExtras().get(LocationPoller.EXTRA_LOCATION);
			String msg;
			
			if (loc==null) {
				msg=intent.getStringExtra(LocationPoller.EXTRA_ERROR);
			}
			else {
				msg=loc.toString();
			}
			
			if (msg==null) {
				msg="Invalid broadcast received!";
			}

In the case where you get an error message via `EXTRA_ERROR`, there
will also be an extra named `EXTRA_LASTKNOWN`, containing the results
of `getLastKnownLocation()` for your selected provider. This may be
`null` &mdash; if not, it will be a `Location` object.

Dependencies
------------
This project has no dependencies.

Version
-------
This is version 0.2 of this module, meaning it is less new than before.

Demo
----
In the `demo/` sub-project you will find
a sample activity that demonstrates the use of `LocationPoller`.

Note that when you build the JAR via `ant jar`, the sample
activity is not included, nor any resources -- only the
compiled classes for the actual library are put into the JAR.

License
-------
The code in this project is licensed under the Apache
Software License 2.0, per the terms of the included LICENSE
file.

Questions
---------
If you have questions regarding the use of this code, please post a question
on [StackOverflow](http://stackoverflow.com/questions/ask) tagged with `commonsware` and `android`. Be sure to indicate
what CWAC module you are having issues with, and be sure to include source code 
and stack traces if you are encountering crashes.

If you have encountered what is clearly a bug, please post an [issue](https://github.com/commonsguy/cwac-locpoll/issues). Be certain to include complete steps
for reproducing the issue.

Do not ask for help via Twitter.

Release Notes
-------------
v0.2.0: added `EXTRA_LASTKNOWN` support
v0.1.0: initial release
