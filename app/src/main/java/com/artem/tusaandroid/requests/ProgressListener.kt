package com.artem.tusaandroid.requests

interface ProgressListener {
    fun update(bytesWritten: Long, contentLength: Long, done: Boolean)
}