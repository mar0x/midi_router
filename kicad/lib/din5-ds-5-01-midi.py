import cadquery as cq

shole = cq.Workplane("XZ") \
    .rect(0.6, 3.7) \
    .extrude(11.8) \
    .union(
        cq.Workplane("XZ") \
        .circle(0.7) \
        .extrude(11.8) \
    ) \
    .translate((0, 11.8 / 2 + (11.8 - 13.5) / 2, 0))

body = cq.Workplane("XY") \
    .rect(17.8, 13.5) \
    .extrude(3) \
    .faces(">Y").workplane(centerOption="CenterOfBoundBox") \
        .rect(0.6, 3) \
        .cutBlind(-1.5) \
    .faces(">Y").workplane(centerOption="CenterOfBoundBox") \
        .rarray(10, 1, 2, 1) \
        .rect(0.6, 3) \
        .cutBlind(-1.5) \
    .faces(">Y").workplane(centerOption="CenterOfBoundBox") \
        .rarray(14.7, 1, 2, 1) \
        .rect(0.6, 3) \
        .cutBlind(-1.5) \
    .union(
        cq.Workplane("XY") \
        .center(0, (11.8 - 13.5) / 2) \
        .rect(17.8, 11.8) \
        .extrude(6.4)
    ) \
    .faces(">Y").workplane(centerOption="CenterOfBoundBox") \
        .center(0, 1.5 / 2) \
        .rarray(8.7, 1, 2, 1) \
        .rect(2.9, 1.5) \
        .cutBlind(-10) \
    .union(
        cq.Workplane("XY") \
        .center(0, (2.5 - 13.5) / 2) \
        .rect(17.8, 2.5) \
        .extrude(20) \
        .faces(">Y").workplane(centerOption="CenterOfBoundBox") \
            .circle(15.7 / 2) \
            .extrude(11.8 - 2.5)
        .faces(">Y").workplane(centerOption="CenterOfBoundBox") \
            .center(0, 3) \
            .rect(5, 1) \
            .cutBlind(-10) \
        .faces(">Y").workplane(centerOption="CenterOfBoundBox") \
            .center(0, 5.3) \
            .rect(2.5, 2.5) \
            .cutBlind(-1.8) \
        .faces(">Y").workplane(centerOption="CenterOfBoundBox") \
            .center(0, 5.3 + 2.5) \
            .rect(2.5, 2.5) \
            .cutBlind(-11.8 + 2.5) \
        .faces(">Y").workplane(centerOption="CenterOfBoundBox") \
            .moveTo(0, 3.5) \
            .polyline( [
                (5, 3.5), (5, 1.7), (3.1, -0.1),
                (3.1, -1.9), (0.8, -3.5), (0, -3.5),
                (-0.8, -3.5), (-3.1, -1.9), (-3.1, -0.1),
                (-5, 1.7), (-5, 3.5) ] ) \
            .close() \
            .extrude(1.5) \
        .faces(">Y").workplane(centerOption="CenterOfBoundBox") \
            .circle(2.8 / 2) \
            .extrude(0.2)
    ) \
    .union(
        cq.Workplane("XY") \
        .center(0, (1.1 - 13.5) / 2) \
        .rect(20, 1.1) \
        .extrude(20)
    ) \
    .faces("<Y").workplane(centerOption="CenterOfBoundBox") \
        .center(0, -20 / 2 + 3 / 2) \
        .rect(11, 3) \
        .cutBlind(-3.2) \
    .faces("<Y").workplane(centerOption="CenterOfBoundBox") \
        .center(0, -20 / 2 + 3 + 1 / 2) \
        .rect(5.4, 1) \
        .cutBlind(-9.4) \
    .faces("<Y").workplane(centerOption="CenterOfBoundBox") \
        .center(0, -20 / 2 + 1.5 + 1.5 / 2) \
        .rect(11, 1.5) \
        .cutBlind(-9.4) \
    .faces("<Y").workplane(centerOption="CenterOfBoundBox") \
        .center(0, 5.3) \
        .rect(2.5, 2.5) \
        .cutBlind(-9.4) \
    .faces("<Y").workplane(centerOption="CenterOfBoundBox") \
        .circle(13.6 / 2) \
        .circle(11.5 / 2) \
        .cutBlind(-9.4) \
    .faces("<Y").workplane(centerOption="CenterOfBoundBox") \
        .hole(13.6, 0.8) \
    .cut( shole.translate((0, 0, 20/2 - 3.5)) ) \
    .cut(
        shole \
        .rotate((0,0,0), (0, 1, 0), -10) \
        .translate((0, 0, -3.5)) \
        .rotate((0,0,0), (0, 1, 0), 45) \
        .translate((0, 0, 20/2))
    ) \
    .cut(
        shole \
        .rotate((0,0,0), (0, 1, 0), 45) \
        .translate((-3.5, 0, 20/2))
    ) \
    .cut(
        shole \
        .rotate((0,0,0), (0, 1, 0), 10) \
        .translate((0, 0, -3.5)) \
        .rotate((0,0,0), (0, 1, 0), -45) \
        .translate((0, 0, 20/2))
    ) \
    .cut(
        shole \
        .rotate((0,0,0), (0, 1, 0), -45) \
        .translate((3.5, 0, 20/2))
    ) \

