package online.mctf.tomtom.ui.main.fragments

import android.annotation.SuppressLint
import android.app.Activity
import android.content.Context
import android.content.Intent
import android.content.SharedPreferences
import android.database.Cursor
import android.net.Uri
import android.os.Bundle
import android.provider.OpenableColumns
import android.util.Log
import android.view.KeyEvent
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import com.google.android.material.snackbar.Snackbar
import kotlinx.android.synthetic.main.fragment_new_contract.*
import online.mctf.tomtom.BuildConfig.DEBUG
import online.mctf.tomtom.Prefs
import online.mctf.tomtom.R
import online.mctf.tomtom.api.Client
import online.mctf.tomtom.api.models.Response.Message
import online.mctf.tomtom.api.models.Response.RequestContract
import online.mctf.tomtom.api.models.Response.Status
import online.mctf.tomtom.ui.login.LoginActivity
import kotlin.concurrent.thread

class NewContractFragment : Fragment() {

    private var attachmentUri: Uri? = null
    private var prefs: SharedPreferences? = null

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_new_contract, container, false)
    }

    override fun onStart() {
        super.onStart()
        prefs = activity?.getSharedPreferences(Prefs.prefname_default, Context.MODE_PRIVATE)
        Button_newContract_selectAttach.setOnClickListener {
            val intent = Intent(Intent.ACTION_GET_CONTENT).apply {
                addCategory(Intent.CATEGORY_OPENABLE)
                type = "*/*"
            }
            startActivityForResult(intent, REQUEST_CODE_CHOOSE_FILE)
        }

        Button_newContract.setOnClickListener {
            newContract()
        }

        EditText_newContract_price.setOnKeyListener { _: View?, keyCode: Int, event: KeyEvent? ->
            if (event?.action == KeyEvent.ACTION_DOWN && keyCode == KeyEvent.KEYCODE_ENTER) {
                newContract()
                return@setOnKeyListener true
            }
            false
        }
    }

    @SuppressLint("SetTextI18n")
    private fun newContract() {
        Button_newContract?.startAnimation()
        var error = false

        val name = EditText_newContract_name?.text?.toString()
        if (name.isNullOrBlank()) {
            error = true
            TIL_newContract_name?.error = getString(R.string.LoginFragment_error_fieldIsEmpty)
        } else {
            TIL_newContract_name?.error = null
        }

        var description = EditText_newContract_description?.text?.toString()
        if (description.isNullOrBlank()) description = null

        val price = EditText_newContract_price?.text?.toString()?.toDoubleOrNull()
        if (price == null) {
            error = true
            TIL_newContract_price?.error = getString(R.string.LoginFragment_error_fieldIsEmpty)
        }

        val serverUrl = prefs?.getString(Prefs.prefkey_serverUrl, null)
        if (serverUrl == null) {
            startActivity(Intent(activity, LoginActivity::class.java))
            activity?.finish()
            return
        }

        val token = prefs?.getString(Prefs.prefkey_token, null)
        if (token == null) {
            startActivity(Intent(activity, LoginActivity::class.java))
            activity?.finish()
            return
        }

        if (error) {
            Button_newContract?.revertAnimation()
            return
        }

        if (DEBUG) Log.d(
            TAG, "Prepared request w/ name=$name, desc=$description," +
                    " price=$price, attachUri=$attachmentUri"
        )

        val client = Client(
            activity as Activity,
            serverUrl
        )

        thread {
            var attachBytes: ByteArray? = null
            if (attachmentUri != null) {
                val attachInputStream = activity?.contentResolver
                    ?.openInputStream(attachmentUri!!)
                if (attachInputStream != null) {
                    attachBytes = attachInputStream.readBytes()
                }
            }
            activity?.runOnUiThread {
                client.requestContract(
                    token, name!!, description, price!!, attachBytes,
                    onFailure = { reason: String? ->
                        Log.e(TAG, "Unsuccessful request: $reason")
                        Button_newContract?.revertAnimation()
                        TIL_newContract_name?.error = reason
                    },
                    onSuccess = { response: Message ->
                        val r = (response.msg(RequestContract()) as RequestContract).result()
                        val u = r.byUser()
                        Log.i(
                            TAG, "Successful request! Data: id=${r.id()}," +
                                    " name=${r.name()}, desc=${r.description()}," +
                                    " price=${r.price()}, attachLen=${r.attachmentLength()}," +
                                    " status=${Status.name(r.status())}, createdAt=${r.createdAt()}," +
                                    " user-username=${u.username()}, user-firstName=${u.firstName()}," +
                                    " user-lastName=${u.lastName()}"
                        )

                        Button_newContract?.revertAnimation()
                        EditText_newContract_name?.text = null
                        EditText_newContract_description?.text = null
                        EditText_newContract_price?.setText("1337")
                        EditText_newContract_attachPath?.text = null
                        attachmentUri = null

                        try {
                            Snackbar.make(
                                Button_newContract,
                                R.string.NewContractFragment_result_success,
                                Snackbar.LENGTH_SHORT
                            ).show()
                        } catch (ignored: Exception) {
                        }
                    }
                )
            }
        }


    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode == REQUEST_CODE_CHOOSE_FILE && resultCode == Activity.RESULT_OK) {
            data?.data?.also { uri: Uri ->
                Log.d(TAG, "Got attach uri: $uri")
                EditText_newContract_attachPath.setText(getFilename(uri))
                attachmentUri = uri
            }
        } else {
            Log.w(TAG, "Not our request code ($requestCode) or unsuccessful result ($resultCode)")
        }
    }

    private fun getFilename(uri: Uri): String? {
        // The query, since it only applies to a single document, will only return
        // one row. There's no need to filter, sort, or select fields, since we want
        // all fields for one document.
        val cursor: Cursor? = activity?.contentResolver?.query(uri, null, null, null, null, null)

        cursor?.use {
            // moveToFirst() returns false if the cursor has 0 rows.  Very handy for
            // "if there's anything to look at, look at it" conditionals.
            if (it.moveToFirst()) {

                // Note it's called "Display Name".  This is
                // provider-specific, and might not necessarily be the file name.
                return it.getString(it.getColumnIndex(OpenableColumns.DISPLAY_NAME))
            }
        }
        return null
    }

    companion object {
        val TAG = NewContractFragment::class.java.simpleName
        const val REQUEST_CODE_CHOOSE_FILE = 8
    }
}
