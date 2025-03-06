package com.artem.tusaandroid.app.chat

class MessagesConsts {
    companion object {
        const val batchSize = 50

        fun isGifUrl(url: String): Boolean {
            if (url.endsWith(".gif") || url.endsWith(".webp")) {
                return true
            }

            val regex = "https://media\\d\\.giphy\\.com/media.*".toRegex()
            return url.contains(".gif") && regex.matches(url)
        }

        fun findUrls(text: String):  Sequence<MatchResult> {
            val urlPattern = "(https?://[\\w.-]+(:\\d+)?(/[\\w-./?%&=]*)?)".toRegex()
            return urlPattern.findAll(text)
        }
    }
}