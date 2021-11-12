package ch.sr35.balllampapp

class SimpleIntColor(var r: Int, var g: Int, var b: Int) {




    fun clone(): SimpleIntColor
    {
        return SimpleIntColor(this.r,this.g,this.b)
    }
}