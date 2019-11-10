package online.mctf.tomtom.ui.login

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import online.mctf.tomtom.R
import online.mctf.tomtom.ui.login.fragments.LoginFragment

class LoginActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_login)

        supportFragmentManager.beginTransaction()
            .replace(R.id.login_fragment, LoginFragment())
            .commit()
    }
}
