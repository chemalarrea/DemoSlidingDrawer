package pl.polidea.imagemanager;

import android.app.Application;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.util.AttributeSet;
import android.view.View;

/**
 * Image view using {@link pl.polidea.imagemanager.ImageManager}. This view can be connected to your view hierarchy (no
 * visual editor support yet!) and provides many options to gain drawing performance or use less memory.
 * 
 * @see pl.polidea.imagemanager.ImageManager
 * @author karooolek
 * 
 */
public class ManagedImageView extends View
{

    public static final String TAG = ManagedImageView.class.getSimpleName();

    private static final int IMAGE_REFRESH_TIME = 1000;

    // image drawing settings
    private final ImageManagerRequest req = new ImageManagerRequest();
    private boolean keepRatio = true;
    private boolean fillWholeView = false;

    // drawing variables
    private final Paint p = new Paint();
    private final Matrix m = new Matrix();

    // common managed images view redrawing
    private static long nextDrawT;
    private static Context lastRedrawnContext;

    public ManagedImageView(final Context context)
    {
        super(context);

        if (isInEditMode())
        {
            return;
        }

        ImageManager.init((Application) getContext().getApplicationContext());
    }

    public ManagedImageView(final Context context, final AttributeSet attr)
    {
        super(context, attr);

        if (isInEditMode())
        {
            return;
        }

        ImageManager.init((Application) getContext().getApplicationContext());

        for (int i = 0; i != attr.getAttributeCount(); ++i)
        {
            final String attrName = attr.getAttributeName(i);
            if ("image_filename".equals(attrName))
            {
                setImage(attr.getAttributeValue(i));
            }
            else if ("image_resource".equals(attrName))
            {
                setImage(attr.getAttributeResourceValue(i, 0));
            }
            else if ("image_uri".equals(attrName))
            {
                setImage(Uri.parse(attr.getAttributeValue(i)));
            }
            else if ("subsampling".equals(attrName))
            {
                setSubsampling(attr.getAttributeUnsignedIntValue(i, 1));
            }
            else if ("anti_alias".equals(attrName))
            {
                setAntiAliasing(attr.getAttributeBooleanValue(i, false));
            }
            else if ("keep_ratio".equals(attrName))
            {
                setKeepAspectRatio(attr.getAttributeBooleanValue(i, true));
            }
            else if ("fill_whole_view".equals(attrName))
            {
                setFillWholeView(attr.getAttributeBooleanValue(i, true));
            }
            else if ("preview".equals(attrName))
            {
                setPreviewEnabled(attr.getAttributeBooleanValue(i, true));
            }
            else if ("strong_cache".equals(attrName))
            {
                setKeepStrongCache(attr.getAttributeBooleanValue(i, false));
            }
            // TODO desired dimensions
        }
    }

    /**
     * Set image from file system.
     * 
     * @param filename
     *            image file name in file system.
     */
    public void setImage(final String filename)
    {
        req.filename = filename;
        req.resId = -1;
        req.uri = null;
        redrawManagedImageViews();
    }

    /**
     * Set image from resources.
     * 
     * @param resId
     *            image resource id.
     */
    public void setImage(final int resId)
    {
        req.resId = resId;
        req.filename = null;
        req.uri = null;
        redrawManagedImageViews();
    }

    /**
     * Set image from URI.
     * 
     * @param uri
     *            image URI.
     */
    public void setImage(final Uri uri)
    {
        req.uri = uri;
        req.filename = null;
        req.resId = -1;
        redrawManagedImageViews();
    }

    /**
     * Get sub-sampling value.
     * 
     * @return sub-sampling value. 1 is default value, which means no sub-sampling.
     */
    public int getSubsampling()
    {
        return req.subsample;
    }

    /**
     * Set sub-sampling value.
     * 
     * @param subsample
     *            sub-sampling value. Must be greater or equal 1. 1 means no sub-sampling.
     */
    public void setSubsampling(final int subsample)
    {
        if (subsample < 1)
        {
            return;
        }

        req.subsample = subsample;
    }

