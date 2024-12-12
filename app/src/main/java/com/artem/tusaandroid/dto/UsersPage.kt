package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable


@Serializable
data class UsersPage(
    val users: List<User>,
    val pagesCount: Int
)