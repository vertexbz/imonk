from __future__ import annotations

def crc16_quick(data: bytes) -> int:
    crc = 0xFFFF
    for byte in data:
        x = (crc >> 8 ^ byte) & 0xFF
        x ^= x >> 4
        crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x

    return crc & 0xFFFF


if __name__ == '__main__':
    crc_expected = 0xBDDC
    crc_computed = crc16_quick(b'\x00\x00\x01')
    print(f'CRC Received {hex(crc_computed)[2:].upper()} / CRC Computed {hex(crc_expected)[2:].upper()}')