l1 = cq.Workplane("XY") \
    .moveTo(0, 0) \
      .vLine(9.7) \
      .hLine(2) \
      .vLine(1.7) \
      .hLine(5.7) \
      .vLine(-0.5) \
      .hLine(4.3) \
      .vLine(-1) \
      .hLine(-12 + 3.7) \
      .vLine(1.5 - 11.4) \
      .hLine(-(3.7 - 1.2) / 2) \
      .vLine(1) \
      .line((1.5 - 1.2) / 2, 1) \
      .vLine(7 - 1 - 1.5/2 - 1) \
      .radiusArc(( (3.7 - 1.5) / 2, 9.7 - 2.7 - 1.5 / 2 ), -1.5 / 2 ) \
      .vLine(-7 + 1 + 1.5/2 + 1) \
      .line((1.5 - 1.2) / 2, -1) \
      .vLine(-1) \
      .close() \
    .extrude(0.6) \
    .edges("|Z").chamfer(0.1) \
    .translate((-3.7 / 2, 0, -0.6 / 2)) \
    .rotate((0, 0, 0), (0, 1, 0), 90) \
    .translate((0, -9.7 + 13.5 / 2 - (13.5 - 11.8), 20/2 - 3.5))

l2 = cq.Workplane("XY") \
    .moveTo(0, 0) \
      .vLine(9.7).hLine(2) \
      .vLine(3.3).hLine(3.5) \
      .vLine(-3.2).hLine(-1.8) \
      .vLine(3.2 - 3.3 - 9.7) \
      .hLine(-(3.7 - 1.2) / 2) \
      .vLine(1) \
      .line((1.5 - 1.2) / 2, 1) \
      .vLine(7 - 1 - 1.5/2 - 1) \
      .radiusArc(( (3.7 - 1.5) / 2, 9.7 - 2.7 - 1.5 / 2 ), -1.5 / 2 ) \
      .vLine(-7 + 1 + 1.5/2 + 1) \
      .line((1.5 - 1.2) / 2, -1) \
      .vLine(-1) \
      .close() \
    .extrude(0.6) \
    .edges("|Z").chamfer(0.1) \
    .translate((-3.7 / 2, 0, -0.6 / 2)) \
    .rotate((0, 0, 0), (0, 1, 0), 90 - 10) \
    .translate((0, 0, -3.5)) \
    .rotate((0, 0, 0), (0, 1, 0), 45) \
    .translate((0, 0, 20 / 2)) \
    .union(
        cq.Workplane("XY") \
        .moveTo(0, 0) \
        .hLine(7.4) \
        .vLine(-1) \
        .hLine(-3.8) \
        .vLine(-3.2 + 1) \
        .hLine(-3.6) \
        .close() \
        .extrude(0.6) \
        .faces("<X").workplane(centerOption="CenterOfBoundBox") \
        .rect(3.2, 0.6) \
        .revolve(35, (0, 0.7), (10, 0.7)) \
        .faces("<X").workplane(centerOption="CenterOfBoundBox") \
        .rect(3.2, 0.6) \
        .extrude(2) \
        .edges("|Z").chamfer(0.1) \
        .translate((-3.0, 13, -0.6 / 2)) \
        .rotate((0, 0, 0), (0, 1, 0), 90) \
        .translate((-10 / 2, 0, 0.7)) \
    ) \
    .translate((0, -9.7 + 13.5 / 2 - (13.5 - 11.8), 0))

