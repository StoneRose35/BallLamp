package ch.sr35.balllampapp

import org.junit.Test

class TriangleCalcTest {


    @Test
    fun isInTriangleTest()
    {
        var t = Triangle(Vertex(0.0,0.0), Vertex(3.0,0.0), Vertex(1.0,2.0))

        assert( t.isInTriangle(Vertex(1.0,1.0)))

        assert(!t.isInTriangle(Vertex(4.0,5.0)))
    }

    @Test
    fun specificIsInTriange()
    {
        var t = Triangle(Vertex(222.0,206.0),
            Vertex(300.704444528,97.672624453),Vertex(143.29555471803786,97.67264453))
        assert(!t.isInTriangle(Vertex(0.9619140625,-33.046875)))
    }
}