package com.artem.tusaandroid.socket

interface EventListener<T> {
    fun onEvent(event: T)
}