    /**
     * Get desired bitmap width.
     * 
     * @return desired loaded bitmap width. -1 is default value, which means that desired width is not specified and
     *         bitmap will be loaded with it's normal size.
     */
    public int getDesiredWidth()
    {
        return req.width;
    }

    /**
     * Get desired bitmap height.
     * 
     * @return desired loaded bitmap height. -1 is default value, which means that desired height is not specified and
     *         bitmap will be loaded with it's normal size.
     */
    public int getDesiredHeight()
    {
        return req.height;
    }

    /**
     * Set desired bitmap dimensions. Bitmap will be loaded and rescaled to desired dimensions. This can be useful when
     * we need to load big images and showed them smaller (ex. thumbnails) to save big amount of memory. Default values
     * are -1 and -1 which means that no rescaling will be done when loading image.
     * 
     * @param width
     *            desired bitmap width.
     * @param height
     *            desired bitmap height.
     */
    public void setDesiredDimensions(final int width, final int height)
    {
        if (width <= 0 || height <= 0)
        {
            return;
        }

        req.width = width;
        req.height = height;
    }

    /**
     * Check if anti-aliasing filter is enabled.
     * 
     * @return true if anti-aliasing filter is enabled, false otherwise.
     */
    public boolean isAntiAliasing()
    {
        return p.isFilterBitmap();
    }

    /**
     * Enable/disable anti-aliasing filter. Enabling anti-aliasing filter is useful when we need higher visual quality
     * when drawing scaled bitmap. However, it has significant impact on performance, especially when drawing big images
     * (ex. full-screen background or gallery). By default anti-aliasing filter is disabled.
     * 
     * @param antiAlias
     *            enable/disable anti-alias filter.
     */
    public void setAntiAliasing(final boolean antiAlias)
    {
        p.setFilterBitmap(antiAlias);
    }

    /**
     * Check if dithering is enabled.
     * 
     * @return true if dithering is enabled, false otherwise.
     */
    public boolean isDithering()
    {
        return p.isDither();
    }

    /**
     * Enable/disable dithering. Dithering is used when bitmap pixel format differs from output buffer format. Enabling
     * dithering will give better colors when writing bitmap to buffer with lower bitrate. Dither by default is
     * disabled.
     * 
     * @param dither
     *            enable/disable dithering.
     */
    public void setDithering(final boolean dither)
    {
        p.setDither(dither);
    }

    /**
     * Check if view is keeping image aspect ratio.
     * 
     * @return true if view is keeping bitmap aspect ratio, false otherwise.
     */
    public boolean isKeepAspectRatio()
    {
        return keepRatio;
    }

    /**
     * Enable/disable keeping image aspect ratio. This should be used together with {@link #setFillWholeView(boolean)}
     * to control scaling options. By default image is keeping aspect ratio.
     * 
     * @see #setFillWholeView(boolean)
     * @param keepRatio
     *            enable/disable keeping image aspect ratio.
     */
    public void setKeepAspectRatio(final boolean keepRatio)
    {
        this.keepRatio = keepRatio;
    }

    /**
     * Check if image is drawn filling whole view.
     * 
     * @return true if image is filling whole view, false otherwise.
     */
    public boolean isFillWholeView()
    {
        return this.fillWholeView;
    }

    /**
     * Enable/disable image filling whole view. When using this option, one should also see
     * {@link #setKeepAspectRatio(boolean)} to control scaling options. By default image is not filling whole view.
     * 
     * @see #setKeepAspectRatio(boolean)
     * @param fillWholeView
     *            enable/disable image filling whole view.
     */
    public void setFillWholeView(final boolean fillWholeView)
    {
        this.fillWholeView = fillWholeView;
    }

    /**
     * Check if image is shown with quick low-quality preview.
     * 
     * @return true if image is shown with sub-sampled preview, false otherwise.
     */
    public boolean isPreviewEnabled()
    {
        return req.preview;
    }

