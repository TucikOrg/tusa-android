package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class AllUsersRequest (
    val page: Int,
    val size: Int,
    val uniqueName: String
)