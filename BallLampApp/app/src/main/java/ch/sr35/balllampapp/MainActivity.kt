package ch.sr35.balllampapp

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.graphics.Color
import android.os.Handler
import android.widget.*
import androidx.core.graphics.blue
import androidx.core.graphics.green
import androidx.core.graphics.red
import java.io.IOException
import java.io.InputStream


const val COLOR_RED = 0
const val COLOR_GREEN = 1
const val COLOR_BLUE = 2

class MainActivity : AppCompatActivity() {
    var clrView: View? = null
    private var lbl: TextView? = null
    var connectionState: TextView? = null
    var mainClr: SimpleIntColor = SimpleIntColor(0,0,0)
    var lampBallSelectorUpper: LampSelectorView? = null
    var lampBallSelectorLower: LampSelectorView? = null
    var btSocket: BluetoothSocket? = null
    var btAdapter: BluetoothAdapter? = null
    var connectionInitActive: Boolean = true
    var btReceiverThread: BluetoothReceiverThread? = null
    var serialLogger: TextView? = null


    override fun onCreate(savedInstanceState: Bundle?) {
        val btMan: BluetoothManager = applicationContext.getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        btAdapter = btMan.adapter
        connectionInitActive = true

        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        findViewById<SeekBar>(R.id.amountRed).setOnSeekBarChangeListener(RGBSeekBarChangeListener(this,COLOR_RED))
        findViewById<SeekBar>(R.id.amountGreen).setOnSeekBarChangeListener(RGBSeekBarChangeListener(this,COLOR_GREEN))
        findViewById<SeekBar>(R.id.amountBlue).setOnSeekBarChangeListener(RGBSeekBarChangeListener(this,COLOR_BLUE))
        clrView = findViewById(R.id.colorView)
        lbl = findViewById(R.id.textViewLblLower)
        connectionState = findViewById(R.id.textViewConnectionState)
        serialLogger = findViewById(R.id.serialOut)
        lampBallSelectorUpper = findViewById(R.id.lampSelectorUpper)
        lampBallSelectorLower = findViewById(R.id.lampSelectorLower)

        lampBallSelectorUpper?.mappingTable = resources.getIntArray(R.array.lampMappingUpper)
        lampBallSelectorLower?.mappingTable = resources.getIntArray(R.array.lampMappingLower)


        findViewById<Button>(R.id.btnConnect).setOnClickListener {
            initConnection()
        }

        initButtons()

        val btReceiver = BTReceiver(this)
        registerReceiver(btReceiver, IntentFilter(BluetoothDevice.ACTION_FOUND))
        registerReceiver(btReceiver, IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED))
        registerReceiver(btReceiver, IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED))

    }


    fun initButtons()
    {
        findViewById<Button>(R.id.buttonRed).setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_red)
        }
        findViewById<Button>(R.id.buttonGreen).setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_green)
        }
        findViewById<Button>(R.id.buttonBlue).setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_blue)
        }
        findViewById<Button>(R.id.buttonWarmWhite).setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_war_white)
        }
        findViewById<Button>(R.id.buttonColdWhite).setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_cold_white)
        }
        findViewById<Button>(R.id.buttonBrightWhite).setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_bright)
        }
        findViewById<Button>(R.id.buttonOff).setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_off)
        }

    }

    private fun setColorOnSelectorAndLamp( resourceIdClr: Int)
    {
        val clr = getColor(resourceIdClr)
        val siclr = SimpleIntColor(clr.red,clr.green,clr.blue)
        sendString("RGB(${clr.red},${clr.green},${clr.blue},0-19)\r")
        lampBallSelectorLower?.setColorForAll(siclr)
        lampBallSelectorUpper?.setColorForAll(siclr)
    }

    override fun onPause() {
        super.onPause()
        btReceiverThread?.isRunning=false
    }

    override fun onResume() {
        super.onResume()
        btReceiverThread?.isRunning=true
        if (btReceiverThread?.isAlive != true)
        {
            btReceiverThread?.start()
        }
    }


    fun initConnection()
    {

        if (connectionInitActive) {
            if (btAdapter?.state == BluetoothAdapter.STATE_OFF) {
                val btOnIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
                connectionInitActive=false
                connectionState?.text =  getString(R.string.bt_switch_on)
                startActivity(btOnIntent)
            } else {
                val btDevice =
                    btAdapter?.bondedDevices?.firstOrNull { e -> e.name == DEVICE_NAME }
                if (btDevice == null) {
                    connectionState?.text = getString(R.string.bt_discovery)
                    btAdapter?.startDiscovery()
                    connectionInitActive=false
                } else {

                    btSocket = btDevice.createRfcommSocketToServiceRecord(appUuid)
                    try {
                        btSocket?.connect()
                        connectionState?.text = getString(R.string.bt_connected)

                        btReceiverThread = BluetoothReceiverThread(btSocket?.inputStream!!,serialLogger!!)
                        btReceiverThread?.start()

                        sendString("API\r")
                    } catch (e: IOException)
                    {
                        connectionState?.text = getString(R.string.bt_timeout)
                        btSocket = null
                    }
                }
            }
        }

    }

    fun sendString(str: String)
    {
        if(btSocket == null)
        {
            initConnection()
        }
        else
        {
            btSocket?.outputStream?.write(str.toByteArray())
        }

    }


}


class RGBSeekBarChangeListener(private var parentAct: MainActivity,private var clr: Int) : SeekBar.OnSeekBarChangeListener
{


    override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
        if (clr== COLOR_RED) {
            parentAct.mainClr.r = progress
        } else if (clr ==COLOR_GREEN)
        {
            parentAct.mainClr.g = progress
        } else if (clr == COLOR_BLUE)
        {
            parentAct.mainClr.b = progress
        }

        parentAct.clrView?.setBackgroundColor(Color.argb(255,parentAct.mainClr.r,parentAct.mainClr.g,parentAct.mainClr.b))
        parentAct.clrView?.invalidate()

        parentAct.lampBallSelectorUpper?.setColorForSelected(parentAct.mainClr)
        parentAct.lampBallSelectorLower?.setColorForSelected(parentAct.mainClr)

        val lampsUpper = parentAct.lampBallSelectorUpper?.getSelectedString()
        val lampsLower = parentAct.lampBallSelectorLower?.getSelectedString()

        var clrCmd = "RGB(${parentAct.mainClr.r},${parentAct.mainClr.g},${parentAct.mainClr.b})"
        if (lampsUpper != null) {
            if (lampsUpper.isNotEmpty())
            {
                clrCmd += ",$lampsUpper"
            }
        }
        if (lampsLower != null) {
            if (lampsLower.isNotEmpty())
            {
                clrCmd += ",$lampsLower"
            }
        }
        clrCmd += ")\r"
        parentAct.sendString(clrCmd)

    }

    override fun onStartTrackingTouch(seekBar: SeekBar?) {
        //TODO("Not yet implemented")
    }

    override fun onStopTrackingTouch(seekBar: SeekBar?) {
        //TODO("Not yet implemented")
    }
}



class BluetoothReceiverThread(var inputStream: InputStream,var logger: TextView): Thread()
{
    private val handler: Handler = Handler()
    var isRunning = true
    @Override
    override fun run() {
        while(isRunning)
        {
            val bytesAvailable = inputStream.available()
            if(bytesAvailable > 0)
            {
                val b = ByteArray(bytesAvailable)
                inputStream.read(b)
                val addedText = b.decodeToString()
                handler.post {
                    logger.text = addedText
                }

            }
            sleep(100)
        }
    }
}