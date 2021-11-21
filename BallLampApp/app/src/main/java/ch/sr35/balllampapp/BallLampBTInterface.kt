package ch.sr35.balllampapp
import android.bluetooth.*
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import java.io.IOException
import java.util.*


const val DEVICE_NAME = "BallLamp"
val appUuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")


class BTReceiver(var mainActivity: MainActivity ): BroadcastReceiver()
{
    override fun onReceive(context: Context?, intent: Intent?) {
        if (intent?.action == BluetoothAdapter.ACTION_STATE_CHANGED)
        {
            val state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,0)
            if (state == BluetoothAdapter.STATE_ON) {
                mainActivity.csFragment.connectionState?.text = "Bluetooth Radio On"
                mainActivity.connectionInitActive = true
                mainActivity.initConnection()
            }
        }
        else if (intent?.action == BluetoothDevice.ACTION_FOUND)
        {
            val discoveredDevice = intent.getParcelableExtra<BluetoothDevice>(BluetoothDevice.EXTRA_DEVICE)
            if( discoveredDevice?.name== DEVICE_NAME)
            {
                mainActivity.csFragment.connectionState?.text = "discovered $DEVICE_NAME"
                mainActivity.btAdapter?.cancelDiscovery()
                discoveredDevice.createBond()
            }
        }
        else if (intent?.action == BluetoothDevice.ACTION_BOND_STATE_CHANGED)
        {
            val bondState = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE,-1)
            if(bondState==BluetoothDevice.BOND_BONDED)
            {
                val bondedDevice = intent.getParcelableExtra<BluetoothDevice>(BluetoothDevice.EXTRA_DEVICE)
                mainActivity.btSocket = bondedDevice?.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"))
                try {
                    val connectorThread = BluetoothConnectionThread(mainActivity)
                    connectorThread.start()
                    mainActivity.csFragment.connectionState?.text = "Connecting"

                } catch (e: IOException)
                {
                    mainActivity.btSocket = null
                    mainActivity.csFragment.connectionState?.text = "Connection Timeout"
                }


            }
        }
    }

}