l3 = cq.Workplane("XY") \
    .moveTo(0, 0) \
      .vLine(9.7).hLine(2) \
      .vLine(3.3).hLine(3.5) \
      .vLine(-3.2).hLine(-1.8) \
      .vLine(3.2 - 3.3 - 9.7) \
      .hLine(-(3.7 - 1.2) / 2) \
      .vLine(1) \
      .line((1.5 - 1.2) / 2, 1) \
      .vLine(7 - 1 - 1.5/2 - 1) \
      .radiusArc(( (3.7 - 1.5) / 2, 9.7 - 2.7 - 1.5 / 2 ), -1.5 / 2 ) \
      .vLine(-7 + 1 + 1.5/2 + 1) \
      .line((1.5 - 1.2) / 2, -1) \
      .vLine(-1) \
      .close() \
    .extrude(0.6) \
    .edges("|Z").chamfer(0.1) \
    .translate((-3.7 / 2, 0, -0.6 / 2)) \
    .rotate((0, 0, 0), (0, 1, 0), 90 + 10) \
    .translate((0, 0, -3.5)) \
    .rotate((0, 0, 0), (0, 1, 0), -45) \
    .translate((0, 0, 20 / 2)) \
    .union(
        cq.Workplane("XY") \
        .moveTo(0, 0) \
        .hLine(7.4) \
        .vLine(-1) \
        .hLine(-3.8) \
        .vLine(-3.2 + 1) \
        .hLine(-3.6) \
        .close() \
        .extrude(0.6) \
        .faces("<X").workplane(centerOption="CenterOfBoundBox") \
        .rect(3.2, 0.6) \
        .revolve(35, (0, -0.7), (-10, -0.7)) \
        .faces("<X").workplane(centerOption="CenterOfBoundBox") \
        .rect(3.2, 0.6) \
        .extrude(2) \
        .edges("|Z").chamfer(0.1) \
        .translate((-3.0, 13, -0.6 / 2)) \
        .rotate((0, 0, 0), (0, 1, 0), 90) \
        .translate((10 / 2, 0, 0.7)) \
    ) \
    .translate((0, -9.7 + 13.5 / 2 - (13.5 - 11.8), 0))

l4 = cq.Workplane("XY") \
    .moveTo(0, 0) \
      .vLine(9.7) \
      .hLine(2) \
      .vLine(1.7) \
      .hLine(3.40) \
      .vLine(-1.5) \
      .hLine(-1.70) \
      .vLine(1.5 - 1.7 - 9.7) \
      .hLine(-(3.7 - 1.2) / 2) \
      .vLine(1) \
      .line((1.5 - 1.2) / 2, 1) \
      .vLine(7 - 1 - 1.5/2 - 1) \
      .radiusArc(( (3.7 - 1.5) / 2, 9.7 - 2.7 - 1.5 / 2 ), -1.5 / 2 ) \
      .vLine(-7 + 1 + 1.5/2 + 1) \
      .line((1.5 - 1.2) / 2, -1) \
      .vLine(-1) \
      .close() \
    .extrude(0.6) \
    .edges("|Z").chamfer(0.1) \
    .translate((-3.7 / 2, 0, -0.6 / 2)) \
    .rotate((0, 0, 0), (0, 1, 0), 90 - 45) \
    .translate((0, 0, -3.5)) \
    .rotate((0, 0, 0), (0, 1, 0), 90) \
    .translate((0, 0, 20 / 2)) \
    .union(
        cq.Workplane("XY") \
        .moveTo(0.0, 11.4) \
        .hLine(5.2) \
        .vLine(-0.5) \
        .hLine(4.3) \
        .vLine(-1) \
        .hLine(-5.2 - 4.3) \
        .close() \
        .extrude(0.6) \
        .faces("<X").workplane(centerOption="CenterOfBoundBox") \
        .rect(1.5, 0.6) \
        .revolve(45, (0, 0.6), (10, 0.6)) \
        .faces("<X").workplane(centerOption="CenterOfBoundBox") \
        .rect(1.5, 0.6) \
        .extrude(2) \
        .edges("|Z").chamfer(0.1) \
        .translate((-5.2, 0, -0.6 / 2)) \
        .rotate((0, 0, 0), (0, 1, 0), 90) \
        .translate((-14.7 / 2, 0, 0.7)) \
    ) \
    .translate((0, -9.7 + 13.5 / 2 - (13.5 - 11.8), 0))

