package ch.sr35.balllampapp

import java.lang.Exception
import kotlin.math.sqrt

class TriangleCalc {
}

class Triangle(var a: Vertex,var b: Vertex,var c: Vertex)
{

    fun isInTriangle(p: Vertex): Boolean
    {
        var matA = b.x-a.x
        var matB = c.x-a.x
        var matC = b.y-a.y
        var matD = c.y-a.y
        var determinator = matA*matD-matB*matC
        if (determinator != 0.0)
        {
            determinator = 1.0/determinator
            var barU = (matD*(p.x-a.x) - matB*(p.y-a.y))*determinator
            var barV = (matA*(p.y-a.y) - matC*(p.x-a.x))*determinator
            return barU > 0 && barV > 0 && barU + barV < 1.0
        }
        else
        {
            throw InvalidTriangleException()
        }
    }

    fun inscribedRadius(): Double
    {
        var sidea=sqrt((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y))
        var sideb=sqrt((c.x-a.x)*(c.x-a.x) + (c.y-a.y)*(c.y-a.y))
        var sidec=sqrt((c.x-b.x)*(c.x-b.x) + (c.y-b.y)*(c.y-b.y))
        var s = 0.5*(sidea+sideb+sidec)
        return sqrt((s-sidea)*(s-sideb)*(s-sidec)/s)
    }

    fun centerCoordinates(): Vertex
    {
       var c = Vertex((a.x+b.x+c.x)/3.0,(a.y+b.y+c.y)/3.0)
       return c
    }
}

class Vertex(var x: Double,var y: Double) {}

class InvalidTriangleException: Exception()
{}