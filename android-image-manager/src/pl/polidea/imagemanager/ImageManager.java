package pl.polidea.imagemanager;

import java.io.File;
import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingQueue;

import android.app.Application;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.BitmapFactory.Options;
import android.graphics.Point;
import android.util.Log;

/**
 * Image manager. Manager provides way to load image resources asynchronously with many options like:
 * <ul>
 * <li>loading from
 * <ul>
 * <li>file system
 * <li>application resources
 * </ul>
 * <li>caching
 * <li>low-quality preview
 * <li>sub-sampling
 * <li>loading rescaled bitmap
 * <li>using strong GC-proof cache
 * </ul>
 * Provides optional logging on different levels which makes it easy to debug your code. Image manager should be
 * interfaced mostly by {@link #getImage(ImageManagerRequest)} or using {@link pl.polidea.imagemanager.ManagedImageView}
 * 
 * 
 * @author karooolek
 * @see #getImage(ImageManagerRequest)
 * @see pl.polidea.imagemanager.ManagedImageView
 */
public final class ImageManager
{
    private static final String TAG = ImageManager.class.getSimpleName();

    /**
     * Image load thread helper class.
     * 
     * @author karooolek
     */
    private static final class LoadThread extends Thread
    {
        private LoadThread()
        {
            super(TAG);
        }

        @Override
        public void run()
        {
            if (logging)
            {
                Log.d(TAG, "Image loading thread started");
            }

            // loop
            final boolean exit = false;
            while (!exit)
            {
                // get loading request
                ImageManagerRequest req = null;
                try
                {
                    loadingReqs.add(req = loadQueue.take());
                }
                catch (final InterruptedException e)
                {
                    break;
                }

                try
                {
                    // load bitmap
                    final Bitmap bmp = loadImage(req, false);

                    // remove preview image
                    if (isImageLoaded(req))
                    {
                        final Bitmap prevbmp = getLoadedBitmap(req);
                        if (prevbmp != null && !prevbmp.isRecycled())
                        {
                            if (logging)
                            {
                                Log.d(TAG, "Unloading preview image " + req);
                            }

                            prevbmp.recycle();

                            if (logging)
                            {
                                Log.d(TAG, "Preview image " + req + " unloaded");
                            }
                        }
                    }

                    // save bitmap
                    loaded.put(req, new LoadedBitmap(bmp, req.strong));
                }
                catch (final OutOfMemoryError err)
                {
                    // oh noes! we have no memory for image
                    if (logging)
                    {
                        Log.e(TAG, "Error while loading full image " + req + ". Out of memory.");
                        logImageManagerStatus();
                    }

                    cleanUp();
                }

                loadingReqs.remove(req);
            } // while(!exit)

            if (logging)
            {
                Log.d(TAG, "Image loading thread ended");
            }
        }
    }

    /**
     * Loaded bitmap helper class.
     * 
     * @author karooolek
     */
    private static final class LoadedBitmap
    {
        private final WeakReference<Bitmap> weakBitmap;
        private final Bitmap bitmap;

        LoadedBitmap(final Bitmap bitmap, final boolean strong)
        {
            this.bitmap = strong ? bitmap : null;
            this.weakBitmap = strong ? null : new WeakReference<Bitmap>(bitmap);
        }

        Bitmap getBitmap()
        {
            return weakBitmap == null ? bitmap : weakBitmap.get();
        }
    }

    private static Application application;
    private static long start;
    private static boolean logging = false;
    private static BlockingQueue<ImageManagerRequest> loadQueue = new LinkedBlockingQueue<ImageManagerRequest>();
    private static List<ImageManagerRequest> loadingReqs = new ArrayList<ImageManagerRequest>();
    private static Map<ImageManagerRequest, LoadedBitmap> loaded = new ConcurrentHashMap<ImageManagerRequest, LoadedBitmap>();

    private ImageManager()
    {
        // unreachable private constructor
    }

    /**
     * Initialize image manager for application.
     * 
     * @param application
     *            application context.
     */
    public static void init(final Application application)
    {
        ImageManager.application = application;
    }

    private static boolean isImageLoaded(final ImageManagerRequest req)
    {
        return loaded.containsKey(req);
    }

