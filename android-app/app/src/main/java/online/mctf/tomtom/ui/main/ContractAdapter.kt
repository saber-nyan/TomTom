package online.mctf.tomtom.ui.main

import android.content.Context
import android.os.Bundle
import android.text.SpannableStringBuilder
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.cardview.widget.CardView
import androidx.core.text.bold
import androidx.navigation.NavController
import androidx.recyclerview.widget.RecyclerView
import online.mctf.tomtom.R
import online.mctf.tomtom.api.models.Response.CheckContract
import online.mctf.tomtom.ui.main.fragments.ContractDetailsFragment.Companion.KEY_CONTRACT_ID
import java.text.SimpleDateFormat
import java.util.*

class ContractAdapter(
    private val dataset: ArrayList<CheckContract>,
    private val findNavController: NavController
) : RecyclerView.Adapter<ContractAdapter.ContractViewHolder>() {

    class ContractViewHolder(v: View) : RecyclerView.ViewHolder(v) {

        val name: TextView = v.findViewById(R.id.TextView_contract_name)
        val price: TextView = v.findViewById(R.id.TextView_contract_price)
        val description: TextView = v.findViewById(R.id.TextView_contract_description)
        val status: TextView = v.findViewById(R.id.TextView_contract_status)
        val user: TextView = v.findViewById(R.id.TextView_contract_byUser)
        val dateTime: TextView = v.findViewById(R.id.TextView_contract_dateTime)
        val root: CardView = v.findViewById(R.id.CardView_contract)
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ContractViewHolder {
        val itemView = LayoutInflater
            .from(parent.context)
            .inflate(R.layout.recyclerview_contract, parent, false)
        return ContractViewHolder(itemView)
    }

    override fun getItemCount(): Int = dataset.size

    override fun onBindViewHolder(holder: ContractViewHolder, position: Int) {
        val r = dataset[position]
        val u = r.byUser()
        holder.apply {
            name.text = r.name()
            price.text = itemView.context.getString(R.string.ContractAdapter_price_template)
                .format(r.price())
            description.text = r.description()?.replace('\n', ' ')
            val span = SpannableStringBuilder()
                .bold { append(itemView.context.getString(R.string.ContractAdapter_status_template)) }
                .append(statusToLocalizedName(r.status(), itemView.context))
            status.text = span
            user.text = "%s %s (@%s)".format(u.lastName(), u.firstName(), u.username())
            val parsed = Date(r.createdAt() * 1000L) // Java is expecting milliseconds
            val format = SimpleDateFormat("yyyy.MM.dd HH:mm", Locale.GERMAN)
            dateTime.text = format.format(parsed)

            root.setOnClickListener {
                Log.i(TAG, "Clicked on #${r.id()}")
                findNavController.navigate(R.id.action_contractsListFragment_to_contractDetailsFragment,
                    Bundle().apply {
                        putLong(KEY_CONTRACT_ID, r.id())
                    }
                )
            }
        }
    }


    fun clear() {
        dataset.clear()
//        notifyDataSetChanged()
    }

    fun addContract(source: CheckContract) {
        dataset.add(source)
//        notifyItemInserted(dataset.size)
    }

    companion object {
        val TAG = ContractAdapter::class.java.simpleName

        @JvmStatic
        fun statusToLocalizedName(s: Int, context: Context): String = when (s) {
            1 -> context.getString(R.string.ContractAdapter_status_processing)
            2 -> context.getString(R.string.ContractAdapter_status_accepted)
            3 -> context.getString(R.string.ContractAdapter_status_rejected)
            else -> context.getString(R.string.ContractAdapter_status_unknown)
        }
    }
}
