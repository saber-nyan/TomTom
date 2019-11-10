package online.mctf.tomtom.ui.main.fragments

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.content.SharedPreferences
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.core.content.edit
import androidx.fragment.app.Fragment
import com.google.android.material.snackbar.Snackbar
import kotlinx.android.synthetic.main.fragment_profile.*
import online.mctf.tomtom.Prefs
import online.mctf.tomtom.R
import online.mctf.tomtom.api.Client
import online.mctf.tomtom.api.models.Response.EditMe
import online.mctf.tomtom.api.models.Response.GetMe
import online.mctf.tomtom.api.models.Response.Message
import online.mctf.tomtom.ui.login.LoginActivity

class ProfileFragment : Fragment() {

    private lateinit var prefs: SharedPreferences

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_profile, container, false)
    }

    override fun onStart() {
        super.onStart()
        prefs = activity?.getSharedPreferences(Prefs.prefname_default, Context.MODE_PRIVATE)!!

        EditText_editMe_serverUrl.setText(
            prefs.getString(
                Prefs.prefkey_serverUrl,
                Prefs.prefdef_serverUrl
            )
        )

        Button_editMe_logout.setOnClickListener {
            prefs.edit(commit = true) {
                remove(Prefs.prefkey_token)
            }
            startActivity(Intent(activity, LoginActivity::class.java))
            activity?.finish()

        }

        updateData()

        Button_editMe.setOnClickListener {
            editMe()
        }
    }

    private fun updateData() {
        val token = prefs.getString(Prefs.prefkey_token, null)
        val serverUrl = prefs.getString(Prefs.prefkey_serverUrl, Prefs.prefdef_serverUrl)
        if (token == null || serverUrl == null) {
            startActivity(Intent(activity, LoginActivity::class.java))
            activity?.finish()
            return
        }

        val client = Client(activity as Activity, serverUrl)
        client.getMe(
            token,
            onFailure = { reason: String? ->
                Log.w(TAG, "Request failed: $reason")
                try {
                    Snackbar.make(
                        Button_editMe,
                        getString(R.string.ProfileFragment_request_fail).format(reason),
                        Snackbar.LENGTH_LONG
                    ).show()
                } catch (ignored: Exception) {
                }
            },
            onSuccess = { response: Message ->
                val r = response.msg(GetMe()) as GetMe
                EditText_editMe_username?.setText(r.username())
                EditText_editMe_firstName?.setText(r.firstName())
                EditText_editMe_lastName?.setText(r.lastName())
            })
    }

    private fun editMe() {
        Button_editMe?.startAnimation()
        val token = prefs.getString(Prefs.prefkey_token, null)
        val serverUrl = EditText_editMe_serverUrl?.text?.toString()
//        val serverUrl = prefs.getString(Prefs.prefkey_serverUrl, Prefs.prefdef_serverUrl)
        if (token == null || serverUrl == null) {
            Button_editMe?.revertAnimation()
            startActivity(Intent(activity, LoginActivity::class.java))
            activity?.finish()
            return
        }

        prefs.edit {
            putString(Prefs.prefkey_serverUrl, serverUrl)
        }

        val password =
            if (!EditText_editMe_password.text?.toString().isNullOrEmpty()) EditText_editMe_password?.text?.toString() else null

        val client = Client(activity as Activity, serverUrl)
        client.editMe(
            EditText_editMe_username?.text?.toString(),
            password,
            EditText_editMe_firstName?.text?.toString(),
            EditText_editMe_lastName?.text?.toString(),
            token,
            onFailure = { reason: String? ->
                Log.w(TAG, "Request failed: $reason")
                try {
                    Snackbar.make(
                        Button_editMe,
                        getString(R.string.ProfileFragment_edit_request_fail).format(reason),
                        Snackbar.LENGTH_LONG
                    ).show()
                } catch (ignored: Exception) {
                }
                Button_editMe?.revertAnimation()
            },
            onSuccess = { response: Message ->
                val r = response.msg(EditMe()) as EditMe
                prefs.edit {
                    putString(Prefs.prefkey_token, r.token())
                }
                Button_editMe?.revertAnimation()
            }
        )
    }

    companion object {
        val TAG = ProfileFragment::class.java.simpleName
    }
}
