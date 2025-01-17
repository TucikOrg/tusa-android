package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class RequestPage(
    val page: Int,
    val pageSize: Int
)