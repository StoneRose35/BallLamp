package ch.sr35.balllampapp

import kotlin.random.Random

class SimpleIntColor(var r: Int, var g: Int, var b: Int) {




    fun clone(): SimpleIntColor
    {
        return SimpleIntColor(this.r,this.g,this.b)
    }

    override fun equals(other: Any?): Boolean {
        if (other is SimpleIntColor)
        {
            return other.r == r && other.g == g && other.b == b
        }
        return false
    }

    companion object {
        var rg = Random(32)
        fun randomColor(): SimpleIntColor
        {

            return SimpleIntColor(rg.nextInt(0,255),rg.nextInt(0,255),rg.nextInt(0,255))
        }
    }
}