package ch.sr35.balllampapp

import android.content.Context
import android.content.res.Resources
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.Path
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import androidx.core.content.res.ResourcesCompat.getColor
import kotlin.math.PI
import kotlin.math.cos
import kotlin.math.sin


class LampSelectorView(context: Context,attributes: AttributeSet): View(context,attributes) {

    private var innerRadius: Double = 1.0
    private var outerRadius: Double = 1.4
    private var centerX: Double = 0.0
    private var centerY: Double = 0.0
    private var triangleColors: Array<SimpleIntColor> =
        Array(10) { SimpleIntColor(255, 255, 255) }
    private var triangleSelected: Array<Boolean> = Array<Boolean>(10){false}
    var triangleSelectedEventListener: TriangleSelectedEventListener? = null
    var mappingTable: IntArray? = null

    var path: Path = Path()
    private var blackLine: Paint = Paint()
    private var whiteLine: Paint = Paint()
    var filler: Paint = Paint()



    fun setColorForSelected(clr: SimpleIntColor)
    {
        var filteredData = triangleSelected.withIndex().filter { e-> e.value }
        for ((cnt, _) in triangleSelected.withIndex().filter { e-> e.value })
        {
            triangleColors[cnt] = clr.clone()
        }
        invalidate()
    }

    fun setColorForAll(clr: SimpleIntColor)
    {
        for (cnt in 0..9)
        {
            triangleColors[cnt] = clr.clone()
        }
        invalidate()
    }

    fun getSelectedString(): String
    {
        var res=""
        for ((cnt,_) in triangleSelected.withIndex().filter { e->e.value })
        {
            res += "${mappingTable?.get(cnt)},"
        }
        res = res.dropLast(1)
        return res
    }

    override fun onDraw(canvas: Canvas?) {

        super.onDraw(canvas)
        blackLine.color= Color.BLACK
        blackLine.strokeWidth=12.5f
        blackLine.strokeCap = Paint.Cap.ROUND
        blackLine.isAntiAlias = true

        whiteLine.color = Color.WHITE
        whiteLine.strokeWidth=3.5f
        whiteLine.strokeCap = Paint.Cap.ROUND
        whiteLine.isAntiAlias = true

        var phi0: Double
        var phi1: Double
        var coord: DoubleArray
        var coord1: DoubleArray
        var coord2: DoubleArray
        var triangleCntr = 0
        for (i in 0..4)
        {
            phi0 = i*2.0*PI/5.0
            phi1 = (i+1)*2.0*PI/5.0
            coord = toXYCoordinates(innerRadius, phi0)
            coord1 = toXYCoordinates(innerRadius,phi1)
            coord2 = toXYCoordinates(outerRadius,phi0 + PI/5.0)


            var triangle = Triangle(Vertex(0.0+ centerX,0.0+centerY), Vertex(coord[0]+centerX,coord[1]+centerY), Vertex(coord1[0]+centerX, coord1[1]+centerY))
            path = Path()
            path.moveTo(centerX.toFloat(),centerY.toFloat())
            path.lineTo(centerX.toFloat() +coord[0].toFloat(),centerY.toFloat() + coord[1].toFloat())
            path.lineTo(centerX.toFloat() + coord1[0].toFloat(),centerY.toFloat() +coord1[1].toFloat())

            path.close()
            filler.style = Paint.Style.FILL
            path.fillType = Path.FillType.EVEN_ODD
            filler.color = Color.argb(255,triangleColors[triangleCntr].r, triangleColors[triangleCntr].g,triangleColors[triangleCntr].b)
            filler.strokeCap = Paint.Cap.ROUND
            canvas?.drawPath(path,filler)

            if(triangleSelected[triangleCntr] )
            {
                var centerPt = triangle.centerCoordinates()
                filler.color = Color.argb(255, 255 - triangleColors[triangleCntr].r,
                    255 - triangleColors[triangleCntr].g,
                    255 - triangleColors[triangleCntr].b)
                canvas?.drawCircle(centerPt.x.toFloat(), centerPt.y.toFloat(),triangle.inscribedRadius().toFloat()*0.3f,filler)
            }

            triangleCntr++
            triangle = Triangle(Vertex(coord[0]+centerX,coord[1]+centerY), Vertex(coord1[0]+centerX,coord1[1]+centerY), Vertex(coord2[0]+centerX, coord2[1]+centerY))
            path = Path()
            path.moveTo(centerX.toFloat() + coord[0].toFloat(),centerY.toFloat() + coord[1].toFloat())
            path.lineTo(centerX.toFloat() + coord1[0].toFloat(),centerY.toFloat() +coord1[1].toFloat())
            path.lineTo(centerX.toFloat() + coord2[0].toFloat(),centerY.toFloat() +coord2[1].toFloat())
            path.close()
            //path.lineTo(centerX.toFloat() + coord[0].toFloat(),centerY.toFloat() + coord[0].toFloat())
            filler.style = Paint.Style.FILL
            path.fillType = Path.FillType.EVEN_ODD
            filler.color = Color.argb(255,triangleColors[triangleCntr].r, triangleColors[triangleCntr].g,triangleColors[triangleCntr].b)
            canvas?.drawPath(path,filler)



            // draw outer lines

            canvas?.drawLine(centerX.toFloat(),centerY.toFloat(),
                centerX.toFloat()+coord[0].toFloat(),centerY.toFloat()+coord[1].toFloat(),blackLine)
            canvas?.drawLine(centerX.toFloat()+coord[0].toFloat(),centerY.toFloat()+coord[1].toFloat(),
                centerX.toFloat() + coord1[0].toFloat(),centerY.toFloat() +  coord1[1].toFloat(),blackLine)
            canvas?.drawLine(centerX.toFloat() + coord[0].toFloat(),centerY.toFloat()+ coord[1].toFloat(),
                centerX.toFloat() + coord2[0].toFloat(),centerY.toFloat() + coord2[1].toFloat(),blackLine)
            canvas?.drawLine(centerX.toFloat() + coord1[0].toFloat(),centerY.toFloat()+ coord1[1].toFloat(),
                centerX.toFloat() + coord2[0].toFloat(),centerY.toFloat() + coord2[1].toFloat(),blackLine)

            canvas?.drawLine(centerX.toFloat(),centerY.toFloat(),
                centerX.toFloat()+coord[0].toFloat(),centerY.toFloat()+coord[1].toFloat(),whiteLine)
            canvas?.drawLine(centerX.toFloat()+coord[0].toFloat(),centerY.toFloat()+coord[1].toFloat(),
                centerX.toFloat() + coord1[0].toFloat(),centerY.toFloat() +  coord1[1].toFloat(),whiteLine)
            canvas?.drawLine(centerX.toFloat() + coord[0].toFloat(),centerY.toFloat()+ coord[1].toFloat(),
                centerX.toFloat() + coord2[0].toFloat(),centerY.toFloat() + coord2[1].toFloat(),whiteLine)
            canvas?.drawLine(centerX.toFloat() + coord1[0].toFloat(),centerY.toFloat()+ coord1[1].toFloat(),
                centerX.toFloat() + coord2[0].toFloat(),centerY.toFloat() + coord2[1].toFloat(),whiteLine)

            if(triangleSelected[triangleCntr] )
            {
                var centerPt = triangle.centerCoordinates()
                filler.color = Color.argb(255, 255 - triangleColors[triangleCntr].r,
                    255 - triangleColors[triangleCntr].g,
                    255 - triangleColors[triangleCntr].b)
                canvas?.drawCircle(centerPt.x.toFloat(), centerPt.y.toFloat(),triangle.inscribedRadius().toFloat()*0.3f,filler)
            }

            triangleCntr++
        }
        coord = toXYCoordinates(innerRadius, 0.0)
        canvas?.drawLine(centerX.toFloat(),centerY.toFloat(),
            centerX.toFloat()+coord[0].toFloat(),centerY.toFloat()+coord[1].toFloat(),blackLine)
        canvas?.drawLine(centerX.toFloat(),centerY.toFloat(),
            centerX.toFloat()+coord[0].toFloat(),centerY.toFloat()+coord[1].toFloat(),whiteLine)

    }

    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {

        if(w>h)
        {
            outerRadius = 0.95*h/2.0
            innerRadius = 0.65*h/2.0
        }
        else
        {
            outerRadius = 0.95*w/2.0
            innerRadius = 0.65*w/2.0
        }
        centerX = w/2.0
        centerY = h/2.0
        super.onSizeChanged(w, h, oldw, oldh)
    }

