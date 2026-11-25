"""
Test traces shared by the reference-based tests.

Each entry is {"x": Trace, "y": Trace}. They are chosen to hit the cases that
hid bugs in the past:
- SMOOTH: straight lines, data starting at 0.
- LATE: data starting after 0, and x and y covering different time ranges.
- STEPS: piecewise-constant data, so the signals jump. The jumps are at odd
  multiples of 0.005 (like 0.605), so they never fall on a requested time
  (multiples of 0.01) nor on a window edge (windows are multiples of 0.1).
- UNEVEN: uneven time steps, short and long pieces, data starting at 0.5.

Slopes stay below about 7 so that the reference's grid error stays small.
"""

from reference import Trace

SMOOTH = {
    "x": Trace([(0, 0), (0.5, 1.5), (1.2, -0.5), (2, 2), (2.6, 1), (3.1, 2.5),
                (4, -1), (4.4, 0.3), (5, 0.3), (6, 1.2)]),
    "y": Trace([(0, 1), (0.8, -1), (1.5, 0.5), (2.5, 0.5), (3.3, -1.5), (4.2, 1), (6, 0)]),
}

LATE = {
    "x": Trace([(1.3, 0.5), (2, 2), (2.7, -1), (3.5, 1), (4.6, -0.5), (5.7, 1)]),
    "y": Trace([(0.8, -1), (1.9, 1), (3, 0.2), (4.1, 1.5), (5, -0.5), (6.2, 0.5)]),
}

STEPS = {
    "x": Trace([(0, 1), (0.605, -1), (1.205, 2), (1.805, 0.5), (2.405, -0.5), (3.005, 1.5),
                (3.605, -1), (4.205, 0.5), (4.805, 2), (6, 2)], steps=True),
    "y": Trace([(0, -1), (0.905, 1), (2.105, -0.5), (2.905, 0.5), (4.505, -1), (6, -1)],
               steps=True),
}

UNEVEN = {
    "x": Trace([(0.5, 0), (0.6, 0.4), (0.75, 0.3), (1.9, -1), (2.1, -0.3), (3.4, 1.4),
                (3.55, 0.9), (5.5, 0.5)]),
    "y": Trace([(0.5, 1), (1, 1), (1.05, 0.8), (2.95, 0.8), (3.1, 1.1), (5.5, -1)]),
}

ALL = {"smooth": SMOOTH, "late": LATE, "steps": STEPS, "uneven": UNEVEN}


def data_range(traces):
    """The earliest start and latest end over the traces."""
    return min(tr.start for tr in traces.values()), max(tr.end for tr in traces.values())
