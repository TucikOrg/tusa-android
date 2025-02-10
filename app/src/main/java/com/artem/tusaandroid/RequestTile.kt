package com.artem.tusaandroid

import android.content.Context
import java.io.ByteArrayOutputStream
import java.io.File
import java.net.HttpURLConnection
import java.net.URL

class RequestTile(private val context: Context) {

    var testPbf = byteArrayOf()

    init {
        context.assets.open("mvt/test.pbf").use { inputStream ->
            testPbf = inputStream.readBytes()
        }
    }

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
//        readByteArrayFromCache(fileName)?.let {
//            return it
//        }

        try {
            val base = "http://192.168.0.103:8081/api/v1/tile/"
            val url = URL("$base$zoom/$x/$y.mvt")
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
            return bytesResult
        } catch (e: Exception) {
            // нету интернета и не могу загрузить тайл
            val t = 0
        }

        return ByteArray(1) // 1 означает что произошла ошибка при загрузке
        // в C++ проверяю размер массива и если он равен 1 то это ошибка
    }
}