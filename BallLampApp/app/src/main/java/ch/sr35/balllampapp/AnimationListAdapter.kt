package ch.sr35.balllampapp

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.EditText
import androidx.recyclerview.widget.RecyclerView

class AnimationListAdapter(private val dataSet: Animation): RecyclerView.Adapter<AnimationListAdapter.ViewHolder>(){

        /**
         * Provide a reference to the type of views that you are using
         * (custom ViewHolder).
         */
        class ViewHolder(view: View) : RecyclerView.ViewHolder(view) {
            val lampSelectorLower: LampSelectorView
            val lampSelectorUpper:LampSelectorView
            val timefield: EditText

            init {
                // Define click listener for the ViewHolder's View.
                lampSelectorLower = view.findViewById(R.id.lampSelectorViewLower)
                lampSelectorUpper = view.findViewById(R.id.lampSelectorViewUpper)
                timefield = view.findViewById(R.id.editTextTime2)
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

        }

        // Return the size of your dataset (invoked by the layout manager)
        override fun getItemCount() = dataSet.lampAnimations[0].steps.size

    }

