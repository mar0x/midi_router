# This is a CadQuery script template
# Add your script code below
import cadquery as cq
import math

flat = True
#midi_sock_count = 7
midi_sock_count = 4
#midi_sock_count = 3  # Splitter x6
#midi_sock_count = 1

if midi_sock_count == 7:
    board_length = 160
    board_width = 80
    board_thickness = 1.6
    board_clearance = 0.6
    board_corner_r = 10
    fillet_r = 3

    box_inner_height = 33
    box_thickness = 2

    base_stand_d = 8
    screw_cup_hole_d = 5.5
    base_edge_height = 2

    base_stand_h = 3

    cover_stand_d = base_stand_d
    cover_stand_d1 = 10
    cover_edge_height = 4
    cover_clearance = 0.2

    stand_x_step = 150
    stand_y_step = 66
    central_stand_x_step = 80

    midi_sock_step = 20
    midi_sock_d = 16
    midi_sock_shift = 10 # above board
    led_shift = midi_sock_shift + 13 # above board
    led_size = (5.2, 2.7) # rect
    # or led_size = 2 for round hole

    usb_led_step = 20
    usb_led_count = 3

    rst_hole_z = 4 # above board
    rst_hole_y = 0 # from center line

if midi_sock_count == 4 and not flat:
    board_length = 96
    board_width = 50
    board_thickness = 1.2
    board_clearance = 0.6
    board_corner_r = 3
    fillet_r = 3

    box_inner_height = 33
    box_thickness = 2

    base_stand_d = 8
    screw_cup_hole_d = 5.5
    base_edge_height = 1

    base_stand_h = 3

    cover_stand_d = base_stand_d
    cover_stand_d1 = 10
    cover_edge_height = 1
    cover_clearance = 0.1

    stand_x_step = 85.5
    stand_y_step = 38.5
    central_stand_x_step = 40

    midi_sock_step = 20.32
    midi_sock_d = 16
    midi_sock_shift = 10 # above board
    led_shift = midi_sock_shift + 15 # above board
    led_size = 2 # round hole
    # or led_size = (5.2, 2.7) for rect

    usb_led_step = 6
    usb_led_count = 3

    rst_hole_z = 14 # above board
    rst_hole_y = 7 # from center line

if midi_sock_count == 4 and flat:
    board_length = 100
    board_width = 80
    board_thickness = 1.5
    board_clearance = 0.6
    board_corner_r = 10
    fillet_r = 3

    box_inner_height = 33 - 4
    box_thickness = 2

    base_stand_d = 8
    screw_cup_hole_d = 5.5
    base_edge_height = 2

    base_stand_h = 3

    cover_stand_d = base_stand_d
    cover_stand_d1 = 10
    cover_edge_height = 4
    cover_clearance = 0.2

    stand_x_step = 90
    stand_y_step = 66
    central_stand_x_step = 40

    midi_sock_step = 20
    midi_sock_d = 16
    midi_sock_shift = 10 # above board
    led_shift = midi_sock_shift + 13 - 2 # above board
    led_size = 2 # round hole
    # or led_size = (5.2, 2.7) for rect

    usb_led_step = 20
    usb_led_count = 3

    rst_hole_z = 4 # above board
    rst_hole_y = 0 # from center line


if midi_sock_count == 3:
    board_length = 96
    board_width = 50
    board_thickness = 1.2
    board_clearance = 0.6
    board_corner_r = 3
    fillet_r = 3

    box_inner_height = 28
    box_thickness = 2

    base_stand_d = 8
    screw_cup_hole_d = 5.5
    base_edge_height = 1

    base_stand_h = 3

    cover_stand_d = base_stand_d
    cover_stand_d1 = 10
    cover_edge_height = 1
    cover_clearance = 0.1

    stand_x_step = 85.5
    stand_y_step = 38.5
    central_stand_x_step = 40

    midi_sock_step = 20.32
    midi_sock_d = 16
    midi_sock_shift = 10 # above board
    led_shift = midi_sock_shift + 5 # above board
    led_size = 2 # round hole
    # or led_size = (5.2, 2.7) for rect

    usb_led_step = 6
    usb_led_count = 1
    usb_y_shift = -10
    usb_led_shift = 6.5

    # MIDI IN led hole
    rst_hole_z = midi_sock_shift # above board
    rst_hole_y = -12.7 # from center line


