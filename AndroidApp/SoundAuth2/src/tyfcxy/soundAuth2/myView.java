package tyfcxy.soundAuth2;

import android.view.View;
import android.content.Context;
import android.util.AttributeSet;
import android.graphics.Canvas;
import android.graphics.Bitmap;

class myView extends View
{
    public Bitmap tbitmap;

    public myView(Context  r1)
    {
        super(r1);

        tbitmap = null;
    }

    public myView(Context  r1, AttributeSet  r2)
    {
        super(r1, r2);

        tbitmap = null;
    }

    public myView(Context  r1, AttributeSet  r2, int  i0)
    {
        super(r1, r2, i0);

        tbitmap = null;
    }

    public void onDraw(Canvas  r1)
    {
        if (tbitmap != null)
        {
            r1.drawBitmap(tbitmap, 0.0F, 0.0F, null);
        }
    }
}