    private static boolean isImageLoading(final ImageManagerRequest req)
    {
        return loadQueue.contains(req) || loadingReqs.contains(req);
    }

    private static void queueImageLoad(final ImageManagerRequest req)
    {
        if (logging)
        {
            Log.d(TAG, "Queuing image " + req + " to load");
        }
        loadQueue.add(req);
    }

    private static Bitmap getLoadedBitmap(final ImageManagerRequest req)
    {
        return isImageLoaded(req) ? loaded.get(req).getBitmap() : null;
    }

    /**
     * Load image request. Loads synchronously image specified by request. Adds loaded image to cache.
     * 
     * @param req
     *            image request
     * @param preview
     *            loading preview or not.
     * @return loaded image.
     */
    public static Bitmap loadImage(final ImageManagerRequest req, final boolean preview)
    {
        // no request
        if (req == null)
        {
            return null;
        }

        if (logging)
        {
            Log.d(TAG, "Loading " + (preview ? "preview" : "full") + " image " + req);
        }

        Bitmap bmp = null;

        // loading options
        final Options opts = new Options();

        // sub-sampling options
        opts.inSampleSize = preview ? 8 : req.subsample;

        // load from filename
        if (req.filename != null)
        {
            final File file = new File(req.filename);
            if (!file.exists() || file.isDirectory())
            {
                if (logging)
                {
                    Log.e(TAG, "Error while loading image " + req + ". File does not exist.");
                }
                return null;
            }

            bmp = BitmapFactory.decodeFile(req.filename, opts);
            if (bmp == null && logging)
            {
                Log.e(TAG, "Error while decoding image from file " + req.filename);
            }
        }

        // load from resources
        else if (req.resId >= 0)
        {
            bmp = BitmapFactory.decodeResource(application.getResources(), req.resId, opts);
            if (bmp == null && logging)
            {
                Log.e(TAG, "Error while decoding image from resources id=" + req.resId);
            }
        }

        // load from uri
        else if (req.uri != null)
        {
            try
            {
                final InputStream is = new URL(req.uri.toString()).openStream();
                bmp = BitmapFactory.decodeStream(is, null, opts);
                is.close();
                if (bmp == null && logging)
                {
                    Log.e(TAG, "Error while decoding image from uri " + req.uri);
                }
            }
            catch (final Exception e)
            {
                if (logging)
                {
                    Log.e(TAG, "Error while decoding image from uri " + req.uri);
                }
            }
        }

        // error while decoding
        if (bmp == null)
        {
            return null;
        }

        // rescaling
        if (!preview && (req.width > 0 && req.height > 0))
        {
            final Bitmap sBmp = Bitmap.createScaledBitmap(bmp, req.width, req.height, true);
            if (sBmp != null)
            {
                bmp.recycle();
                bmp = sBmp;
            }
        }

        if (logging)
        {
            Log.d(TAG, (preview ? "Preview" : "Full") + " image " + req + " loaded");
        }

        return bmp;
    }

    /**
     * Unload image specified by image request and remove it from cache.
     * 
     * @param req
     *            image request.
     */
    public static void unloadImage(final ImageManagerRequest req)
    {
        if (logging)
        {
            Log.d(TAG, "Unloading image " + req);
        }

        final Bitmap bmp = getLoadedBitmap(req);
        if (bmp != null)
        {
            bmp.recycle();
        }
        loaded.remove(req);

        if (logging)
        {
            Log.d(TAG, "Image " + req + " unloaded");
        }
    }

    /**
     * Clean up image manager. Unloads all cached images.
     */
    public static synchronized void cleanUp()
    {
        final long t = System.currentTimeMillis();

        if (logging)
        {
            Log.d(TAG, "Image manager clean up");
        }

        // unload all images
        final Set<ImageManagerRequest> reqs = loaded.keySet();
        for (final ImageManagerRequest req : reqs)
        {
            unloadImage(req);
        }

        if (logging)
        {
            Log.d(TAG, "Image manager clean up finished, took " + (System.currentTimeMillis() - t) + "[msec]");
            logImageManagerStatus();
        }
    }

    /**
     * Check if image manager logging is enabled. By default logging is disabled.
     * 
     * @return true if image manager logging is enabled, false otherwise.
     */
    public static boolean isLoggingEnabled()
    {
        return logging;
    }

