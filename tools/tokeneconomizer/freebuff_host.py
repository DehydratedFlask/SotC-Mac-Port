"""A TokenEconomizer host adapter for Freebuff.

Freebuff exposes no plugin/hook protocol, so nothing can call TokenEconomizer on its own. This
adapter does two things instead: it registers a `freebuff` host so the package's dispatcher and
folders run unmodified under a host they know, and it feeds them the payload shape they already
read -- Claude Code's hook JSON, with the session's tool calls written into a Claude-shaped
transcript (`tools/tokeneconomizer/te_freebuff.py` writes it).

What Freebuff can and cannot give the mechanisms:

  * micro-skills (envfacts, no model call) -- full: the manual is printed at session start and
    read by the agent, and each session's calls are folded afterwards;
  * the repository map -- full, the same text channel;
  * the loop guard -- only for commands run through `te-freebuff run`, which is where it can see
    a tool result; its judge needs a host CLI, so `loopguard` is left off in this repository;
  * tool pruning -- not available: Freebuff's tool set is not configurable from a repository.
"""


class Freebuff:
    """Built lazily so this module can be imported before the package is on `sys.path`."""

    @staticmethod
    def build():
        from tokeneconomizer.plugin import hosts
        from tokeneconomizer import trace

        class Host(hosts.Host):
            name = "freebuff"
            # The agent reads what a hook returns: `te-freebuff start` prints it.
            prompt_context = True
            # A Freebuff session does not replay an earlier start's context, so every start
            # injects the manual again.
            keeps_start_context = False
            # Freebuff shows the agent's messages, so a notice goes through the agent.
            relays_notices = True
            interrupt = "Interrupt the agent to stop it."
            end_run = "Interrupt the agent to end the run."

            def transcript(self, p):
                return (p or {}).get("transcript_path") or ""

            def is_subagent(self, p):
                return False

            def headless(self, env, transcript=None):
                return False

            def session_usage(self, transcript_path):
                # The adapter records calls, prompts and text, not token counts: Freebuff does
                # not expose a per-session usage log. Zeros keep the accounting honest.
                usage = trace.claude_session_usage(transcript_path) if transcript_path else {}
                usage.setdefault("requests", 0)
                return usage

            def prompt_count(self, transcript_path):
                return trace.claude_prompt_count(transcript_path) if transcript_path else 0

            def tool_usage(self, trace_path):
                return trace.claude_tool_usage(trace_path) if trace_path else {}

            def history(self, env, extra_dirs=()):
                # No earlier Freebuff sessions to import: the adapter only sees sessions it ran.
                return []

        return Host


def register():
    """Add `freebuff` to `hosts.HOSTS`; idempotent, and a no-op without the package."""
    from tokeneconomizer.plugin import hosts

    if "freebuff" not in hosts.HOSTS:
        hosts.HOSTS["freebuff"] = Freebuff.build()
    return hosts.HOSTS["freebuff"]


if __name__ == "__main__":  # a quick check that the package is importable here
    from tokeneconomizer.plugin import hosts
    register()
    print("registered:", "freebuff" in hosts.HOSTS, "->", hosts.HOSTS["freebuff"].__mro__[1].__name__)
