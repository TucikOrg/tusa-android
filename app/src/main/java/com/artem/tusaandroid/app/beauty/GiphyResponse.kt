package com.artem.tusaandroid.app.beauty

data class GiphyResponse(
    val data: List<Data>,
)

data class Data(
    val images: Images
)

data class Images(
    val original: Original
)

data class Original(
    val url: String
)

