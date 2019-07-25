package glnative.example

import android.app.Activity
import android.os.Bundle
import android.widget.Toast
import android.view.Surface
import android.view.SurfaceView
import android.view.SurfaceHolder
import android.view.View
import android.view.View.OnClickListener
import android.util.Log


class NativeEglExample : Activity(), SurfaceHolder.Callback {

    public override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        Log.i(TAG, "onCreate()")

        setContentView(R.layout.activity_main)
        val surfaceView = findViewById<View>(R.id.surfaceview) as SurfaceView
        surfaceView.holder.addCallback(this)
        surfaceView.setOnClickListener {
            val toast = Toast.makeText(this@NativeEglExample,
                    "This demo combines Java UI and native EGL + OpenGL renderer",
                    Toast.LENGTH_LONG)
            toast.show()
        }
    }

    override fun onStart() {
        super.onStart()
        Log.i(TAG, "onStart()")
        nativeOnStart()
    }

    override fun onResume() {
        super.onResume()
        Log.i(TAG, "onResume()")
        nativeOnResume()
    }

    override fun onPause() {
        super.onPause()
        Log.i(TAG, "onPause()")
        nativeOnPause()
    }

    override fun onStop() {
        super.onStop()
        Log.i(TAG, "onStop()")
        nativeOnStop()
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, w: Int, h: Int) {
        nativeSetSurface(holder.surface)
    }

    override fun surfaceCreated(holder: SurfaceHolder) {}

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        nativeSetSurface(null)
    }

    // move these out of the companion object to avoid needing to rename JNI methods to Java_glnative_example_NativeEglExample_00024Companion
    external fun nativeOnStart()
    external fun nativeOnResume()
    external fun nativeOnPause()
    external fun nativeOnStop()
    external fun nativeSetSurface(surface: Surface?)

    companion object {

        private val TAG = "EglSample"

        init {
            System.loadLibrary("nativeegl")
        }
    }

}
