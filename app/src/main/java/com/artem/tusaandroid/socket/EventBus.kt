package com.artem.tusaandroid.socket

class EventBus<T> {
    private val listeners = mutableListOf<EventListener<T>>()

    fun addListener(listener: EventListener<T>) {
        listeners.add(listener)
    }

    fun removeListener(listener: EventListener<T>) {
        listeners.remove(listener)
    }

    fun pushEvent(event: T) {
        for (listener in listeners) {
            listener.onEvent(event)
        }
    }
}