    /**
     * Enable/disable image preview. If this is enabled, image will be shown with quick low-quality preview. Otherwise
     * image won't be visible until it's full loaded. Enabling this option can lead to small performance and memory
     * loss. By default image is shown with preview.
     * 
     * @param preview
     *            enable/disable image preview.
     */
    public void setPreviewEnabled(final boolean preview)
    {
        req.preview = preview;
    }

    /**
     * Check if image is stored using strong cache.
     * 
     * @return true if image is stored using strong cache, false otherwise.
     */
    public boolean isKeepStrongCache()
    {
        return req.strong;
    }

    /**
     * Enable/disable keeping image with strong cache. This is quite dangerous option which should be used rarely and
     * only in image memory demanding applications. Strongly cached images can't be released by GC which can lead to
     * {@link OutOfMemoryError} in random places across application.
     * 
     * @param strong
     *            enable/disable keeping image with strong cache.
     */
    public void setKeepStrongCache(final boolean strong)
    {
        req.strong = strong;
    }

    /**
     * Request image loading immediately.
     * 
     * @return loaded image.
     */
    public Bitmap load()
    {
        return ImageManager.loadImage(req, false);
    }

    /**
     * Request image unloading.
     */
    public void unload()
    {
        ImageManager.unloadImage(req);
    }

    @Override
    public void draw(final Canvas canvas)
    {
        // draw background
        final Drawable bg = getBackground();
        if (bg != null)
        {
            bg.setBounds(0, 0, getWidth(), getHeight());
            bg.draw(canvas);
        }

        // not drawing in edit mode
        if (isInEditMode())
        {
            return;
        }

        // no or invalid image request
        if (req == null || (req.filename == null && req.resId < 0 && req.uri == null))
        {
            return;
        }

        // redraw sometimes
        redrawManagedImageViews();

        // get and clip drawing size
        final int w = getWidth() - getPaddingLeft() - getPaddingRight();
        final int h = getHeight() - getPaddingTop() - getPaddingBottom();
        if (w < 0 || h < 0)
        {
            // lol this is funny ;>
            return;
        }
        canvas.clipRect(getPaddingLeft(), getPaddingTop(), getPaddingLeft() + w, getPaddingTop() + h);

        // get bitmap from manager
        final Bitmap bmp = ImageManager.getImage(req);
        if (bmp == null || bmp.isRecycled())
        {
            return;
        }

        if (bmp.getWidth() == w && bmp.getHeight() == h)
        {
            // bitmap size matches exactle drawing size
            canvas.drawBitmap(bmp, getPaddingLeft(), getPaddingTop(), p);
        }
        else
        {
            // draw rescaled
            final float sx = (float) w / bmp.getWidth();
            final float sy = (float) h / bmp.getHeight();
            if (keepRatio)
            {
                final float s = fillWholeView ? Math.max(sx, sy) : Math.min(sx, sy);
                final float dx = 0.5f * (w - s * bmp.getWidth()) + getPaddingLeft();
                final float dy = 0.5f * (h - s * bmp.getHeight()) + getPaddingTop();
                m.setTranslate(dx, dy);
                m.preScale(s, s);
            }
            else
            {
                m.setTranslate(getPaddingLeft(), getPaddingTop());
                m.preScale(sx, sy);
            }
            canvas.drawBitmap(bmp, m, p);

        }
    }

    /**
     * Redraw visible managed image views. Since view is based on asynchronous it has to refresh it's content every 1
     * second.
     */
    private void redrawManagedImageViews()
    {
        final long t = System.currentTimeMillis();
        final Context context = getContext();
        if (nextDrawT > t && nextDrawT < t + IMAGE_REFRESH_TIME && context == lastRedrawnContext)
        {
            return;
        }
        nextDrawT = t + IMAGE_REFRESH_TIME;
        lastRedrawnContext = getContext();
        getRootView().postInvalidateDelayed(IMAGE_REFRESH_TIME);
        postInvalidateDelayed(IMAGE_REFRESH_TIME);
    }
}
