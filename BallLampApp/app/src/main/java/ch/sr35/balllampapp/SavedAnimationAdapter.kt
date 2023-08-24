package ch.sr35.balllampapp


import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import ch.sr35.balllampapp.backend.CommandDispatcher
import ch.sr35.balllampapp.backend.SavedAnimationDao

class SavedAnimationAdapter(private val dataSet: List<SavedAnimationDao>): RecyclerView.Adapter<SavedAnimationAdapter.ViewHolderSA>() {

    class ViewHolderSA(view: View) : RecyclerView.ViewHolder(view) {
        var textViewAnimationName: TextView=view.findViewById(R.id.textViewAnimationName)
        var savedAnimation: SavedAnimationDao?= null

    }
    override fun onCreateViewHolder(
        parent: ViewGroup,
        viewType: Int
    ): ViewHolderSA {
        val view = LayoutInflater.from(parent.context)
            .inflate(R.layout.saved_animation_layout, parent, false)
        return ViewHolderSA(view)
    }

    override fun getItemCount(): Int {
        return dataSet.size
    }

    override fun onBindViewHolder(holder: ViewHolderSA, position: Int) {
        val cmds = ArrayList<String>()
        holder.textViewAnimationName.text = dataSet[position].animation?.name ?: ""
        holder.savedAnimation = dataSet[position]
        holder.itemView.setOnClickListener {
            val commands = holder.savedAnimation!!.animation!!.toCommandList()
            cmds.add("STOP\r")
            cmds.addAll(commands)
            cmds.add("START\r")
            val cmdDispatcher = CommandDispatcher(cmds, (it.context as MainActivity))
            cmdDispatcher.start()
            (it.context as MainActivity).alFragment.animation = holder.savedAnimation!!.animation!!
            (it.context as MainActivity).alFragment.fileName = holder.savedAnimation!!.fileName
        }
    }
}