    override fun performClick(): Boolean {
        return super.performClick()
    }

    override fun onTouchEvent(event: MotionEvent?): Boolean {
        val xcoord= event?.x?.toDouble()
        val ycoord=event?.y?.toDouble()

        if (event?.action == MotionEvent.ACTION_DOWN) {
            var phi0: Double
            var phi1: Double
            var coord: DoubleArray
            var coord1: DoubleArray
            var coord2: DoubleArray
            var triangle: Triangle
            var triangleCntr: Int = 0
            if (xcoord != null && ycoord != null) {
                for (i in 0..4) {
                    phi0 = i * 2.0 * PI / 5.0
                    phi1 = (i + 1) * 2.0 * PI / 5.0
                    coord = toXYCoordinates(innerRadius, phi0)
                    coord1 = toXYCoordinates(innerRadius, phi1)
                    coord2 = toXYCoordinates(outerRadius, phi0 + PI / 5.0)
                    triangle = Triangle(
                        Vertex(0.0 + centerX, 0.0+ centerY),
                        Vertex(coord[0]+ centerX, coord[1]+ centerY),
                        Vertex(coord1[0]+ centerX, coord1[1]+centerY)
                    )
                    if (triangle.isInTriangle(Vertex(xcoord, ycoord))) {
                        triangleSelected[triangleCntr] = !triangleSelected[triangleCntr]
                        invalidate()
                        triangleSelectedEventListener?.onSelected(triangleCntr)
                    }
                    triangleCntr++
                    triangle = Triangle(
                        Vertex(coord[0]+centerX, coord[1]+centerY),
                        Vertex(coord1[0]+centerX, coord1[1]+centerY),
                        Vertex(coord2[0]+centerX, coord2[1]+centerY))

                    if (triangle.isInTriangle(Vertex(xcoord, ycoord))) {
                        triangleSelected[triangleCntr] = !triangleSelected[triangleCntr]
                        invalidate()
                        triangleSelectedEventListener?.onSelected(triangleCntr)

                    }
                    triangleCntr++
                }
            }
        }
        return super.onTouchEvent(event)
    }


    private fun toXYCoordinates(r: Double, phi: Double): DoubleArray
    {
        var res: DoubleArray=DoubleArray(2)
        res[0] = r* sin(phi)
        res[1] = r* cos(phi)
        return res
    }
}

interface TriangleSelectedEventListener
{
    fun onSelected(triangleNr: Int)
}