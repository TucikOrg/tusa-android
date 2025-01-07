package com.artem.tusaandroid

import android.content.Context
import java.io.ByteArrayOutputStream
import java.io.File
import java.net.HttpURLConnection
import java.net.URL

class RequestTile(private val context: Context) {
    private fun saveByteArrayToCache(fileName: String, byteArray: ByteArray) {
        val cacheFile = File(context.cacheDir, fileName)
        cacheFile.outputStream().use { it.write(byteArray) }
    }

    private fun readByteArrayFromCache(fileName: String): ByteArray? {
        val cacheFile = File(context.cacheDir, fileName)
        if (!cacheFile.exists()) {
            return null
        }
        return cacheFile.inputStream().use { it.readBytes() }
    }

    fun request(zoom: Int, x: Int, y: Int): ByteArray {
        val fileName = "$zoom-$x-$y.mvt"
        // это чтение кеша из файлов системы
        readByteArrayFromCache(fileName)?.let {
            return it
        }

        try {
            val base = "https://api.mapbox.com/v4/mapbox.mapbox-streets-v8,mapbox.mapbox-terrain-v2/"
            val token = "pk.eyJ1IjoiaW52ZWN0eXMiLCJhIjoiY2w0emRzYWx5MG1iMzNlbW91eWRwZzdldCJ9.EAByLTrB_zc7-ytI6GDGBw"
            val url = URL("$base$zoom/$x/$y.mvt?access_token=$token")
            val result = ByteArrayOutputStream()
            val urlConnection = url.openConnection() as HttpURLConnection
            urlConnection.inputStream.use { inputStream ->
                val buffer = ByteArray(1024)
                var length: Int
                while (inputStream.read(buffer).also { length = it } != -1) {
                    result.write(buffer, 0, length)
                }
            }

            // это запись кеша в файловую систему
            val bytesResult = result.toByteArray()
            saveByteArrayToCache(fileName, bytesResult)
        } catch (e: Exception) {
            // нету интернета и не могу загрузить тайл
        }

        return ByteArray(0)
    }
}