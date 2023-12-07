from __future__ import annotations


def crc16_step(byte: int, last: int = 0xFFFF) -> int:
    last = last & 0xFFFF

    x = (last >> 8 ^ (byte & 0xFF)) & 0xFF
    x ^= x >> 4
    last = (last << 8) ^ (x << 12) ^ (x << 5) ^ x

    return last & 0xFFFF


def crc16_quick(data: bytes) -> int:
    crc = 0xFFFF

    for byte in data:
        crc = crc16_step(byte, crc)

    return crc & 0xFFFF


if __name__ == '__main__':
    print(crc16_quick(b'\x00\x00\x01').to_bytes(2, 'little', signed=False).hex().upper())
    print(crc16_quick(b'\x00\x00\x00').to_bytes(2, 'little', signed=False).hex().upper())
    print(crc16_quick(b'\x01\x00\x00').to_bytes(2, 'little', signed=False).hex().upper())
    print(crc16_quick(b'\x00\x00\x00\x01').to_bytes(2, 'little', signed=False).hex().upper())
    print(crc16_quick(b'\x00\x01').to_bytes(2, 'little', signed=False).hex().upper())

    crc_expected = 0xBDDC
    crc_computed = crc16_quick(b'\x00\x00\x01').to_bytes(2, "little", signed=False)
    print(f'CRC Received {crc_computed.hex().upper()} / CRC Computed {hex(crc_expected)[2:].upper()}')
