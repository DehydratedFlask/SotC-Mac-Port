#!/usr/bin/env python3
"""`te-freebuff`: run TokenEconomizer inside a Freebuff session.

Freebuff has no plugin or hook protocol, so the plugin cannot install itself into the agent.
This adapter is the install: the agent (or the person) calls it at the points Freebuff has no
hook for, and it speaks the plugin's own hook contract on Freebuff's behalf -- Claude Code's
payload shape, with the session's calls written into a Claude-shaped transcript the package's
folders already read.

    te-freebuff start                 SessionStart: prints the repository's learned manual
    te-freebuff prompt "text"         UserPromptSubmit: the person's prompt, one turn open
    te-freebuff run -- <command...>   runs a command and reports the result as PostToolUse
    te-freebuff say "text"            records what the agent answered (transcript only)
    te-freebuff end                   SessionEnd: folds the session, prints what was learned
    te-freebuff <command> ...         anything else goes to the plugin's own CLI
                                      (`status`, `microskills`, `toolprune`, `config`, ...)

State lives in `~/.tokeneconomizer` (or `$TOKENECONOMIZER_HOME`), per repository and per host,
exactly as for every other agent. Nothing here calls a model.
"""
import json
import os
import secrets
import subprocess
import sys
import time
from datetime import datetime, timezone
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO_ROOT = HERE.parents[1]
DEFAULT_CORE = REPO_ROOT / "local" / "tools" / "TokenEconomizer"


def core_path():
    """The TokenEconomizer checkout: `$TE_CORE_PATH`, else the private clone under `local/`."""
    cand = os.environ.get("TE_CORE_PATH") or str(DEFAULT_CORE)
    if (Path(cand) / "tokeneconomizer" / "plugin" / "hook.py").is_file():
        return str(Path(cand).resolve())
    raise SystemExit(
        "te-freebuff: TokenEconomizer is not installed.\n"
        f"  expected the package at {cand}\n"
        "  install it with: tools/tokeneconomizer/install.sh"
    )


def bootstrap():
    """Put the package on `sys.path`, link the adapter into the checkout, register the host."""
    core = core_path()
    if core not in sys.path:
        sys.path.insert(0, core)
    # The spawned `te`/fold children run with this checkout on PYTHONPATH and pick the host up
    # from `sitecustomize`; the links are what make that work, and re-making them is cheap.
    for name in ("freebuff_host.py", "sitecustomize.py"):
        link = Path(core) / name
        try:
            if link.is_symlink() or link.exists():
                continue
            link.symlink_to(HERE / name)
        except OSError:
            pass
    if str(HERE) not in sys.path:
        sys.path.insert(0, str(HERE))
    import freebuff_host

    freebuff_host.register()
    from tokeneconomizer.plugin import hook, cli

    return core, hook, cli


# ---- state -------------------------------------------------------------------------------
def state_root():
    return Path(os.environ.get("TOKENECONOMIZER_HOME") or Path.home() / ".tokeneconomizer")


def sessions_dir():
    d = state_root() / "freebuff-sessions"
    d.mkdir(parents=True, exist_ok=True)
    return d


def _pointer():
    return sessions_dir() / "current.json"


def _read_json(path, default):
    try:
        with open(path, encoding="utf-8") as fh:
            return json.load(fh)
    except (OSError, ValueError):
        return default


def _write_json(path, value):
    tmp = f"{path}.{os.getpid()}"
    with open(tmp, "w", encoding="utf-8") as fh:
        json.dump(value, fh, indent=2)
    os.replace(tmp, path)


def meta_path(sid):
    return sessions_dir() / f"{sid}.meta.json"


def current():
    """The session this directory is in, from the last `start`."""
    cwd = os.getcwd()
    sid = (_read_json(_pointer(), {}) or {}).get(cwd)
    if not sid:
        return None
    meta = _read_json(meta_path(sid), None)
    return meta if isinstance(meta, dict) else None


def _need_session():
    meta = current()
    if not meta:
        raise SystemExit(
            "te-freebuff: no session here yet. Run `te-freebuff start` first "
            "(it opens the session and prints the repository's learned manual)."
        )
    return meta


def _save(meta):
    _write_json(meta_path(meta["session_id"]), meta)
    pointers = _read_json(_pointer(), {}) or {}
    pointers[meta["cwd"]] = meta["session_id"]
    _write_json(_pointer(), pointers)


# ---- the transcript the folders read (Claude Code's shape) -------------------------------
def _append(meta, lines):
    with open(meta["transcript"], "a", encoding="utf-8") as fh:
        for line in lines:
            fh.write(json.dumps(line, ensure_ascii=False) + "\n")


def _entry(meta, kind, content):
    return {"type": kind, "sessionId": meta["session_id"], "cwd": meta["cwd"],
            "timestamp": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
            "message": {"role": kind, "content": content}}


# ---- hook plumbing -----------------------------------------------------------------------
def _hook(hook, payload):
    return hook.run(payload, dict(os.environ), host_hint="freebuff") or {}


def _payload(meta, event, **extra):
    return dict({"session_id": meta["session_id"], "cwd": meta["cwd"],
                 "transcript_path": meta["transcript"], "hook_event_name": event}, **extra)


def _context_of(out):
    hso = out.get("hookSpecificOutput")
    parts = []
    if isinstance(hso, dict) and hso.get("additionalContext"):
        parts.append(hso["additionalContext"])
    if out.get("additionalContext"):
        parts.append(out["additionalContext"])
    return "\n\n".join(parts).strip()


