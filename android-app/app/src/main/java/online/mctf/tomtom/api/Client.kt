package online.mctf.tomtom.api

import android.app.Activity
import android.util.Log
import com.google.flatbuffers.FlatBufferBuilder
import okhttp3.*
import okhttp3.RequestBody.Companion.toRequestBody
import online.mctf.tomtom.R
import online.mctf.tomtom.api.models.Request.*
import java.io.IOException
import java.nio.ByteBuffer
import java.security.KeyStore
import java.security.SecureRandom
import java.security.cert.CertificateFactory
import java.util.concurrent.TimeUnit
import javax.net.ssl.*

class Client(
    private val context: Activity,
    private val baseUrl: String
) {
    private lateinit var sslSocketFactory: SSLSocketFactory
    private lateinit var x509TrustManager: X509TrustManager

    private var client: OkHttpClient?

    init {
        createPinnedSocketFactory()
        client = OkHttpClient.Builder()
            .connectTimeout(5, TimeUnit.SECONDS)
            .readTimeout(5, TimeUnit.SECONDS)
            .writeTimeout(5, TimeUnit.SECONDS)
            .sslSocketFactory(sslSocketFactory, x509TrustManager)
            .hostnameVerifier(HostnameVerifier { _, _ -> true })
            .build()
    }

    private fun createPinnedSocketFactory() {
        try {
            val certFactory = CertificateFactory.getInstance("X.509")

            val iStream = context.resources.openRawResource(R.raw.cert)
            val cert = certFactory.generateCertificate(iStream)
            iStream.close()

            val keyStoreType = KeyStore.getDefaultType()
            val keyStore = KeyStore.getInstance(keyStoreType)
            keyStore.load(null, null) // ???
            keyStore.setCertificateEntry("ca", cert)

            val tmfAlgo = TrustManagerFactory.getDefaultAlgorithm()
            val tmf = TrustManagerFactory.getInstance(tmfAlgo)
            tmf.init(keyStore)

            val trustManagers = tmf.trustManagers
            x509TrustManager = trustManagers[0] as X509TrustManager

            val sslContext = SSLContext.getInstance("SSL")
            sslContext.init(null, arrayOf(x509TrustManager), SecureRandom())
            sslSocketFactory = sslContext.socketFactory
        } catch (e: Exception) {
            Log.e(TAG, "Failed to init SSL Context:", e)
        }
    }

    fun login(
        username: String, password: String,
        onFailure: (reason: String?) -> Unit,
        onSuccess: (response: online.mctf.tomtom.api.models.Response.Message) -> Unit
    ) {
        val b = FlatBufferBuilder(128)

        val usernameB = b.createString(username)
        val passwordB = b.createString(password)

        val login = Login.createLogin(
            b,
            usernameB,
            passwordB
        )
        Message.startMessage(b)
        Message.addMsgType(b, MessageType.Login)
        Message.addMsg(b, login)
        val msg = Message.endMessage(b)

        b.finish(msg)
        val data = b.sizedByteArray()

        execute(data, onFailure, onSuccess)
    }

    fun register(
        username: String, password: String, firstName: String, lastName: String,
        onFailure: (reason: String?) -> Unit,
        onSuccess: (response: online.mctf.tomtom.api.models.Response.Message) -> Unit
    ) {
        val b = FlatBufferBuilder(128)

        val usernameB = b.createString(username)
        val passwordB = b.createString(password)
        val firstNameB = b.createString(firstName)
        val lastNameB = b.createString(lastName)

        val register = Register.createRegister(
            b,
            usernameB,
            passwordB,
            firstNameB,
            lastNameB
        )
        Message.startMessage(b)
        Message.addMsgType(b, MessageType.Register)
        Message.addMsg(b, register)
        val msg = Message.endMessage(b)

        b.finish(msg)
        val data = b.sizedByteArray()

        execute(data, onFailure, onSuccess)
    }

    fun getMe(
        token: String,
        onFailure: (reason: String?) -> Unit,
        onSuccess: (response: online.mctf.tomtom.api.models.Response.Message) -> Unit
    ) {
        val b = FlatBufferBuilder(128)

        val tokenB = b.createString(token)

        val getMe = GetMe.createGetMe(
            b,
            tokenB
        )
        Message.startMessage(b)
        Message.addMsgType(b, MessageType.GetMe)
        Message.addMsg(b, getMe)
        val msg = Message.endMessage(b)

        b.finish(msg)
        val data = b.sizedByteArray()

        execute(data, onFailure, onSuccess)
    }

    fun editMe(
        username: String?, password: String?, firstName: String?, lastName: String?, token: String,
        onFailure: (reason: String?) -> Unit,
        onSuccess: (response: online.mctf.tomtom.api.models.Response.Message) -> Unit
    ) {
        val b = FlatBufferBuilder(128)

        var usernameB: Int? = null
        if (username != null) usernameB = b.createString(username)

        var passwordB: Int? = null
        if (password != null) passwordB = b.createString(password)

        var firstNameB: Int? = null
        if (firstName != null) firstNameB = b.createString(firstName)

        var lastNameB: Int? = null
        if (lastName != null) lastNameB = b.createString(lastName)
        val tokenB = b.createString(token)

        EditMe.startEditMe(b)
        EditMe.addAdmin(b, false)
        EditMe.addToken(b, tokenB)
        if (usernameB != null) EditMe.addUsername(b, usernameB)
        if (passwordB != null) EditMe.addPassword(b, passwordB)
        if (firstNameB != null) EditMe.addFirstName(b, firstNameB)
        if (lastNameB != null) EditMe.addLastName(b, lastNameB)
        val editMe = EditMe.endEditMe(b)

        Message.startMessage(b)
        Message.addMsgType(b, MessageType.EditMe)
        Message.addMsg(b, editMe)
        val msg = Message.endMessage(b)

        b.finish(msg)
        val data = b.sizedByteArray()

        execute(data, onFailure, onSuccess)
    }

    fun requestContract(
        token: String, name: String, description: String?, price: Double, attach: ByteArray?,
        onFailure: (reason: String?) -> Unit,
        onSuccess: (response: online.mctf.tomtom.api.models.Response.Message) -> Unit
    ) {
        val b = FlatBufferBuilder(1024)

        val tokenB = b.createString(token)
        val nameB = b.createString(name)

        var descriptionB: Int? = null
        if (description != null) descriptionB = b.createString(description)

        var attachB: Int? = null
        if (attach != null) attachB = b.createByteVector(attach)

        RequestContract.startRequestContract(b)
        RequestContract.addToken(b, tokenB)
        RequestContract.addName(b, nameB)
        if (descriptionB != null) RequestContract.addDescription(b, descriptionB)
        RequestContract.addPrice(b, price)
        if (attachB != null) RequestContract.addAttachment(b, attachB)
        val requestContract = RequestContract.endRequestContract(b)

        Message.startMessage(b)
        Message.addMsgType(b, MessageType.RequestContract)
        Message.addMsg(b, requestContract)
        val msg = Message.endMessage(b)

        b.finish(msg)
        val data = b.sizedByteArray()

        execute(data, onFailure, onSuccess)
    }

    fun listContracts(
        token: String,
        onFailure: (reason: String?) -> Unit,
        onSuccess: (response: online.mctf.tomtom.api.models.Response.Message) -> Unit
    ) {
        val b = FlatBufferBuilder(128)

        val tokenB = b.createString(token)

        val listContracts = ListContracts.createListContracts(
            b,
            tokenB
        )
        Message.startMessage(b)
        Message.addMsgType(b, MessageType.ListContracts)
        Message.addMsg(b, listContracts)
        val msg = Message.endMessage(b)

        b.finish(msg)
        val data = b.sizedByteArray()

        execute(data, onFailure, onSuccess)
    }

    fun checkContract(
        token: String, id: Long,
        onFailure: (reason: String?) -> Unit,
        onSuccess: (response: online.mctf.tomtom.api.models.Response.Message) -> Unit
    ) {
        val b = FlatBufferBuilder(128)

        val tokenB = b.createString(token)

        val checkContract = CheckContract.createCheckContract(
            b,
            tokenB,
            id
        )
        Message.startMessage(b)
        Message.addMsgType(b, MessageType.CheckContract)
        Message.addMsg(b, checkContract)
        val msg = Message.endMessage(b)

        b.finish(msg)
        val data = b.sizedByteArray()

        execute(data, onFailure, onSuccess)
    }

    private fun execute(
        data: ByteArray,
        onFailure: (reason: String?) -> Unit,
        onSuccess: (response: online.mctf.tomtom.api.models.Response.Message) -> Unit
    ) {
        val request = Request.Builder()
            .url(baseUrl)
            .post(data.toRequestBody(null))
            .build()

        client?.newCall(request)?.enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                Log.e(TAG, "HTTP request failed!", e)
                context.runOnUiThread {
                    onFailure(e.message.toString())
                }
            }

            override fun onResponse(call: Call, response: Response) {
                Log.i(TAG, "HTTP request succeed!")
                val raw = response.body?.bytes()
                if (raw == null) {
                    context.runOnUiThread {
                        onFailure("Server did not send any data")
                    }
                    return
                }

                val rMsg = online.mctf.tomtom.api.models.Response.Message.getRootAsMessage(
                    ByteBuffer.wrap(raw)
                )

                if (rMsg.error()) {
                    context.runOnUiThread {
                        onFailure(rMsg.errorDescription())
                    }
                    return
                }

                context.runOnUiThread {
                    onSuccess(rMsg)
                }
            }
        })
    }

    companion object {
        val TAG = Client::class.java.simpleName
    }
}