if midi_sock_count == 1:
    board_length = 30
    board_width = 30
    board_thickness = 1.6
    board_clearance = 0.3
    board_corner_r = 3
    fillet_r = 3

    box_inner_height = 28
    box_thickness = 2

    base_stand_d = 6
    screw_cup_hole_d = 4
    base_edge_height = 1

    base_stand_h = 3

    cover_stand_d = base_stand_d
    cover_stand_d1 = 8
    cover_edge_height = 1
    cover_clearance = 0.1

    stand_x_step = 25.5
    stand_y_step = 20.5
    central_stand_x_step = 40

    midi_sock_step = 20.32
    midi_sock_d = 16
    midi_sock_shift = 10 # above board
    led_shift = midi_sock_shift + 11 # above board
    led_size = 2 # round hole
    # or led_size = (5.2, 2.7) for rect

    usb_led_step = 6
    usb_led_count = 3


box_inner_length = board_length + 2 * board_clearance
box_inner_width = board_width + 2 * board_clearance

box_outer_length = box_inner_length + 2 * box_thickness
box_outer_width = box_inner_width + 2 * box_thickness
box_outer_height = box_inner_height + box_thickness

# 2.6 x 12 mm screw (d=4.0 mm head)
# base_stand_hole_d = 3.0
# cover_stand_hole_d = 2.5

# 2.3 x 12 mm screw (d=3.6 mm head)
base_stand_hole_d = 2.6
cover_stand_hole_d = 2.2

cover_stand_h = box_inner_height + 1 - base_stand_h - board_thickness - 0.3

edge_center_shift = (stand_x_step / 2) ** 2 + (stand_y_step / 2)  ** 2 - (central_stand_x_step / 2) ** 2
edge_center_shift = edge_center_shift / stand_y_step
edge_rad = math.sqrt( (central_stand_x_step / 2) ** 2 + edge_center_shift ** 2)

# because of (3mm) fillet which rises face edge above
# face height is (box_outer_height - fillet_r)
# face edge is fillet_r above 0
board_top_shift = - (box_outer_height - fillet_r) / 2 - fillet_r + (box_thickness + base_stand_h + board_thickness)

usb_width = 12
usb_height = 11

if 'usb_led_shift' not in locals():
    usb_led_shift = led_shift

if 'usb_y_shift' not in locals():
    usb_y_shift = 0

if 'usb_led_count' not in locals():
    usb_led_count = 3

rst_hole_d = 2

usb_z_shift = box_thickness + base_stand_h + board_thickness + usb_height / 2


# New method to render script results using the CadQuery Gateway Interface
# Use the following to render your model with grey RGB and no transparency
# show_object(result, options={"rgba":(204, 204, 204, 0.0)})

stand_centers = [
      (-stand_x_step / 2, -stand_y_step / 2),
      ( stand_x_step / 2, -stand_y_step / 2),
      ( stand_x_step / 2,  stand_y_step / 2),
      (-stand_x_step / 2,  stand_y_step / 2) ]

if midi_sock_count == 7:
    stand_centers.extend( [
        (-central_stand_x_step / 2, 0),
        ( central_stand_x_step / 2, 0) ] )

# make the box
base = cq.Workplane("XY").rect(box_outer_length, box_outer_width) \
        .extrude(box_outer_height) \
        .edges("|Z").fillet(board_corner_r + board_clearance + box_thickness) \
        .faces("<Z").fillet(fillet_r)

# turn box to shell
base = base.faces(">Z").shell(-box_thickness)

# extrude base stands
base = base.faces("<Z[-2]").workplane(centerOption="CenterOfBoundBox") \
        .pushPoints(stand_centers) \
        .circle(base_stand_d / 2).extrude(base_stand_h)

# making holes in base
base = base.faces("<Z").workplane(centerOption="CenterOfBoundBox") \
        .pushPoints(stand_centers) \
        .cboreHole(base_stand_hole_d, screw_cup_hole_d, 2)

# making holes for midi sockets and leds
base = base.faces(">Y").workplane(centerOption="CenterOfBoundBox") \
        .center(0, board_top_shift + midi_sock_shift) \
        .rarray(midi_sock_step, 1, midi_sock_count, 1, True) \
        .hole(midi_sock_d)

# making rst hole
base = base.faces(">X").workplane(centerOption="CenterOfBoundBox") \
        .center(rst_hole_y, board_top_shift + rst_hole_z) \
        .hole(rst_hole_d, box_thickness)

midi_led_plane = base.faces(">Y").workplane(centerOption="CenterOfBoundBox") \
        .center(0, board_top_shift + led_shift) \
        .rarray(midi_sock_step, 1, midi_sock_count, 1, True)

