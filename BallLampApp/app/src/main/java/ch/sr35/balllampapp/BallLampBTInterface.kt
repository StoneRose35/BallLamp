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
            var state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,0)
            if (state == BluetoothAdapter.STATE_ON) {
                mainActivity.connectionState?.text = "Bluetooth Radio On"
                mainActivity.connectionInitActive = true
                mainActivity.initConnection()
            }
        }
        else if (intent?.action == BluetoothDevice.ACTION_FOUND)
        {
            var discoveredDevice = intent.getParcelableExtra<BluetoothDevice>(BluetoothDevice.EXTRA_DEVICE)
            if( discoveredDevice?.name== DEVICE_NAME)
            {
                mainActivity.connectionState?.text = "discovered $DEVICE_NAME"
                mainActivity.btAdapter?.cancelDiscovery()
                discoveredDevice.createBond()
            }
        }
        else if (intent?.action == BluetoothDevice.ACTION_BOND_STATE_CHANGED)
        {
            var bondState = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE,-1)
            if(bondState==BluetoothDevice.BOND_BONDED)
            {
                var bondedDevice = intent.getParcelableExtra<BluetoothDevice>(BluetoothDevice.EXTRA_DEVICE)
                mainActivity.btSocket = bondedDevice?.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"))
                try {
                    mainActivity.btSocket?.connect()
                    mainActivity.connectionState?.text = "Connected!"

                    mainActivity.btReceiverThread = BluetoothReceiverThread(mainActivity.btSocket?.inputStream!!,mainActivity.serialLogger!!)
                    mainActivity.btReceiverThread?.start()

                    mainActivity.sendString("API\r")
                } catch (e: IOException)
                {
                    mainActivity.btSocket = null
                    mainActivity.connectionState?.text = "Connection Timeout"
                }


            }
        }
    }

}
