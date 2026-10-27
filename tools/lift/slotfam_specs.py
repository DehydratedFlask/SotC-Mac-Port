"""Decoded per-slot facts of the L3 startup constructor slots 322, 325, 327, 328, 330, 331, 332, 333
and 334 (original ELF sha256 d0723afb...5861d3).

Every number here was read from the pinned bytes by tools/lift/slotfam_interp.py (a symbolic walk of
the straight-line body that only RECORDS calls) and is re-derived and compared by
tools/lift/slotfam_check.py on every checker run. This file is the single source for the checkers and
for the native-module generator; the native C++ constants are compared against it textually.
Nothing here loads or executes an original RVA.
"""

from __future__ import annotations

# step kinds: var (bool console variable), toggle (0x26ea0), named (0x26db0), group (0x26c10)
def var(ident, obj, name, name_rva, desc_rva, fn_tag, value, default):
    return dict(kind="var", id=ident, obj=obj, name=name, name_rva=name_rva, desc_rva=desc_rva,
                fn_tag=fn_tag, value=value, default=default)


def toggle(ident, obj, target, label, label_rva, text, text_rva, flags=0x30):
    return dict(kind="toggle", id=ident, obj=obj, target=target, label=label, label_rva=label_rva,
                text=text, text_rva=text_rva, flags=flags)


def named(ident, obj, target, parent, parent_rva, flags=0x30):
    return dict(kind="named", id=ident, obj=obj, target=target, parent=parent, parent_rva=parent_rva,
                flags=flags)


def group(ident, obj, parent, parent_rva, caption, caption_rva, flags=0x30):
    return dict(kind="group", id=ident, obj=obj, parent=parent, parent_rva=parent_rva, caption=caption,
                caption_rva=caption_rva, flags=flags)


BOOL_SLOTS = {
    322: dict(rva=0x849E30, size=386,
              sha="13a487fd93b1e690f6dd051238a273727b903c5965fe066447d35c5abd00a81d",
              steps=[var("var", 0x3294D20, "shadow_autoEquipNewWeapons", 0x128EDE0, 0x128EE13, 0x15CEC90, 1, 1),
                     named("item", 0x3294DE8, "var", "Shadow Items", 0x128EDFB)]),
    325: dict(rva=0x839F00, size=455,
              sha="2335dcff52ec21e00f2c734d480cab9a2294bf1edf70e64de2055adc0935ddb0",
              steps=[var("var", 0x3293288, "shadow_debugHints", 0x128E3F8, 0x128E47A, 0x15CEC20, 0, 0),
                     toggle("item", 0x3293350, "var", "Colossus", 0x128E40A, "Hint debug", 0x128E413)]),
    327: dict(rva=0x836330, size=455,
              sha="b6848e14b5cb880806379ae0aa0e24695acf18f8f2ff00bc8fa99d5e92575af7",
              steps=[var("var", 0x3291608, "colossusCanSeePlayer", 0x128E080, 0x128E0B8, 0x15CEBC0, 1, 1),
                     toggle("item", 0x32916D0, "var", "Colossus", 0x128E095, "Can See Player", 0x128E09E)]),
    328: dict(rva=0x82E040, size=519,
              sha="790ce9bbe5df739f0d6851012dd50fcc9f42be60a81a7277bdd980e978b20821",
              steps=[var("var_a", 0x3291470, "shadow_showBossScrModeStatus", 0x128DE60, 0x128DEC9, 0x15CEBA0, 0, 0),
                     var("var_b", 0x3291538, "shadow_showBossMotKind", 0x128DE7D, 0x128DEC9, 0x15CEBA0, 0, 0)]),
    330: dict(rva=0x810A50, size=299,
              sha="d7276191d498d8895e9d411761d9fcc6c34c739e26a2c8e788e4ab6a8121a09a",
              steps=[var("var", 0x3288CD0, "shadow_debugDrawHorsePassageDir", 0x128CEF0, 0x128CF1B, 0x15CE920, 0, 0)]),
    331: dict(rva=0x809640, size=512,
              sha="2185d9ddea36b25c86373424abc1ec9e6e24115f96ed07d393bd42448bdbf7f9",
              steps=[var("var", 0x3288A98, "CP10_DebugWormIk", 0x128CBE0, 0x128CC10, 0x15CE900, 0, 0),
                     group("group", 0x3288B60, "CP10-Anim", 0x128CBF1, "Shadow IK", 0x128CBFB),
                     named("item", 0x3288C08, "var", "Shadow IK", 0x128CBFB)]),
    332: dict(rva=0x805440, size=694,
              sha="85c8ac1d5ff0ced126ea3bc23971124f64e71820c6278756c9dbe33cf234a735",
              steps=[var("var_a", 0x32887B0, "shadow_gazeIkEnableSelectiveRecomputeGlobal", 0x128CA40, 0x128CA9D,
                         0x15CE8E0, 1, 1),
                     named("item_a", 0x3288878, "var_a", "Shadow IK", 0x128CA6C),
                     var("var_b", 0x3288920, "cp10_enableGazeIk", 0x128CA76, 0x128CA9D, 0x15CE8E0, 1, 1),
                     named("item_b", 0x32889E8, "var_b", "CP10-Anim", 0x128CA88)]),
}

