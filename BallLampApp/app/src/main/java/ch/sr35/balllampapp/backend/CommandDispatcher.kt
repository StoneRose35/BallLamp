package ch.sr35.balllampapp.backend

import ch.sr35.balllampapp.MainActivity

class CommandDispatcher(var commands: ArrayList<String>,val sender: MainActivity): Thread() {


    override fun run() {
       for (cmd in commands)
       {
           sender.sendString(cmd)
           Thread.sleep(10)
       }
    }
}