import cadquery as cq
from Helpers import show
import FreeCAD
import FreeCADGui as Gui
import ImportGui

import sys, os

# from https://github.com/easyw/kicad-3d-models-in-freecad.git
sys.path.append("/Users/max_romanov/projects/kicad-3d-models-in-freecad/cadquery/FCAD_script_generator/_tools")

from cq_cad_tools import FuseObjs_wColors, restore_Main_Tools, z_RotateObject
import exportPartToVRML as expVRML
import shaderColors

# see https://ru.aliexpress.com/item/32820176757.html

size_x = 20.2
size_y = 12.1
size_z = 20.4

front_panel_size_y = 2.1
body_d = 16
base_height = 8

wp = cq.Workplane("XY").workplane(offset=0.0).moveTo(0.0, 0.0)

front_panel = wp.rect(size_x, front_panel_size_y, True).extrude(size_z).translate((0, front_panel_size_y / 2, 0))
body = wp.circle(body_d / 2).extrude(size_y - front_panel_size_y)
body = body.rotate((0,0,0), (1,0,0), -90).translate((0, 0, size_z / 2))

front_panel = front_panel.cut(body)

body = body.translate((0, front_panel_size_y, 0))

base = wp.rect(size_x - 2, size_y - front_panel_size_y, True).extrude(base_height).translate((0, front_panel_size_y + (size_y - front_panel_size_y) / 2, 0))
base = base.union(body)

obj2show = [
    base,
    front_panel,
]
color_names = [
    "black body",
    "metal silver",
]

for o in obj2show:
    show(o)

doc = FreeCAD.ActiveDocument
objs = doc.Objects[:]

material_subst = { }

def setColor(obj, color):
    o = Gui.ActiveDocument.getObject(obj.Name)

    o.ShapeColor = color
    o.LineColor = color
    o.PointColor = color
    o.DiffuseColor = color

    return o.DiffuseColor[0]

for i in range(0, len(objs)):
    color_name = color_names[i]
    color = shaderColors.named_colors[color_name].getDiffuseFloat()
    c = setColor(objs[i], color)

    material_subst[c[:-1]] = color_names[i]

while len(objs) > 1:
    FuseObjs_wColors(FreeCAD, Gui, doc.Name, objs[0].Name, objs[1].Name)
    del objs
    objs = doc.Objects[:]

CheckedModelName = "DIN5MIDI-OOTDTY"

doc.Label = CheckedModelName
objs[0].Label = CheckedModelName
restore_Main_Tools()

#rotate if required
#rotation = all_params[model]['rotation']
#if (rotation != 0):
#    z_RotateObject(doc, rotation)

script_dir = os.path.dirname(os.path.realpath(__file__))

file_name = script_dir + os.sep + CheckedModelName

ImportGui.export(objs, file_name + '.step')

# scale and export Vrml model
scale = 1 / 2.54

export_objects, color_keys = expVRML.determineColors(Gui, objs, material_subst)
colored_meshes = expVRML.getColoredMesh(Gui, export_objects, scale)

expVRML.writeVRMLFile(colored_meshes, file_name + '.wrl', color_keys)

# Save the doc in Native FC format
doc.saveAs(file_name + '.FCStd')

