package ch.sr35.balllampapp

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView

class AnimationListAdapter(private val dataSet: Animation): RecyclerView.Adapter<AnimationListAdapter.ViewHolder>(){

        /**
         * Provide a reference to the type of views that you are using
         * (custom ViewHolder).
         */
        class ViewHolder(view: View) : RecyclerView.ViewHolder(view) {
            val lampSelectorLower: LampSelectorView = view.findViewById(R.id.lampSelectorViewLower)
            val lampSelectorUpper:LampSelectorView = view.findViewById(R.id.lampSelectorViewUpper)
            val timefield: TextView = view.findViewById(R.id.timeView)
            var duration: Double = 0.0

            init {
                // Define click listener for the ViewHolder's View.
                lampSelectorLower.editable = false
                lampSelectorUpper.editable = false
            }
        }

        // Create new views (invoked by the layout manager)
        override fun onCreateViewHolder(viewGroup: ViewGroup, viewType: Int): ViewHolder {
            // Create a new view, which defines the UI of the list item
            val view = LayoutInflater.from(viewGroup.context)
                .inflate(R.layout.animation_step_layout, viewGroup, false)

            return ViewHolder(view)
        }

        // Replace the contents of a view (invoked by the layout manager)
        override fun onBindViewHolder(viewHolder: ViewHolder, position: Int) {

            // Get element from your dataset at this position and replace the
            // contents of the view with that element
            for (el in dataSet.lampAnimations) {
                if (el.lampNr < 10) {
                    viewHolder.lampSelectorUpper.triangleColors[el.lampNr.toInt()] =
                        el.steps[position].color
                }
                else
                {
                    viewHolder.lampSelectorLower.triangleColors[el.lampNr.toInt()-10] =
                        el.steps[position].color
                }
            }
            viewHolder.duration = dataSet.lampAnimations[0].steps[position].duration/30.0
            viewHolder.timefield.text = viewHolder.duration.toString()

        }

        // Return the size of your dataset (invoked by the layout manager)
        override fun getItemCount() = dataSet.lampAnimations[0].steps.size

    }

