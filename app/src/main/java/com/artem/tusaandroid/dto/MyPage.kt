package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class MyPage<Item>(
    val items: List<Item>,
    val page: Int,
    val totalPages: Int
)