package ch.sr35.balllampapp

import android.app.Activity
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.graphics.Color
import android.os.Handler
import android.widget.*
import org.w3c.dom.Text
import java.io.IOException
import java.io.InputStream
import java.util.*
import java.util.concurrent.Executor

const val COLOR_RED = 0
const val COLOR_GREEN = 1
const val COLOR_BLUE = 2

class MainActivity : AppCompatActivity() {
    var clrView: View? = null
    var lbl: TextView? = null
    var connectionState: TextView? = null
    var mainClr: simpleIntColor = simpleIntColor(0,0,0)
    var btSocket: BluetoothSocket? = null
    var btAdapter: BluetoothAdapter? = null
    var connectionInitActive: Boolean = true
    var btReceiverThread: BluetoothReceiverThread? = null
    var serialLogger: TextView? = null


    override fun onCreate(savedInstanceState: Bundle?) {
        var btMan: BluetoothManager = applicationContext.getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        btAdapter = btMan.adapter
        connectionInitActive = true

        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        findViewById<SeekBar>(R.id.amountRed).setOnSeekBarChangeListener(RGBSeekBarChangeListener(this,COLOR_RED))
        findViewById<SeekBar>(R.id.amountGreen).setOnSeekBarChangeListener(RGBSeekBarChangeListener(this,COLOR_GREEN))
        findViewById<SeekBar>(R.id.amountBlue).setOnSeekBarChangeListener(RGBSeekBarChangeListener(this,COLOR_BLUE))
        clrView = findViewById<View>(R.id.colorView)
        lbl = findViewById<TextView>(R.id.textView)
        connectionState = findViewById(R.id.textViewConnectionState)
        serialLogger = findViewById(R.id.serialOut)



        findViewById<Button>(R.id.btnConnect).setOnClickListener {
            initConnection()
        }

        findViewById<Button>(R.id.btnSendHelp).setOnClickListener {
            sendString("HELP\r")
        }

        var btReceiver = BTReceiver(this)
        registerReceiver(btReceiver, IntentFilter(BluetoothDevice.ACTION_FOUND))
        registerReceiver(btReceiver, IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED))
        registerReceiver(btReceiver, IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED))

    }

    override fun onPause() {
        super.onPause()
        btReceiverThread?.isRunning=false
    }

    override fun onResume() {
        super.onResume()
        btReceiverThread?.isRunning=true
        if (btReceiverThread?.isAlive() != true)
        {
            btReceiverThread?.start()
        }
    }


    fun initConnection()
    {

        if (connectionInitActive) {
            if (btAdapter?.state == BluetoothAdapter.STATE_OFF) {
                var btOnIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
                connectionInitActive=false
                connectionState?.text = "Switching on Bluetooth"
                startActivity(btOnIntent)
            } else {
                var btDevice =
                    btAdapter?.bondedDevices?.firstOrNull { e -> e.name == DEVICE_NAME }
                if (btDevice == null) {
                    connectionState?.text = "Discovering.."
                    btAdapter?.startDiscovery()
                    connectionInitActive=false
                } else {

                    btSocket = btDevice.createRfcommSocketToServiceRecord(appUuid)
                    try {
                        btSocket?.connect()
                        connectionState?.text = "Connected!"

                        btReceiverThread = BluetoothReceiverThread(btSocket?.inputStream!!,serialLogger!!)
                        btReceiverThread?.start()
                    } catch (e: IOException)
                    {
                        connectionState?.text = "Connection Timeout"
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


    fun updateLogger(str: String)
    {
        serialLogger?.text = str
    }

    @Override
    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode == 42 )
        {
            if(resultCode==BluetoothAdapter.STATE_ON)
            {

            }
            Toast.makeText(applicationContext,"received as response from bluetooth enable request",Toast.LENGTH_SHORT).show()
        }
        else
        {
            Toast.makeText(applicationContext,"received as response from bluetooth enable request",Toast.LENGTH_SHORT).show()
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

        parentAct.lbl?.text = "Color: 1,${parentAct.mainClr.r},${parentAct.mainClr.g},${parentAct.mainClr.b}"
        parentAct.clrView?.setBackgroundColor(Color.argb(255,parentAct.mainClr.r,parentAct.mainClr.g,parentAct.mainClr.b))
        parentAct.clrView?.invalidate()

        parentAct.sendString("  RGB(${parentAct.mainClr.r},${parentAct.mainClr.g},${parentAct.mainClr.b},0-19)\r\n")
    }

    override fun onStartTrackingTouch(seekBar: SeekBar?) {
        //TODO("Not yet implemented")
    }

    override fun onStopTrackingTouch(seekBar: SeekBar?) {
        //TODO("Not yet implemented")
    }
}

class BTReceiver(var mainActivity: MainActivity ): BroadcastReceiver()
{
    override fun onReceive(context: Context?, intent: Intent?) {
        if (intent?.action == BluetoothAdapter.ACTION_STATE_CHANGED)
        {
            var state = intent?.getIntExtra(BluetoothAdapter.EXTRA_STATE,0)
            if (state == BluetoothAdapter.STATE_ON) {
                mainActivity.connectionState?.text = "Bluetooth Radio On"
                mainActivity.connectionInitActive = true
                mainActivity.initConnection()
            }
        }
        else if (intent?.action == BluetoothDevice.ACTION_FOUND)
        {
            var discoveredDevice = intent?.getParcelableExtra<BluetoothDevice>(BluetoothDevice.EXTRA_DEVICE)
            if( discoveredDevice?.name== DEVICE_NAME)
            {
                mainActivity.connectionState?.text = "discovered $DEVICE_NAME"
                mainActivity.btAdapter?.cancelDiscovery()
                discoveredDevice.createBond()
            }
        }
        else if (intent?.action == BluetoothDevice.ACTION_BOND_STATE_CHANGED)
        {
            var bondState = intent?.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE,-1)
            if(bondState==BluetoothDevice.BOND_BONDED)
            {
                var bondedDevice = intent?.getParcelableExtra<BluetoothDevice>(BluetoothDevice.EXTRA_DEVICE)
                mainActivity.btSocket = bondedDevice?.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"))
                try {
                    mainActivity.btSocket?.connect()
                    mainActivity.connectionState?.text = "Connected!"

                    mainActivity.btReceiverThread = BluetoothReceiverThread(mainActivity.btSocket?.inputStream!!,mainActivity.serialLogger!!)
                    mainActivity.btReceiverThread?.start()
                } catch (e: IOException)
                {
                    mainActivity.btSocket = null
                    mainActivity.connectionState?.text = "Connection Timeout"
                }


            }
        }
    }

}

class BluetoothReceiverThread(var inputStream: InputStream,var logger: TextView): Thread()
{
    var handler: Handler = Handler()
    var isRunning = true
    @Override
    override fun run() {
        while(isRunning)
        {
            var bytesAvailable = inputStream.available()
            if(bytesAvailable > 0)
            {
                var b: ByteArray = ByteArray(bytesAvailable)
                inputStream.read(b)
                var addedText = b.decodeToString()
                handler.post {
                    logger.text = addedText
                }

            }
            Thread.sleep(100)
        }
    }
}