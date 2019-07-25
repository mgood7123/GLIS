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

        val text = TextView(applicationContext);
        text.text = "Hello World! Try clicking the screen"
        text.setTextSize(60f)
        text.setTextColor(Color.WHITE)
        rel.addView(text)

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

    override fun onResume() {
        super.onResume()
        Log.i(n!!.TAG, "onResume()")
        n!!.nativeOnResume()
    }

    override fun onPause() {
        super.onPause()
        Log.i(n!!.TAG, "onPause()")
        n!!.nativeOnPause()
    }

    override fun onStop() {
        super.onStop()
        Log.i(n!!.TAG, "onStop()")
        n!!.nativeOnStop()
    }
}