# Slot 333: two console commands (the shape of core/fconcmd_object_slot306).
CMD_SLOT = dict(
    rva=0x7FD6D0, size=550,
    sha="1003425f9de4127d77906be8aca9d628eb04e3e87feda2a6776d4c767255a71e",
    cmds=[
        dict(obj=0x3288640, name="shadow_timeAttackSetCompletionCount", name_rva=0x128C588,
             desc="Sets how many colossi have been defeated in time attack mode. "
                  "+shadow_timeAttackSetCompletionCount <number completed>",
             desc_rva=0x128C5AC, handler=0x7FD4D0),
        dict(obj=0x32886E0, name="shadow_timeAttackSetRetroactiveCompletionCount", name_rva=0x128C622,
             desc="Sets how many colossi have been defeated in time attack mode and retroactively unlocks the "
                  "time attack rewards. +shadow_timeAttackSetRetroactiveCompletionCount <number completed>",
             desc_rva=0x128C651, handler=0x7FD590),
    ])

# Slot 334: an unbounded int variable through the 0x3a980 worker plus a 0x26ea0 toggle.
INT_SLOT = dict(
    rva=0x7FC530, size=481,
    sha="969702b27cf20d859d5a68c19a085c38a35ed925cdcd7e749c858447e4629dc9",
    var=0x32882F8, name="gMaxSimaltaneousAreaLoadRequests", name_rva=0x128C1C0, desc_rva=0x128C2F4,
    arg_fn_tag=0x15CE8C0, flags=1, value=0x10, default=0x10, dtor=0x18A60,
    item=0x32883D0, label="Shadow Misc", label_rva=0x128C1E1,
    text="MaxSimaltaneousAreaLoadRequests", text_rva=0x128C1ED, item_flags=0x30)

# sha256 of the complete recorded symbolic walk of each body (tools/lift/slotfam_interp.py trace_digest):
# executed pcs, every store with its address/size/value (stack cells included), every call with its
# register and stack-snapshot arguments, and every conditional/unconditional branch with its outcome.
# This pins instructions the semantic event list does not mention (stack-view zeroing, length immediates,
# canary placement, branch conditions) independently of the whole-body hash.
TRACE_SHA256 = {
    322: "4571cfa1ab63278e9ca11c2e09b157f2cc88afbec1249c778d1c45f5a36f5d70",
    325: "570ecdb6290b862dd1fba99a0a436c1de6c56606acf49d6bcbc4ff53abbefa9b",
    327: "eddfab29b0ac2c40f1856286961bd8779fa79d58d11d6bd2582688988813b81e",
    328: "150dc8d0d21ae98a7e6a2ab074ea40c580191501d93bb9c01cb83104920fb81b",
    330: "1da74e3bf8ffb449284109c88c76ba90b159bc0207420b8d6d9a6c99fb68bb38",
    331: "cceaa84c303541edb996b289c8381259c88579a468eea58b965979de634b20fc",
    332: "75a659362f914a3e044fb048b1273b5b759b51569c4fa80b6f83b1f0dad4b9bd",
    333: "88252b684f620055c62b2f4c64d33346cb5a146fc6771fd051ed78c0ea2d019e",
    334: "f3f83eec692a11fa661ec1288f9d3d8e0fc6e13a407969ca12f028fc7adec123",
}
