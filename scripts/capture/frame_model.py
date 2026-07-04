"""Shared frame model for the Phase 2 capture toolkit (spec 2-S4).

Single source of protocol knowledge on the host side, mirroring
lib/rimlink (offsets stay confined to adapter-equivalents).
"""
FRAME_LEN = 33
HEADER = 0xA5

_CRC_TABLE = []


def _build_table():
    # Reflected poly 0x31 (table form 0x8C), matches lib/rimlink.
    for i in range(256):
        crc = i
        for _ in range(8):
            crc = (crc >> 1) ^ 0x8C if crc & 1 else crc >> 1
        _CRC_TABLE.append(crc)


_build_table()


def crc8(data: bytes) -> int:
    crc = 0xFF
    for b in data:
        crc = _CRC_TABLE[b ^ crc]
    return crc


def parse_frame(frame: bytes) -> dict:
    """Decode one 33-byte frame into the logical model."""
    assert len(frame) == FRAME_LEN, f"need {FRAME_LEN} bytes"
    return {
        "header": frame[0],
        "id": frame[1],
        "buttons": frame[2] | (frame[3] << 8) | (frame[4] << 16),
        "disp": list(frame[2:5]),        # base->rim meaning
        "leds": frame[5] | (frame[6] << 8),
        "rumble": list(frame[7:9]),
        "axis_x": frame[5], "axis_y": frame[6], "encoder": frame[7],
        "crc": frame[32],
        "crc_ok": crc8(frame[:32]) == frame[32],
    }
