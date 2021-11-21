package ch.sr35.balllampapp.fragments

import android.os.Bundle
import android.text.method.ScrollingMovementMethod
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.EditText
import android.widget.SeekBar
import android.widget.TextView
import androidx.appcompat.app.AlertDialog
import androidx.core.graphics.blue
import androidx.core.graphics.green
import androidx.core.graphics.red
import androidx.fragment.app.Fragment
import ch.sr35.balllampapp.*
import ch.sr35.balllampapp.backend.LampSelectorData
import ch.sr35.balllampapp.backend.SimpleIntColor


class ColorSelect : Fragment(R.layout.fragment_color_select) {


    private var lbl: TextView? = null
    var connectionState: TextView? = null
    var mainClr: SimpleIntColor = SimpleIntColor(0,0,0)
    var lampBallSelectorUpper: LampSelectorView? = null
    var lampBallSelectorLower: LampSelectorView? = null

    var serialLogger: TextView? = null
    private var btnConnect: Button? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val ldLower = savedInstanceState?.getParcelable<LampSelectorData>("lampLowerData")
        val ldUpper = savedInstanceState?.getParcelable<LampSelectorData>("lampUpperData")
        if (ldLower != null)
        {
            lampBallSelectorLower?.lampData=ldLower
        }
        if (ldUpper != null)
        {
            lampBallSelectorUpper?.lampData=ldUpper
        }
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        view.findViewById<SeekBar>(R.id.amountRed).setOnSeekBarChangeListener(
            RGBSeekBarChangeListener(this, COLOR_RED)
        )
        view.findViewById<SeekBar>(R.id.amountGreen).setOnSeekBarChangeListener(
            RGBSeekBarChangeListener(this, COLOR_GREEN)
        )
        view.findViewById<SeekBar>(R.id.amountBlue).setOnSeekBarChangeListener(
            RGBSeekBarChangeListener(this, COLOR_BLUE)
        )

        lbl = view.findViewById(R.id.textViewLblLower)
        connectionState = view.findViewById(R.id.textViewConnectionState)
        serialLogger = view.findViewById(R.id.serialOut)
        lampBallSelectorUpper = view.findViewById(R.id.lampSelectorUpper)
        lampBallSelectorLower =view. findViewById(R.id.lampSelectorLower)
        btnConnect = view.findViewById(R.id.btnConnect)
        val durField = view.findViewById<EditText>(R.id.editTextDuration)

        lampBallSelectorUpper?.mappingTable = resources.getIntArray(R.array.lampMappingUpper)
        lampBallSelectorLower?.mappingTable = resources.getIntArray(R.array.lampMappingLower)

        serialLogger?.text = (activity as MainActivity).btReceiverThread?.fullString
        serialLogger?.movementMethod = ScrollingMovementMethod()
        connectionState?.text = savedInstanceState?.getCharSequence("connectionState")
        btnConnect?.text = savedInstanceState?.getCharSequence("connectBtnLabel")


        view.findViewById<Button>(R.id.btnConnect)?.setOnClickListener {
            (activity as MainActivity).initConnection()
        }

        view.findViewById<Button>(R.id.add_to_animation).setOnClickListener {
            var doubleDuration = -1.0
            val txtVal = durField.text
            try {
                doubleDuration = txtVal.toString().toDouble()
            } catch (e: NumberFormatException) {
                val alertDialog: AlertDialog? =
                    context?.let { it1 -> AlertDialog.Builder(it1).create() }
                alertDialog?.setTitle(resources.getString(R.string.napa_alert_duration_title))
                alertDialog?.setMessage(resources.getString(R.string.napa_alert_duration_message))
                alertDialog?.setButton(
                    AlertDialog.BUTTON_NEGATIVE, resources.getString(R.string.napa_alert_duration_ok)
                ) { dialog, _ -> dialog.cancel() }
                alertDialog?.show()
            }
            if (doubleDuration >= 0.0) {
                for (la in (activity as MainActivity).alFragment.animation.lampAnimations.withIndex()) {

                    if (la.index < 10 && lampBallSelectorUpper != null) {
                        val newstep = Step(
                            lampBallSelectorUpper!!.triangleColors[la.index].clone(),
                            (doubleDuration * 30).toLong(),
                            InterpolationType.LINEAR
                        )
                        la.value.steps.add(newstep)
                    } else {
                        val newstep = Step(
                            lampBallSelectorLower!!.triangleColors[la.index - 10].clone(),
                            (doubleDuration * 30).toLong(),
                            InterpolationType.LINEAR
                        )
                        la.value.steps.add(newstep)
                    }
                }
            }
        }

        initButtons()

        val ldLower = savedInstanceState?.getParcelable<LampSelectorData>("lampLowerData")
        val ldUpper = savedInstanceState?.getParcelable<LampSelectorData>("lampUpperData")
        if (ldLower != null)
        {
            lampBallSelectorLower?.lampData=ldLower
        }
        if (ldUpper != null)
        {
            lampBallSelectorUpper?.lampData=ldUpper
        }

    }

    override fun onSaveInstanceState(outState: Bundle) {
        super.onSaveInstanceState(outState)

        outState.putParcelable("lampLowerData",lampBallSelectorLower?.lampData)
        outState.putParcelable("lampUpperData",lampBallSelectorUpper?.lampData)
        outState.putCharSequence("connectionState",connectionState?.text)
        outState.putCharSequence( "connectBtnLabel",btnConnect?.text)
    }

    private fun initButtons()
    {
        view?.findViewById<Button>(R.id.buttonRed)?.setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_red)
        }
        view?.findViewById<Button>(R.id.buttonGreen)?.setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_green)
        }
        view?.findViewById<Button>(R.id.buttonBlue)?.setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_blue)
        }
        view?.findViewById<Button>(R.id.buttonWarmWhite)?.setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_war_white)
        }
        view?.findViewById<Button>(R.id.buttonColdWhite)?.setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_cold_white)
        }
        view?.findViewById<Button>(R.id.buttonBrightWhite)?.setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_bright)
        }
        view?.findViewById<Button>(R.id.buttonOff)?.setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_off)
        }

    }

    private fun setColorOnSelectorAndLamp( resourceIdClr: Int)
    {
        val clr = (activity as MainActivity).getColor(resourceIdClr)
        val siclr = SimpleIntColor(clr.red,clr.green,clr.blue)
        (activity as MainActivity).sendString("RGB(${clr.red},${clr.green},${clr.blue},0-19)\r")
        lampBallSelectorLower?.setColorForAll(siclr)
        lampBallSelectorUpper?.setColorForAll(siclr)
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_color_select, container, false)
    }


}