# ---- commands ----------------------------------------------------------------------------
def cmd_start(hook, _cli, argv):
    cwd = os.getcwd()
    sid = f"fb-{time.strftime('%Y%m%d-%H%M%S')}-{secrets.token_hex(3)}"
    path = sessions_dir() / f"{sid}.jsonl"
    path.touch()
    meta = {"session_id": sid, "transcript": str(path), "cwd": cwd,
            "started": time.time(), "prompts": 0, "calls": 0}
    _save(meta)
    out = _hook(hook, _payload(meta, "SessionStart", source="startup"))
    text, notice = _context_of(out), out.get("systemMessage") or ""
    if text:
        print(text)
    if notice:
        print(f"\n[{notice}]", file=sys.stderr)
    print(f"\n[TokenEconomizer/freebuff: session {sid} in {cwd}]", file=sys.stderr)
    return 0


def cmd_prompt(hook, _cli, argv):
    if not argv:
        raise SystemExit("usage: te-freebuff prompt \"the person's prompt\"")
    meta = _need_session()
    text = " ".join(argv)
    _append(meta, [_entry(meta, "user", [{"type": "text", "text": text}])])
    meta["prompts"] = int(meta.get("prompts") or 0) + 1
    _save(meta)
    out = _hook(hook, _payload(meta, "UserPromptSubmit",
                               prompt_id=f"{meta['session_id']}:{meta['prompts']}", prompt=text))
    extra, notice = _context_of(out), out.get("systemMessage") or ""
    if extra:
        print(extra)
    if notice:
        print(f"[{notice}]", file=sys.stderr)
    return 0


def cmd_run(hook, _cli, argv):
    """Run a command, report it as a tool call, and pass its output and status through."""
    if argv and argv[0] == "--":
        argv = argv[1:]
    if not argv:
        raise SystemExit("usage: te-freebuff run -- <command...>")
    meta = _need_session()
    command = " ".join(argv)
    proc = subprocess.run(command, shell=True, executable="/bin/bash", stdout=subprocess.PIPE,
                          stderr=subprocess.STDOUT, text=True, errors="replace")
    text, code = proc.stdout or "", proc.returncode
    meta["calls"] = int(meta.get("calls") or 0) + 1
    call_id = f"call-{meta['calls']}"
    _append(meta, [
        _entry(meta, "assistant", [{"type": "tool_use", "id": call_id, "name": "Bash",
                                    "input": {"command": command}}]),
        _entry(meta, "user", [{"type": "tool_result", "tool_use_id": call_id,
                               "content": text, "is_error": code != 0}]),
    ])
    _save(meta)
    out = _hook(hook, _payload(meta, "PostToolUse",
                               prompt_id=f"{meta['session_id']}:{meta['prompts']}",
                               tool_name="Bash", tool_use_id=call_id,
                               tool_input={"command": command},
                               tool_response={"content": text, "exit_code": code,
                                              "isError": code != 0}))
    sys.stdout.write(text)
    redirect = _context_of(out)
    if redirect:
        print(f"\n[{redirect}]", file=sys.stderr)
    notice = out.get("systemMessage") or ""
    if notice:
        print(f"[{notice}]", file=sys.stderr)
    return code


def cmd_say(_hook, _cli, argv):
    if not argv:
        return 0
    meta = _need_session()
    _append(meta, [_entry(meta, "assistant", [{"type": "text", "text": " ".join(argv)}])])
    return 0


def cmd_end(hook, _cli, _argv):
    """Close the session and fold it now, so what was learned is learned before the next start."""
    meta = _need_session()
    from tokeneconomizer.plugin import fold, hosts
    from tokeneconomizer.plugin.context import LiveContext

    host = hosts.HOSTS["freebuff"]()
    ctx = LiveContext.build(host, meta["cwd"], dict(os.environ),
                            session_id=meta["session_id"])
    _hook(hook, _payload(meta, "SessionEnd", reason="exit"))
    try:
        record = fold.fold_session(ctx, meta["session_id"], transcript_path=meta["transcript"])
    except Exception as exc:  # a fold failure must not look like a session failure
        print(f"[TokenEconomizer/freebuff: fold failed: {exc}]", file=sys.stderr)
        record = None
    prompts = meta.get("prompts") or 0
    calls = meta.get("calls") or 0
    status = "folded" if record else "not folded"
    print(f"[TokenEconomizer/freebuff: session {meta['session_id']} ended ({status}, "
          f"{prompts} prompt(s), {calls} call(s)); see `te-freebuff status`]", file=sys.stderr)
    return 0


def cmd_passthrough(_hook, cli, argv):
    """Everything else is the plugin's own command line, for this host and repository."""
    args = list(argv)
    if "--host" not in args:
        args += ["--host", "freebuff"]
    if "--cwd" not in args:
        args += ["--cwd", current().get("cwd") if current() else os.getcwd()]
    return cli.main(args)


COMMANDS = {"start": cmd_start, "prompt": cmd_prompt, "run": cmd_run, "say": cmd_say,
            "end": cmd_end}


def main(argv=None):
    argv = sys.argv[1:] if argv is None else argv
    if not argv or argv[0] in ("-h", "--help", "help"):
        print(__doc__)
        return 0 if argv else 2
    core, hook, cli = bootstrap()
    os.environ.setdefault("PYTHONPATH", core)
    name, rest = argv[0], argv[1:]
    handler = COMMANDS.get(name)
    return handler(hook, cli, rest) if handler else cmd_passthrough(hook, cli, argv)


if __name__ == "__main__":
    sys.exit(main())
