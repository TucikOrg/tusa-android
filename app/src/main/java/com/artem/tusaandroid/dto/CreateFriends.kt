package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class CreateFriends(
    val startFrom: Int,
    val count: Int
)