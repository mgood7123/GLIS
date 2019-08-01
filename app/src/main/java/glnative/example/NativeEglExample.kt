package glnative.example

import android.app.Activity
import android.graphics.Color
import android.os.Bundle
import android.widget.Toast
import android.view.Surface
import android.view.SurfaceView
import android.view.SurfaceHolder
import android.view.View
import android.view.View.OnClickListener
import android.util.Log
import android.widget.RelativeLayout
import android.widget.TextView


class NativeEglExample : Activity() {

    var n: NativeView? = null;

    public override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        if (n == null) n = NativeView(applicationContext);

        Log.i(n!!.TAG, "onCreate()")

        // build layout
        val rel = RelativeLayout(applicationContext)
        rel.addView(n!!.surfaceView!!)

        // DO NOT add text!, adding text will incure screen burn when displayed for long periods of time

        // set layout
        setContentView(rel)

        n!!.surfaceView!!.setOnClickListener {
            val toast = Toast.makeText(this@NativeEglExample,
                    "This demo combines Java UI and native EGL + OpenGL renderer",
                    Toast.LENGTH_LONG)
            toast.show()
        }
    }

    override fun onStart() {
        super.onStart()
        Log.i(n!!.TAG, "onStart()")
        n!!.nativeOnStart()
    }
}