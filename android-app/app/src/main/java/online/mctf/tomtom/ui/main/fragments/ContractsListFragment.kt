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
import androidx.fragment.app.Fragment
import androidx.navigation.fragment.findNavController
import androidx.recyclerview.widget.DefaultItemAnimator
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.google.android.material.snackbar.Snackbar
import kotlinx.android.synthetic.main.fragment_contracts_list.*
import online.mctf.tomtom.Prefs
import online.mctf.tomtom.R
import online.mctf.tomtom.api.Client
import online.mctf.tomtom.api.models.Response.ListContracts
import online.mctf.tomtom.api.models.Response.Message
import online.mctf.tomtom.ui.login.LoginActivity
import online.mctf.tomtom.ui.main.ContractAdapter


class ContractsListFragment : Fragment() {

    private lateinit var recyclerView: RecyclerView
    private lateinit var viewAdapter: RecyclerView.Adapter<ContractAdapter.ContractViewHolder>
    private lateinit var viewManager: RecyclerView.LayoutManager
    private var prefs: SharedPreferences? = null

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_contracts_list, container, false)
    }

    override fun onStart() {
        super.onStart()
        prefs = activity?.getSharedPreferences(Prefs.prefname_default, Context.MODE_PRIVATE)

        SwipeRefresh_contracts_refresher.setOnRefreshListener {
            loadContracts()
        }

        viewManager = LinearLayoutManager(activity)
        viewAdapter = ContractAdapter(ArrayList(), findNavController())
        recyclerView = RecyclerView_contracts.apply {
            //            setHasFixedSize(true)
            layoutManager = viewManager
            adapter = viewAdapter
            itemAnimator = DefaultItemAnimator()
                .apply {
                    addDuration = 500
                    removeDuration = 500
                    changeDuration = 500
                }
        }
        loadContracts()
        SwipeRefresh_contracts_refresher?.isRefreshing = true
    }

    private fun loadContracts() {
        val token = prefs?.getString(Prefs.prefkey_token, null)
        val serverUrl = prefs?.getString(Prefs.prefkey_serverUrl, null)
        if (token == null || serverUrl == null) {
            startActivity(Intent(activity, LoginActivity::class.java))
            activity?.finish()
            return
        }

        val client = Client(activity as Activity, serverUrl)
        client.listContracts(
            token,
            onFailure = { reason: String? ->
                try {
                    Log.e(TAG, "Unsuccessful request: $reason")
                    Snackbar.make(
                        SwipeRefresh_contracts_refresher,
                        getString(R.string.ContractsListFragment_result_error).format(reason),
                        Snackbar.LENGTH_LONG
                    ).show()
                } catch (e: Exception) {
                    Log.w(tag, "Exception (probably UI-based)", e)
                }
                SwipeRefresh_contracts_refresher?.isRefreshing = false // FIXME: call unneeded?
            },
            onSuccess = { response: Message ->
                val r = response.msg(ListContracts()) as ListContracts
                var i = 0

                (viewAdapter as ContractAdapter).apply {
                    clear()
                    while (i < r.contractsLength()) {
                        addContract(r.contracts(i))
                        ++i
                    }
                }
                recyclerView.adapter?.notifyDataSetChanged()
                SwipeRefresh_contracts_refresher?.isRefreshing = false
            })
    }

    companion object {
        val TAG = ContractsListFragment::class.java.simpleName
    }
}
