package ch.sr35.balllampapp


import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.appcompat.widget.SwitchCompat
import androidx.recyclerview.widget.RecyclerView
import ch.sr35.balllampapp.backend.CommandDispatcher
import ch.sr35.balllampapp.backend.SavedAnimationDao
import com.google.android.material.snackbar.BaseTransientBottomBar.LENGTH_SHORT
import com.google.android.material.snackbar.Snackbar
import org.w3c.dom.Text

class SavedAnimationAdapter(private val dataSet: List<SavedAnimationDao>): RecyclerView.Adapter<SavedAnimationAdapter.ViewHolderSA>() {

    class ViewHolderSA(view: View) : RecyclerView.ViewHolder(view) {
        var textViewAnimationName: TextView=view.findViewById(R.id.textViewAnimationName)
        var textViewAnimationLength: TextView=view.findViewById(R.id.savedAnimationDuration)
        var textViewAnimationNFrames: TextView=view.findViewById(R.id.savedAnimationNFrames)
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
        val nFrames: Int
        holder.textViewAnimationName.text = dataSet[position].animation?.name ?: ""
        val totDuration: Double = dataSet[position].animation?.getTotalDurationInSeconds() ?: 0.0
        holder.textViewAnimationLength.text = String.format("Duration: %.2f",totDuration)
        nFrames = if (dataSet[position].animation != null) {
            dataSet[position].animation!!.lampAnimations[0].steps.size
        } else {
            0
        }
        holder.textViewAnimationNFrames.text = String.format("Frames: %d",nFrames)
        holder.savedAnimation = dataSet[position]
        holder.itemView.setOnClickListener {
            if ((it.context as MainActivity).btSocket != null && (it.context as MainActivity).btSocket?.isConnected == true) {
                val commands = holder.savedAnimation!!.animation!!.toCommandList()
                cmds.add("STOP\r")
                cmds.addAll(commands)
                cmds.add("START\r")
                val cmdDispatcher = CommandDispatcher(cmds, (it.context as MainActivity))
                cmdDispatcher.start()
                (it.context as MainActivity).alFragment.animation =
                    holder.savedAnimation!!.animation!!
                (it.context as MainActivity).alFragment.fileName = holder.savedAnimation!!.fileName
                (it.context as MainActivity).alFragment.view?.findViewById<SwitchCompat>(R.id.switchAnimationOn)?.isChecked =
                    true
                val sbLaunchAnimation  = Snackbar.make(it.rootView,"Animation launched",LENGTH_SHORT)
                sbLaunchAnimation.show()
            }
        }
    }
}