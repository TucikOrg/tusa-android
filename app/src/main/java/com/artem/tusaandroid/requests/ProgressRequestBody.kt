package com.artem.tusaandroid.requests

import okhttp3.MediaType
import okhttp3.RequestBody
import okio.Buffer
import okio.BufferedSink
import okio.ForwardingSink
import okio.Sink
import okio.buffer

class ProgressRequestBody(
    private val delegate: RequestBody,
    private val progressListener: ProgressListener
) : RequestBody() {

    override fun contentType(): MediaType? {
        return delegate.contentType()
    }

    override fun contentLength(): Long {
        return delegate.contentLength()
    }

    override fun writeTo(sink: BufferedSink) {
        val bufferedSink = sink(sink).buffer()
        delegate.writeTo(bufferedSink)
        bufferedSink.flush()
    }

    private fun sink(sink: Sink): Sink {
        return object : ForwardingSink(sink) {
            private var totalBytesWritten = 0L
            private var completed = false

            override fun write(source: Buffer, byteCount: Long) {
                super.write(source, byteCount)
                totalBytesWritten += byteCount
                progressListener.update(totalBytesWritten, contentLength(), completed)
            }

            override fun close() {
                super.close()
                if (!completed) {
                    completed = true
                    progressListener.update(totalBytesWritten, contentLength(), completed)
                }
            }
        }
    }
}