l5 = cq.Workplane("XY") \
    .moveTo(0, 0) \
      .vLine(9.7) \
      .hLine(2) \
      .vLine(1.7) \
      .hLine(3.40) \
      .vLine(-1.5) \
      .hLine(-1.70) \
      .vLine(1.5 - 1.7 - 9.7) \
      .hLine(-(3.7 - 1.2) / 2) \
      .vLine(1) \
      .line((1.5 - 1.2) / 2, 1) \
      .vLine(7 - 1 - 1.5/2 - 1) \
      .radiusArc(( (3.7 - 1.5) / 2, 9.7 - 2.7 - 1.5 / 2 ), -1.5 / 2 ) \
      .vLine(-7 + 1 + 1.5/2 + 1) \
      .line((1.5 - 1.2) / 2, -1) \
      .vLine(-1) \
      .close() \
    .extrude(0.6) \
    .edges("|Z").chamfer(0.1) \
    .translate((-3.7 / 2, 0, -0.6 / 2)) \
    .rotate((0, 0, 0), (0, 1, 0), 90 + 45) \
    .translate((0, 0, -3.5)) \
    .rotate((0, 0, 0), (0, 1, 0), -90) \
    .translate((0, 0, 20 / 2)) \
    .union(
        cq.Workplane("XY") \
        .moveTo(0.0, 11.4) \
        .hLine(5.2) \
        .vLine(-0.5) \
        .hLine(4.3) \
        .vLine(-1) \
        .hLine(-5.2 - 4.3) \
        .close() \
        .extrude(0.6) \
        .faces("<X").workplane(centerOption="CenterOfBoundBox") \
        .rect(1.5, 0.6) \
        .revolve(45, (0, -0.6), (-10, -0.6)) \
        .faces("<X").workplane(centerOption="CenterOfBoundBox") \
        .rect(1.5, 0.6) \
        .extrude(2) \
        .edges("|Z").chamfer(0.1) \
        .translate((-5.2, 0, -0.6 / 2)) \
        .rotate((0, 0, 0), (0, 1, 0), 90) \
        .translate((14.7 / 2, 0, 0.7)) \
    ) \
    .translate((0, -9.7 + 13.5 / 2 - (13.5 - 11.8), 0))

l6 = cq.Workplane("XY") \
    .rect(11, 9) \
    .extrude(0.3) \
    .faces("<Y").workplane(centerOption="CenterOfBoundBox") \
    .rect(11, 0.3) \
    .revolve(90, (0, -0.4), (-10, -0.4)) \
    .faces("<Z").workplane(centerOption="CenterOfBoundBox") \
    .rect(11, 0.3) \
    .extrude(4) \
    .translate((0, 0, 1.5))

body_color = (103, 103, 103, 0)
body_options = {"rgba": body_color}

leg_color = (200, 200, 200, 0)
leg_options = {"rgba": leg_color}

if __name__ == '__cqgi__':
    show_object(body, options=body_options)

    show_object(l1, options=leg_options)
    show_object(l2, options=leg_options)
    show_object(l3, options=leg_options)
    show_object(l4, options=leg_options)
    show_object(l5, options=leg_options)
    show_object(l6, options=leg_options)
