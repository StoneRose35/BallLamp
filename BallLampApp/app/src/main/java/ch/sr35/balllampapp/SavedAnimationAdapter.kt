package ch.sr35.balllampapp


import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import ch.sr35.balllampapp.backend.SavedAnimationDao

class SavedAnimationAdapter(private val dataSet: List<SavedAnimationDao>): RecyclerView.Adapter<SavedAnimationAdapter.ViewHolderSA>() {

    class ViewHolderSA(view: View) : RecyclerView.ViewHolder(view) {
        var textViewAnimationName: TextView=view.findViewById(R.id.textViewAnimationName)
        var savedAnimation: SavedAnimationDao?= null
        init{
            textViewAnimationName.text= "Muh"
        }


    }
    override fun onCreateViewHolder(
        parent: ViewGroup,
        viewType: Int
    ): SavedAnimationAdapter.ViewHolderSA {
        val view = LayoutInflater.from(parent.context)
            .inflate(R.layout.saved_animation_layout, parent, false)
        return ViewHolderSA(view)
    }

    override fun getItemCount(): Int {
        return dataSet.size
    }

    override fun onBindViewHolder(holder: SavedAnimationAdapter.ViewHolderSA, position: Int) {
        holder.textViewAnimationName.text = dataSet[position].animation?.name ?: ""
        holder.savedAnimation = dataSet[position]
        holder.itemView.setOnClickListener {
            holder.savedAnimation!!.animation?.toCommandList()
            (it.context as MainActivity).alFragment.animation = holder.savedAnimation!!.animation!!
        }
    }

}