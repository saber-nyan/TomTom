package online.mctf.tomtom.ui.login.fragments

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.view.KeyEvent
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.core.content.edit
import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentTransaction
import kotlinx.android.synthetic.main.fragment_login.*
import online.mctf.tomtom.BuildConfig.DEBUG
import online.mctf.tomtom.Prefs
import online.mctf.tomtom.R
import online.mctf.tomtom.api.Client
import online.mctf.tomtom.api.models.Response.Login
import online.mctf.tomtom.api.models.Response.Message
import online.mctf.tomtom.ui.main.MainActivity

class LoginFragment : Fragment() {

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_login, container, false)
    }

    override fun onStart() {
        super.onStart()
        TextView_switchToRegistration.setOnClickListener {
            fragmentManager?.beginTransaction()
                ?.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE)
                ?.replace(R.id.login_fragment, RegisterFragment())
                ?.addToBackStack(null)
                ?.commit()
        }

        Button_doLogin.setOnClickListener {
            login()
        }
        EditText_login_password.setOnKeyListener { _: View?, keyCode: Int, event: KeyEvent? ->
            if (event?.action == KeyEvent.ACTION_DOWN && keyCode == KeyEvent.KEYCODE_ENTER) {
                login()
                return@setOnKeyListener true
            }
            false
        }
    }

    private fun login() {
        Button_doLogin?.startAnimation()
        var error = false

        val username = EditText_login_username?.text?.toString()
        if (username.isNullOrEmpty()) {
            error = true
            TIL_login_username?.error = getString(R.string.LoginFragment_error_fieldIsEmpty)
        } else {
            TIL_login_username?.error = null
        }

        val password = EditText_login_password?.text?.toString()
        if (password.isNullOrEmpty()) {
            error = true
            TIL_login_password?.error = getString(R.string.LoginFragment_error_fieldIsEmpty)
        } else {
            TIL_login_password?.error = null
        }

        if (error) {
            Button_doLogin?.revertAnimation()
            return
        }

        if (DEBUG) Log.d(TAG, "Prepared request w/ username=$username, password=$password")

        val prefs = activity?.getSharedPreferences(Prefs.prefname_default, Context.MODE_PRIVATE)
        if (prefs == null) {
            Log.e(TAG, "Activity already destroyed!")
            return
        }
        val serverUrl = prefs.getString(Prefs.prefkey_serverUrl, Prefs.prefdef_serverUrl)
            ?: Prefs.prefdef_serverUrl
        val client = Client(
            activity as Activity,
            serverUrl
        )

        client.login(
            username!!, // Already checked for null-ness, wtf Kotlin?!
            password!!,
            onFailure = { reason: String? ->
                Log.e(TAG, "Unsuccessful request: $reason")
                Button_doLogin?.revertAnimation()
                TIL_login_password?.error = reason
            },
            onSuccess = { response: Message ->
                val r = response.msg(Login()) as Login
                if (DEBUG) Log.i(TAG, "Successful request! Token: ${r.token()}")
                Button_doLogin?.revertAnimation()

                prefs.edit(commit = true) {
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
        val TAG = LoginFragment::class.java.simpleName
    }
}
