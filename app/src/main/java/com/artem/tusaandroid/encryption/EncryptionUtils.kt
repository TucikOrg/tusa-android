package com.artem.tusaandroid.encryption

import java.util.Base64
import javax.crypto.Cipher
import javax.crypto.KeyGenerator
import javax.crypto.SecretKey


class EncryptionUtils {
    companion object {
        private const val ALGORITHM = "AES"
        private const val KEY_SIZE = 128

        fun keyToString(secretKey: SecretKey): String {
            val keyBytes = secretKey.encoded
            return Base64.getEncoder().encodeToString(keyBytes)
        }

        fun stringToKey(key: String): SecretKey {
            val keyBytes = Base64.getDecoder().decode(key)
            val keySpec = javax.crypto.spec.SecretKeySpec(keyBytes, ALGORITHM)
            return keySpec
        }

        fun generateKey(): SecretKey {
            val keyGen = KeyGenerator.getInstance(ALGORITHM)
            keyGen.init(KEY_SIZE)
            return keyGen.generateKey()
        }

        fun encrypt(data: String, secretKey: SecretKey): String {
            val cipher = Cipher.getInstance(ALGORITHM)
            cipher.init(Cipher.ENCRYPT_MODE, secretKey)
            val encryptedBytes = cipher.doFinal(data.toByteArray(Charsets.UTF_8))
            return Base64.getEncoder().encodeToString(encryptedBytes)
        }

        fun decrypt(encryptedData: String, secretKey: SecretKey): String {
            val cipher = Cipher.getInstance(ALGORITHM)
            cipher.init(Cipher.DECRYPT_MODE, secretKey)
            val decodedBytes = Base64.getDecoder().decode(encryptedData)
            val decryptedBytes = cipher.doFinal(decodedBytes)
            return String(decryptedBytes, Charsets.UTF_8)
        }
    }
}