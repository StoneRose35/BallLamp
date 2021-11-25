package ch.sr35.balllampapp

import ch.sr35.balllampapp.backend.SimpleIntColor
import org.junit.Test

class SimpleIntColorTest {
    @Test
    fun colorEqualsTest()
    {
        val clr1 = SimpleIntColor(1,2,3)
        val clr2 = SimpleIntColor(1,2,3)
        assert(clr1 == clr2)
    }

    @Test
    fun colorNotEqualsTest()
    {
        val clr1 = SimpleIntColor(1,2,3)
        val clr2 = SimpleIntColor(3,2,1)
        assert(clr1 != clr2)
    }

    @Test
    fun distinctColorListTest()
    {
        var list=ArrayList<SimpleIntColor>()
        list.add(SimpleIntColor(1,2,3))
        list.add(SimpleIntColor(1,2,3))
        val nlist = list.distinctBy { e -> e }
        assert(nlist.count()==1)
    }
}