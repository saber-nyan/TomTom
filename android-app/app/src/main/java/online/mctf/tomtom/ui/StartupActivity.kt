package online.mctf.tomtom.ui

import android.content.Context
import android.content.Intent
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import online.mctf.tomtom.Prefs
import online.mctf.tomtom.ui.login.LoginActivity
import online.mctf.tomtom.ui.main.MainActivity

class StartupActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val prefs = getSharedPreferences(Prefs.prefname_default, Context.MODE_PRIVATE)

        if (prefs.getString(Prefs.prefkey_token, null).isNullOrBlank()) {
            startActivity(Intent(this, LoginActivity::class.java))
        } else {
            startActivity(Intent(this, MainActivity::class.java))
        }
        finish()
    }
}
