package online.mctf.tomtom.ui.login.fragments

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.content.SharedPreferences
import android.os.Bundle
import android.util.Log
import android.view.KeyEvent
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.core.content.edit
import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentManager
import androidx.fragment.app.FragmentTransaction
import kotlinx.android.synthetic.main.fragment_register.*
import online.mctf.tomtom.BuildConfig.DEBUG
import online.mctf.tomtom.Prefs
import online.mctf.tomtom.R
import online.mctf.tomtom.api.Client
import online.mctf.tomtom.api.models.Response.Message
import online.mctf.tomtom.api.models.Response.Register
import online.mctf.tomtom.ui.main.MainActivity

class RegisterFragment : Fragment() {

    private var prefs: SharedPreferences? = null

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_register, container, false)
    }

    override fun onStart() {
        super.onStart()
        TextView_switchToLogin.setOnClickListener {
            fragmentManager?.popBackStack(null, FragmentManager.POP_BACK_STACK_INCLUSIVE)
            fragmentManager?.beginTransaction()
                ?.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE)
                ?.replace(R.id.login_fragment, LoginFragment())
                ?.commit()
        }

        prefs = activity?.getSharedPreferences(Prefs.prefname_default, Context.MODE_PRIVATE)

        Button_doRegister.setOnClickListener {
            register()
        }
        EditText_register_serverUrl.setText(
            prefs?.getString(Prefs.prefkey_serverUrl, null)
                ?: Prefs.prefdef_serverUrl
        )

        EditText_register_serverUrl.setOnKeyListener { _: View?, keyCode: Int, event: KeyEvent? ->
            if (event?.action == KeyEvent.ACTION_DOWN && keyCode == KeyEvent.KEYCODE_ENTER) {
                register()
                return@setOnKeyListener true
            }
            false
        }
    }

    private fun register() {
        Button_doRegister?.startAnimation()
        var error = false

        val username = EditText_register_username?.text?.toString()
        if (username.isNullOrEmpty()) {
            error = true
            TIL_register_username?.error = getString(R.string.LoginFragment_error_fieldIsEmpty)
        } else {
            TIL_register_username?.error = null
        }

        val password = EditText_register_password?.text?.toString()
        if (password.isNullOrEmpty()) {
            error = true
            TIL_register_password?.error = getString(R.string.LoginFragment_error_fieldIsEmpty)
        } else {
            TIL_register_password?.error = null
        }

        val firstName = EditText_register_firstName?.text?.toString()
        if (firstName.isNullOrEmpty()) {
            error = true
            TIL_register_firstName?.error = getString(R.string.LoginFragment_error_fieldIsEmpty)
        } else {
            TIL_register_firstName?.error = null
        }

        val lastName = EditText_register_lastName?.text?.toString()
        if (lastName.isNullOrEmpty()) {
            error = true
            TIL_register_lastName?.error = getString(R.string.LoginFragment_error_fieldIsEmpty)
        } else {
            TIL_register_lastName?.error = null
        }

        val serverUrl = EditText_register_serverUrl?.text?.toString()
        if (serverUrl.isNullOrEmpty()) {
            error = true
            TIL_register_serverUrl?.error = getString(R.string.LoginFragment_error_fieldIsEmpty)
        } else {
            TIL_register_serverUrl?.error = null
        }

        if (error) {
            Button_doRegister?.revertAnimation()
            return
        }

        if (DEBUG) Log.d(
            TAG, "Prepared request w/ username=$username, " +
                    "password=$password, firstName=$firstName, serverUrl=$serverUrl"
        )

        prefs?.edit {
            putString(Prefs.prefkey_serverUrl, serverUrl)
        }

        val client = Client(
            activity as Activity,
            serverUrl!!
        )

        client.register(
            username!!,
            password!!,
            firstName!!,
            lastName!!,
            onFailure = { reason: String? ->
                Log.e(TAG, "Unsuccessful request: $reason")
                Button_doRegister?.revertAnimation()
                TIL_register_serverUrl?.error = reason
            },
            onSuccess = { response: Message ->
                val r = response.msg(Register()) as Register
                if (DEBUG) Log.i(TAG, "Successful request! Token: ${r.token()}")
                Button_doRegister?.revertAnimation()

                prefs?.edit(commit = true) {
                    putString(Prefs.prefkey_token, r.token())
                }

                val i = Intent(activity, MainActivity::class.java)
                i.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP or Intent.FLAG_ACTIVITY_SINGLE_TOP)
                startActivity(i)
                activity?.finish()
            }
        )
    }

    companion object {
        val TAG = RegisterFragment::class.java.simpleName
    }
}
