package com.artem.tusaandroid

import android.os.AsyncTask

class UploadAvatarAsyncTask : AsyncTask<Void, Void, String>() {
    override fun doInBackground(vararg params: Void?): String {
        // Perform background operations here
        return "Result"
    }

    override fun onPostExecute(result: String) {
        super.onPostExecute(result)
        // Update UI with the result
    }
}