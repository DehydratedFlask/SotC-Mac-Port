"""Register TokenEconomizer's `freebuff` host in every Python that runs on this PYTHONPATH.

The plugin spawns its own detached work -- the fold after a session, the repository map -- as
`python -m tokeneconomizer.plugin.cli ... --host freebuff`. Those children never see the
adapter's `register()` call, and the package's command line only accepts the hosts it knows
about. Python imports `sitecustomize` at interpreter start-up from `PYTHONPATH`, which the
spawned child inherits, so this file registers the host there.

It is linked into the TokenEconomizer checkout's root (which is what that PYTHONPATH is), is
inert when the package is absent, and never raises: a broken import here must not change how a
child process starts.
"""
import os
import sys

try:
    import freebuff_host

    here = os.path.dirname(os.path.abspath(__file__))
    if here not in sys.path:
        sys.path.insert(0, here)
    freebuff_host.register()
except Exception:
    pass
