# Lossless Compression

Huffman coding implementation for lossless file compression and decompression with integrity verification.

## Features

- **Huffman Encoding**: Variable-length prefix codes based on frequency
- **Lossless Compression**: Exact reconstruction of original data
- **Integrity Verification**: Byte-for-byte comparison after decompression
- **Efficient Storage**: Frequency table stored in header for reconstruction

## Algorithm

**Huffman Coding**:
1. Count character frequencies in input
2. Build min-heap priority queue
3. Construct optimal binary tree (frequent chars = shorter codes)
4. Generate prefix-free codes
5. Encode data bit-by-bit

**Compression Ratio**: Depends on data entropy (repetitive data compresses better)

## File Format

```
[MAGIC: "HUF1"] [Original Size: 8 bytes] [Frequency Table: 256×4 bytes] [Compressed Bits]
```

## Compilation

```bash
gcc -o compress compress.c -Wall -Wextra
gcc -o decompress decompress.c -Wall -Wextra
```

## Usage

### Compress

```bash
./compress input_log.txt
```

Output: `compressed.log`

### Decompress

```bash
./decompress input_log.txt
```

Reads: `compressed.log`  
Output: `decompressed.log`  
Verifies: Matches original input

## Example Session

```bash
$ ./compress sample.txt
Original size   : 1024 bytes
Compressed size : 687 bytes

$ ./decompress sample.txt
✅ Integrity check PASSED: decompressed.log matches original exactly.
```

## Data Structures

- **Min-Heap**: Priority queue for building Huffman tree (O(n log n))
- **Binary Tree**: Huffman tree for encoding/decoding
- **Bit Writer/Reader**: Efficient bit-level I/O

## Special Cases

- **Empty File**: Handled gracefully
- **Single Character**: Creates minimal tree with 1-bit codes
- **Uniform Distribution**: May result in larger file (compression overhead)

## Verification

Decompressor performs automatic integrity check:
- Compares file sizes
- Byte-for-byte comparison
- Reports PASS/FAIL status

## Use Case

Log file compression for embedded systems or storage-constrained environments requiring guaranteed lossless reconstruction.
