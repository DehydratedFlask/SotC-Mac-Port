#!/usr/bin/env python3
"""Fail if a commit credits Claude/Anthropic or leaks a hostname-derived identity.

The project owner is the only contributor. This gate inspects the effective git
identity and the author, committer and message of commits that are about to be
published, and rejects:

  * author/committer names or e-mails that name an assistant or agent;
  * assistant credit trailers (``Co-Authored-By: Claude ...``);
  * "Generated with [Claude Code]" style signatures and robot sign-offs;
  * ``.local`` / ``localhost`` e-mail addresses, which git writes when
    ``user.email`` is unset and it falls back to ``login@hostname``.

Usage:
  python3 -I scripts/check_commit_attribution.py             # identity + commits not on upstream
  python3 -I scripts/check_commit_attribution.py --all       # every commit reachable from HEAD
  python3 -I scripts/check_commit_attribution.py --range A..B
  python3 -I scripts/check_commit_attribution.py --identity  # effective identity only

Exit 0 means clean; exit 1 lists every violation. Policy: docs/ATTRIBUTION.md.
"""

from __future__ import annotations

import argparse
import re
import socket
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

FIELD = "\x1f"
RECORD = "\x1e"

# Names/aliases no commit may be authored, committed, or co-authored by.
ASSISTANT_RE = re.compile(
    r"(?i)(?:^|[^a-z0-9])"
    r"(claude|anthropic|sonnet|opus|haiku|codex|opencode|cursor[- ]agent|"
    r"orchestrator|subagent|assistant|chatgpt|openai|deepseek)"
    r"(?:[^a-z0-9]|$)"
)

# Trailer keys that credit another party.
TRAILER_RE = re.compile(r"(?im)^\s*([a-z][a-z-]*)\s*:\s*(.+?)\s*$")
CREDIT_KEYS = frozenset(
    {
        "co-authored-by",
        "co-author",
        "co-developed-by",
        "contributed-by",
        "generated-with",
        "generated-by",
    }
)

SIGNATURE_RE = re.compile(r"(?im)^\s*(?:🤖\s*)?generated (?:with|by)\b")
ROBOT_RE = re.compile(r"(?m)^\s*🤖")

# git's `login@hostname` fallback and explicit `.local` / `localhost` domains.
HOSTNAME_EMAIL_RE = re.compile(r"(?i)@(?:[^@\s<>]*\.)?local$|@localhost$")


@dataclass(frozen=True)
class Commit:
    sha: str
    author_name: str
    author_email: str
    committer_name: str
    committer_email: str
    message: str


def _host_matches(email: str, hostname: str) -> bool:
    """True when the e-mail domain is this machine's (short) hostname."""
    if "@" not in email:
        return False
    domain = email.rsplit("@", 1)[1].lower()
    host = hostname.lower()
    short = host.split(".")[0]
    return bool(domain) and domain in {host, short, f"{short}.local"}


def identity_problems(
    name: str, email: str, hostname: str | None = None
) -> list[str]:
    """Reject an author/committer identity that is not the human owner."""
    hostname = hostname or socket.gethostname()
    problems: list[str] = []
    if not email:
        problems.append(
            "identity e-mail is empty; git would fall back to <login>@<hostname>"
        )
    if name and ASSISTANT_RE.search(name):
        problems.append(f"identity name is an assistant/agent alias: {name!r}")
    if email:
        if ASSISTANT_RE.search(email):
            problems.append(
                f"identity e-mail is an assistant/agent address: {email!r}"
            )
        elif HOSTNAME_EMAIL_RE.search(email) or _host_matches(email, hostname):
            problems.append(f"identity e-mail is host-derived: {email!r}")
    return problems


def message_problems(message: str) -> list[str]:
    """Reject assistant credit trailers and generated-by signatures."""
    problems: list[str] = []
    for line in message.splitlines():
        trailer = TRAILER_RE.match(line)
        if (
            trailer
            and trailer.group(1).lower() in CREDIT_KEYS
            and ASSISTANT_RE.search(trailer.group(2))
        ):
            problems.append(f"assistant credit trailer: {line.strip()!r}")
        elif SIGNATURE_RE.match(line) and ASSISTANT_RE.search(line):
            problems.append(f"assistant signature: {line.strip()!r}")
        elif ROBOT_RE.match(line):
            problems.append(f"robot sign-off: {line.strip()!r}")
    return problems


def commit_problems(commit: Commit) -> list[str]:
    problems = [
        f"author: {p}"
        for p in identity_problems(commit.author_name, commit.author_email)
    ]
    problems += [
        f"committer: {p}"
        for p in identity_problems(commit.committer_name, commit.committer_email)
    ]
    problems += message_problems(commit.message)
    return problems


def parse_log(text: str) -> list[Commit]:
    """Parse ``%H..%B`` records separated by RECORD and fields by FIELD."""
    commits: list[Commit] = []
    for record in text.split(RECORD):
        if not record.strip():
            continue
        parts = record.strip("\n").split(FIELD)
        if len(parts) < 6:
            raise ValueError(f"unexpected git log record: {record[:80]!r}")
        sha, author_name, author_email, committer_name, committer_email = parts[:5]
        commits.append(
            Commit(
                sha=sha.strip(),
                author_name=author_name,
                author_email=author_email,
                committer_name=committer_name,
                committer_email=committer_email,
                message=FIELD.join(parts[5:]),
            )
        )
    return commits


def git(*args: str) -> str:
    return subprocess.check_output(
        ["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL
    )


def git_or(*args: str) -> str:
    try:
        return git(*args).strip()
    except (subprocess.CalledProcessError, FileNotFoundError):
        return ""


def revision_scope(all_commits: bool, range_: str | None) -> str:
    if range_:
        return range_
    if all_commits:
        return "HEAD"
    upstream = git_or(
        "rev-parse", "--abbrev-ref", "--symbolic-full-name", "@{u}"
    )
    return f"{upstream}..HEAD" if upstream else "HEAD"


def load_commits(revspec: str) -> list[Commit]:
    out = git(
        "log",
        "--no-color",
        f"--format=%H{FIELD}%an{FIELD}%ae{FIELD}%cn{FIELD}%ce{FIELD}%B{RECORD}",
        revspec,
    )
    return parse_log(out)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument(
        "--all", action="store_true", help="audit every commit reachable from HEAD"
    )
    ap.add_argument(
        "--range", dest="range_", metavar="A..B", help="audit an explicit revision range"
    )
    ap.add_argument(
        "--identity", action="store_true", help="audit only the effective git identity"
    )
    args = ap.parse_args()

    failures: list[str] = []
    if not args.range_:
        name = git_or("config", "user.name")
        email = git_or("config", "user.email")
        failures += [f"identity: {p}" for p in identity_problems(name, email)]

    if args.identity:
        scope = "(identity only)"
    else:
        scope = revision_scope(args.all, args.range_)
        try:
            commits = load_commits(scope)
        except (subprocess.CalledProcessError, ValueError) as exc:
            print(
                f"check_commit_attribution: cannot read {scope}: {exc}",
                file=sys.stderr,
            )
            return 2
        for commit in commits:
            failures += [
                f"{commit.sha[:12]} {p}" for p in commit_problems(commit)
            ]

    if failures:
        print("ATTRIBUTION AUDIT FAILED:", file=sys.stderr)
        for line in failures:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print(f"ATTRIBUTION AUDIT OK ({scope})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