    /**
     * Enable/disable image manager logging.
     * 
     * @param logging
     *            enable/disable image manager logging.
     */
    public static void setLoggingEnabled(final boolean logging)
    {
        ImageManager.logging = logging;
    }

    /**
     * Log image manager current status. Logs:
     * <ul>
     * <li>manager uptime in seconds
     * <li>all loaded images details
     * <li>used memory
     * </ul>
     */
    public static void logImageManagerStatus()
    {
        final float t = 0.001f * (System.currentTimeMillis() - start);

        Log.d(TAG, "Uptime: " + t + "[s]");

        // count loaded images
        final int imgn = loaded.size();
        Log.d(TAG, "Loaded images: " + imgn);
        if (imgn > 0)
        {
            int totalSize = 0;
            for (final LoadedBitmap limg : loaded.values())
            {
                final Bitmap bmp = limg.getBitmap();

                // no bitmap
                if (bmp == null)
                {
                    continue;
                }

                // get bits per pixel
                int bpp = 0;
                if (bmp.getConfig() != null)
                {
                    switch (bmp.getConfig())
                    {
                    case ALPHA_8:
                        bpp = 1;
                        break;
                    case RGB_565:
                    case ARGB_4444:
                        bpp = 2;
                        break;
                    case ARGB_8888:
                    default:
                        bpp = 4;
                        break;
                    }
                }

                // count total size
                totalSize += bmp.getWidth() * bmp.getHeight() * bpp;
            }

            Log.d(TAG, "Estimated loaded images size: " + totalSize / 1024 + "[kB]");
        }

        // count queued images
        Log.d(TAG, "Queued images: " + loadQueue.size());
    }

    /**
     * Get size of image specified by image request.
     * 
     * @param req
     *            image request.
     * @return image dimensions.
     */
    public static Point getImageSize(final ImageManagerRequest req)
    {
        final Options opts = new Options();
        opts.inJustDecodeBounds = true;
        if (req.filename != null)
        {
            BitmapFactory.decodeFile(req.filename, opts);
        }
        if (req.resId >= 0)
        {
            BitmapFactory.decodeResource(application.getResources(), req.resId, opts);
        }
        if (req.uri != null)
        {
            try
            {
                final InputStream is = new URL(req.uri.toString()).openStream();
                BitmapFactory.decodeStream(is, null, opts);
                is.close();
            }
            catch (final Exception e)
            {
                // nothing
            }
        }
        return new Point(opts.outWidth, opts.outHeight);
    }

    /**
     * Get image specified by image request. This returns image as currently available in manager, which means:
     * <ul>
     * <li>not loaded at all: NULL - no image
     * <li>loaded preview
     * <li>loaded full
     * </ul>
     * If image is not available in cache, image request is posted to asynchronous loading and will be available soon.
     * All image options are specified in image request.
     * 
     * @param req
     *            image request.
     * @return image as currently available in manager (preview/full) or NULL if it's not available at all.
     * @see pl.polidea.imagemanager.ImageManagerRequest
     */
    public static Bitmap getImage(final ImageManagerRequest req)
    {
        Bitmap bmp = null;

        // look for bitmap in already loaded resources
        if (isImageLoaded(req))
        {
            bmp = getLoadedBitmap(req);
        }

        // bitmap found
        if (bmp != null)
        {
            return bmp;
        }

        // load preview image quickly
        if (req.preview)
        {
            try
            {
                bmp = loadImage(req, true);
                if (bmp == null)
                {
                    return null;
                }

                // save preview image
                loaded.put(req, new LoadedBitmap(bmp, req.strong));
            }
            catch (final OutOfMemoryError err)
            {
                // oh noes! we have no memory for image
                if (logging)
                {
                    Log.e(TAG, "Error while loading preview image " + req + ". Out of memory.");
                    logImageManagerStatus();
                }
            }
        }

        // add image to loading queue
        if (!isImageLoading(req))
        {
            queueImageLoad(req);
        }

        return bmp;
    }

    static
    {
        // save starting time
        start = System.currentTimeMillis();

        // start 7 loading threads
        new LoadThread().start();
        new LoadThread().start();
        new LoadThread().start();
        new LoadThread().start();
        new LoadThread().start();
        new LoadThread().start();
        new LoadThread().start();
    }
}
