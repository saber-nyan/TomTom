package online.mctf.tomtom.ui.main.fragments


import android.app.Activity
import android.content.Context
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.text.InputType
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.fragment.app.Fragment
import androidx.navigation.fragment.findNavController
import kotlinx.android.synthetic.main.fragment_contract_details.*
import online.mctf.tomtom.Prefs
import online.mctf.tomtom.R
import online.mctf.tomtom.api.Client
import online.mctf.tomtom.api.models.Response.CheckContract
import online.mctf.tomtom.api.models.Response.Message
import online.mctf.tomtom.ui.login.LoginActivity
import online.mctf.tomtom.ui.main.ContractAdapter
import java.io.FileOutputStream
import java.text.SimpleDateFormat
import java.util.*
import kotlin.concurrent.thread

class ContractDetailsFragment : Fragment(), View.OnClickListener {
    override fun onClick(v: View?) {
        if (v?.id == R.id.Button_contractDetails_attachment) {
            val intent = Intent(Intent.ACTION_CREATE_DOCUMENT).apply {
                addCategory(Intent.CATEGORY_OPENABLE)
                type = "image/*"
                putExtra(Intent.EXTRA_TITLE, "attachment")
            }
            startActivityForResult(intent, REQUEST_CODE_WRITE_FILE)
        } else {
            Log.w(TAG, "${v?.id} is not implemented")
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode == REQUEST_CODE_WRITE_FILE && resultCode == Activity.RESULT_OK) {
            data?.data?.also { uri: Uri ->
                Log.v(TAG, "New URI: $uri")
                thread {
                    try {
                        activity?.contentResolver?.openFileDescriptor(uri, "w")?.use { f ->
                            FileOutputStream(f.fileDescriptor).use {
                                if (attach == null) return@thread
//                                attach!!.get(arr)
                                it.write(attach!!)
//                                attach.rewind()
                            }
                        }
                    } catch (e: Exception) {
                        activity?.runOnUiThread {
                            Toast.makeText(activity, e.localizedMessage, Toast.LENGTH_LONG).show()
                        }
                    }
                }
            }
        } else {
            Log.w(TAG, "Not our request code ($requestCode) or unsuccessful result ($resultCode)")
        }
    }

    private var attach: ByteArray? = null

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_contract_details, container, false)
    }

    override fun onStart() {
        super.onStart()
        EditText_contractDetails_name.inputType = InputType.TYPE_NULL
//        EditText_contractDetails_description.inputType =
//            InputType.TYPE_NULL or InputType.TYPE_TEXT_FLAG_MULTI_LINE or InputType.TYPE_TEXT_VARIATION_LONG_MESSAGE
        EditText_contractDetails_price.inputType = InputType.TYPE_NULL
        EditText_contractDetails_status.inputType = InputType.TYPE_NULL
        EditText_contractDetails_createdAt.inputType = InputType.TYPE_NULL
        EditText_contractDetails_user_username.inputType = InputType.TYPE_NULL
        EditText_contractDetails_user_firstName.inputType = InputType.TYPE_NULL
        EditText_contractDetails_user_lastName.inputType = InputType.TYPE_NULL


        val id = arguments?.getLong(KEY_CONTRACT_ID, -1)
        Log.d(TAG, "Got $id as arg")

        if (id == null || id == -1L) {
            findNavController().popBackStack()
            return
        }

        val prefs = activity?.getSharedPreferences(Prefs.prefname_default, Context.MODE_PRIVATE)
        val token = prefs?.getString(Prefs.prefkey_token, null)
        val serverUrl = prefs?.getString(Prefs.prefkey_serverUrl, null)
        if (token == null || serverUrl == null) {
            startActivity(Intent(activity, LoginActivity::class.java))
            activity?.finish()
            return
        }

        val client = Client(activity as Activity, serverUrl)
        client.checkContract(
            token,
            id,
            onFailure = { reason: String? ->
                Toast.makeText(
                    activity,
                    getString(R.string.ContractDetailsFragment_result_error).format(reason),
                    Toast.LENGTH_SHORT
                ).show()
                findNavController().popBackStack()
            },
            onSuccess = { response: Message ->
                val r = response.msg(CheckContract()) as CheckContract
                val u = r.byUser()

                EditText_contractDetails_name?.setText(r.name())

                if (r.description() != null) {
                    TIL_contractDetails_description?.visibility = View.VISIBLE
                    EditText_contractDetails_description?.setText(r.description())
                } else {
                    TIL_contractDetails_description?.visibility = View.GONE
                }

                EditText_contractDetails_price?.setText(r.price().toString())

                Button_contractDetails_attachment?.visibility =
                    if (r.attachmentLength() > 0) View.VISIBLE else View.GONE
                if (r.attachmentLength() > 0) {
                    val len = r.attachmentLength()
                    attach = ByteArray(len)
                    var i = 0
                    while (i < len) {
                        attach!![i] = r.attachment(i).toByte()
                        ++i
                    }

                    Button_contractDetails_attachment?.setOnClickListener(this@ContractDetailsFragment)
                }

                EditText_contractDetails_status?.setText(
                    ContractAdapter.statusToLocalizedName(
                        r.status(),
                        activity!!
                    )
                )

                val parsed = Date(r.createdAt() * 1000L) // Java is expecting milliseconds
                val format = SimpleDateFormat("yyyy.MM.dd HH:mm", Locale.GERMAN)
                EditText_contractDetails_createdAt?.setText(format.format(parsed))

                EditText_contractDetails_user_username?.setText(u.username())
                EditText_contractDetails_user_firstName?.setText(u.firstName())
                EditText_contractDetails_user_lastName?.setText(u.lastName())
            })
    }

    companion object {
        val TAG = ContractDetailsFragment::class.java.simpleName
        const val KEY_CONTRACT_ID = "KEY_CONTRACT_ID"
        const val REQUEST_CODE_WRITE_FILE = 16
    }
}
