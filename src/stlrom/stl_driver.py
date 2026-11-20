"""Signal access and plotting helpers, attached to STLDriver and STLMonitor in ``stlrom/__init__.py``."""
from __future__ import annotations

from typing import TYPE_CHECKING, List, Optional, Union

from ._stlrom import Signal
from .signal import plot

if TYPE_CHECKING:
    import matplotlib.axes

__all__ = ["Signal", "plot", "get_signal_idx", "get_signame_from_idx", "get_signal", "plot_signal", "plot_signals"]


def get_signal_idx(self, sig: str) -> int:
    """Index of a declared signal, or -1 if there is no signal with this name."""
    return self.data.get_signal_idx(sig)

def get_signame_from_idx(self, idx: int) -> str:
    """Name of the signal at this index."""
    return self.data.get_signame_from_idx(idx)

def get_signal(self, sig: Union[str, int]) -> Signal:
    """A copy of one data signal, by name or index."""
    return self.data.get_signal(sig)

def plot_signal(self, sig: Union[str, int], label: Optional[str] = None, ax: Optional[matplotlib.axes.Axes] = None,
                draw_samples: bool = False) -> matplotlib.axes.Axes:
    """Plot one data signal (by name or index); returns the axes drawn on."""
    S = self.get_signal(sig)

    if S is not None and label is None:
        label = str(sig) #FIXME: dummy if sig is integer
    return plot(S, label=label, ax=ax, draw_samples=draw_samples)

def plot_signals(self, sig_names: Optional[List[str]] = None, same_ax: bool = True, ax: Optional[matplotlib.axes.Axes] = None,
                 draw_samples: bool = False) -> Optional[matplotlib.axes.Axes]:
    """Plot several data signals (all of them by default).

    Args:
        sig_names: names to plot; all declared signals if None.
        same_ax: draw them on one axes; otherwise one figure each.
        ax: axes to draw on; a new figure is created if None.
        draw_samples: mark the samples.

    Returns:
        The axes of the first signal, or ``ax`` if there is nothing to plot.
    """
    if sig_names is None:
        sig_names= list(self.data.signal_map.keys())

    if len(sig_names)>0:
        if ax is None:
            ax = self.plot_signal(sig_names[0], draw_samples=draw_samples)
        else:
            self.plot_signal(sig_names[0], ax=ax, draw_samples=draw_samples)
        if same_ax:
            for sig in sig_names[1:]:
                self.plot_signal(sig, ax=ax,draw_samples=draw_samples)
        else:
            for sig in sig_names[1:]:
                self.plot_signal(sig, draw_samples=draw_samples)
    return ax


