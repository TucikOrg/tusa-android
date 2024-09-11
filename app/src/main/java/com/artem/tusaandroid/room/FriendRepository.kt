package com.artem.tusaandroid.room

import kotlinx.coroutines.flow.Flow

class FriendRepository(private val friendDao: FriendDao) {
    val allFriends: Flow<List<FriendEntity>> = friendDao.getAllFriends()

    suspend fun insert(user: FriendEntity) {
        friendDao.insert(user)
    }

    suspend fun deleteByInstallAppId(installAppId: String) {
        friendDao.deleteByInstallAppId(installAppId)
    }
}