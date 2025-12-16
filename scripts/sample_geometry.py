"""Example: basic parametric geometry built with the embedded ``aegiscad`` module."""

import math

import aegiscad


def build_box_with_fillet(edge: float, fillet_radius: float) -> aegiscad.Shape:
    """Create a filleted cube for quick visualization."""
    box = aegiscad.make_box(edge)
    return aegiscad.fillet(box, fillet_radius)


def build_cylinder_stack(radius: float, height: float) -> aegiscad.Shape:
    """Return a revolved profile joined with an extruded section."""
    base = aegiscad.make_cylinder(radius, height)
    profile = aegiscad.make_box(radius, radius)
    rotated = aegiscad.revolve(profile, (0, 0, 0), (0, 0, 1), math.pi)
    return aegiscad.extrude(rotated, height * 0.5)


if __name__ == "__main__":
    cube = build_box_with_fillet(20.0, 2.0)
    stack = build_cylinder_stack(5.0, 10.0)
    try:
        aegiscad.display(cube)
        aegiscad.display(stack)
        aegiscad.zoom_fit()
    except AttributeError:
        # Running outside the UI — rendering hooks may be unavailable.
        pass
