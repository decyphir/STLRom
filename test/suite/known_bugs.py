"""
Known bugs: one place that lists every bug the suite knows about.

Tests that hit one of these are marked `xfail(strict=True)` with the reason
below. They run on every test run and show up as "xfailed" (x) in the report:
- while the bug is there, they fail as expected, and the run stays green;
- once the bug is fixed, they pass, and strict=True turns that into a FAILURE
  ("XPASS(strict)"), as a reminder to remove the mark here.

Each bug has a small stand-alone test in test_known_bugs.py.

There are no open bugs right now: add one here as NAME = ("explanation"), and
mark its tests with @known_bugs.xfail(known_bugs.NAME).
"""

import pytest


def xfail(reason, run=True):
    """The mark for a known bug. run=False for bugs that crash the process."""
    return pytest.mark.xfail(strict=True, reason=reason, run=run)