if isinstance(led_size, (list, tuple)):
    base = midi_led_plane.rect(*led_size).cutBlind(-box_outer_width)
else:
    base = midi_led_plane.hole(led_size)

usb_led_plane = base.faces("<X").workplane(centerOption="CenterOfBoundBox") \
        .center(0, board_top_shift + usb_led_shift) \
        .rarray(usb_led_step, 1, usb_led_count, 1, True)

if isinstance(led_size, (list, tuple)):
    base = usb_led_plane.rect(*led_size).cutBlind(-box_thickness)
else:
    base = usb_led_plane.hole(led_size, box_thickness)

if midi_sock_count == 3:
    base = base.faces(">X").workplane(centerOption="CenterOfBoundBox") \
        .center(0, board_top_shift + midi_sock_shift) \
        .hole(midi_sock_d, box_thickness) \
        .faces("<X").workplane(centerOption="CenterOfBoundBox") \
        .center(-9, board_top_shift + 6.5) \
        .hole(7, box_thickness) \

usb = cq.Workplane("YZ").rect(usb_width, usb_height) \
        .extrude(-box_thickness) \
        .edges("|X").fillet(1) \
        .translate((-box_inner_length / 2, usb_y_shift, usb_z_shift))

base = base.cut(usb)


# make the cover
cover = cq.Workplane("XY").rect(box_outer_length, box_outer_width) \
        .extrude(fillet_r + 0.1) \
        .edges("|Z").fillet(board_corner_r + board_clearance + box_thickness) \
        .faces(">Z").fillet(fillet_r) \
        .faces("<Z").shell(-box_thickness) \
        .translate((0, 0, box_thickness - fillet_r))

# cover stands
stands = cq.Workplane("XY").rect(box_inner_length + 2 * cover_stand_d1, box_inner_width + 2 * cover_stand_d1) \
        .extrude(1)

# cover rim
cover_rim = cq.Workplane("XY").rect(box_inner_length - 2 * cover_clearance, box_inner_width - 2 * cover_clearance) \
        .extrude(fillet_r) \
        .edges("|Z").fillet(board_corner_r + board_clearance - cover_clearance) \
        .faces("<Z").shell(-1) \
        .translate((0, 0, 1 - fillet_r))

for p in stand_centers:
    stands = stands.faces(">Z[-2]").workplane(centerOption="CenterOfBoundBox") \
        .center(*p) \
        .circle(cover_stand_d1 / 2) \
        .workplane(offset=cover_stand_h) \
        .circle(cover_stand_d / 2) \
        .loft(combine=True)

stands = stands.faces(">Z").workplane(centerOption="CenterOfBoundBox") \
        .pushPoints(stand_centers) \
        .hole(cover_stand_hole_d)

if (midi_sock_count == 4 and not flat) or midi_sock_count == 3:
    stands = stands.cut(
        cq.Workplane("XY").rect(94, 30 + 1).extrude(led_shift) \
        .union(
            cq.Workplane("XY").rect(76 + 1, 44).extrude(led_shift)
        ) \
        .translate((0, 0, -cover_stand_h - 1)))

i = cq.Workplane("XY") \
        .rect(box_inner_length - 2 * cover_clearance, box_inner_width - 2 * cover_clearance) \
        .extrude(-cover_stand_h) \
        .edges("|Z").fillet(board_corner_r + board_clearance - cover_clearance)

e1 = cq.Workplane("XY") \
        .center(0, edge_center_shift) \
        .circle(edge_rad + 4).circle(edge_rad + 3) \
        .extrude(-cover_edge_height)

e2 = cq.Workplane("XY") \
        .center(0, -edge_center_shift) \
        .circle(edge_rad + 4).circle(edge_rad + 3) \
        .extrude(-cover_edge_height)

stands = stands.union(e1).union(e2).intersect(i)

cover = cover.union(stands).union(cover_rim)

# move the cover up
#cover = cover.translate((0, 0, box_inner_height * 2))

cover = cover.rotate((0, 0, 0), (1, 0, 0), 180) \
    .translate((0, box_outer_width + 5, box_thickness + 0.1))

if __name__ == '__cqgi__':
    show_object(cover)
    show_object(base)
#    show_object(logo)
else:
    from cadquery import exporters

    box = cover.union(base)

    name = 'box_%dx%d' % (midi_sock_count, midi_sock_count)
    exporters.export(box, name + '.step')
    exporters.export(box, name + '.stl')
    exporters.export(box, name + '.vrml')
