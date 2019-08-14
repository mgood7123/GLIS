package glnative.example

import android.content.res.AssetManager
import android.util.Log
import java.io.*

class AssetsManager {

    val LOG_TAG = "AssetsManager"

    fun copyAssetFolder(
            assetManager: AssetManager,
            toPath: String
    ): Boolean = copyAssetFolder(assetManager, null, toPath)

    fun copyAssetFolder(
            assetManager: AssetManager,
            fromAssetPath: String?,
            toPath: String,
            directoryPrintsLast: Boolean = false
    ): Boolean {
        try {
            val files: Array<String>? = assetManager.list(if (fromAssetPath.isNullOrBlank()) "" else fromAssetPath)
            if (files == null) return false else if (files.isEmpty()) return false
            File(toPath).mkdirs()
            if (!directoryPrintsLast) Log.i(LOG_TAG, "'${toPath.substringAfterLast('/')}' -> '$toPath'")
            var copied = 0
            val total = files.size
            for (file in files) {
                val asset = if (fromAssetPath.isNullOrBlank()) file else "$fromAssetPath/$file"
                var isFile = true
                try {
                    assetManager.open(asset).close()
                } catch (e: FileNotFoundException) {
                    isFile = false
                }
                if (if (isFile) copyAsset(assetManager, asset, "$toPath/$file")
                        else copyAssetFolder(assetManager, asset, "$toPath/$file")) copied++
            }
            if (directoryPrintsLast) Log.i(LOG_TAG, "'${toPath.substringAfterLast('/')}' -> '$toPath'")
            return copied == total
        } catch (e: Exception) {
            e.printStackTrace()
            return false
        }
    }

    fun copyAsset(
            assetManager: AssetManager,
            fromAssetPath: String, toPath: String
    ): Boolean {
        var `in`: InputStream? = null
        var out: OutputStream? = null
        try {
            `in` = assetManager.open(fromAssetPath)
            File(toPath).createNewFile()
            out = FileOutputStream(toPath)
            copyFile(`in`, out)
            Log.i(LOG_TAG, "'$fromAssetPath' -> '$toPath'")
            `in`.close()
            `in` = null
            out.flush()
            out.close()
            out = null
            return true
        } catch (e: Exception) {
            e.printStackTrace()
            return false
        }
    }

    @Throws(IOException::class)
    fun copyFile(`in`: InputStream, out: OutputStream) {
        val buffer = ByteArray(1024)
        var read = `in`.read(buffer)
        while (read != -1) {
            out.write(buffer, 0, read)
            read = `in`.read(buffer)
        }
    }
}
