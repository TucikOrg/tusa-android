package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class RequestChats(
    val page: Int,
    val size: Int
)