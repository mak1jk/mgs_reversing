#include "hind.h"

// s11d Hind module - helicopter actor support functions
// Extracted from MIPS assembly in asm/overlays/s11d/

// External function declarations
extern void func_800329C4(void *, int, int);
extern int GV_Time;

// ============================================================================
// FUNCTION 1: s11d_hind_800C97B8 - Field update with masking (56 bytes)
// ============================================================================
// Pattern: Rotation accumulator - updates two fields in a sub-structure
// by adding values from global effect fields and masking to 12-bit range.
void s11d_hind_800C97B8(void *a0)
{
    unsigned char *base;
    unsigned char *sub;
    short val1;
    short effect1;
    short result1;
    short val2;
    short effect2;
    short result2;

    base = (unsigned char *)a0;
    sub = base + 156;

    // First field update: offset 46 within sub-structure
    val1 = *(short *)(sub + 46);
    effect1 = *(short *)(base + 500);
    result1 = (val1 + effect1) & 0xFFF;
    *(short *)(sub + 46) = result1;

    // Second field update: offset 52 within sub-structure
    val2 = *(short *)(sub + 52);
    effect2 = *(short *)(base + 502);
    result2 = (val2 + effect2) & 0xFFF;
    *(short *)(sub + 52) = result2;
}

// ============================================================================
// FUNCTION 4: s11d_hind_800C976C - Range clamp with state validation (76 bytes)
// ============================================================================
// Pattern: Attraction mechanics - constrains a 16-bit value to be within
// a certain range of a target value, returning collision detection boolean.
int s11d_hind_800C976C(short *a0, int a1, int a2)
{
    short signed_val;
    unsigned short unsigned_val;
    int difference;
    int neg_bound;
    short result;
    short readback;
    int xor_result;

    // Load current value in both signed and unsigned forms
    signed_val = *a0;
    unsigned_val = *(unsigned short *)a0;

    // Compute the difference and bounds
    difference = a1 - signed_val;
    neg_bound = -a2;

    // Compute result based on three-way branch
    if (difference < neg_bound) {
        // Value is too negative - pull up
        result = (short)(unsigned_val + a2);
    } else if (a2 < difference) {
        // Value is too positive - pull down
        result = (short)(unsigned_val + a2);
    } else {
        // Value is within range - move toward target
        result = (short)(unsigned_val + difference);
    }

    // Store the computed result
    *a0 = result;

    // Read back and check for collision/match
    readback = *a0;
    xor_result = readback ^ a1;

    // Return 1 if collision detected (xor < 1), else 0
    return (xor_result < 1) ? 1 : 0;
}

// ============================================================================
// FUNCTION 2: s11d_hind_800C9838 - Event dispatcher (53 bytes)
// ============================================================================
// Pattern: Periodic event trigger - calls external dispatcher every 4 frames
// when (GV_Time & 3) == 0
void s11d_hind_800C9838(void *a0)
{
    int gv_time;

    // Load current game time
    gv_time = GV_Time;

    // Check if we're on a frame divisible by 4 (every 4 frames)
    if ((gv_time & 3) == 0) {
        // Trigger event: dispatch with event code 0xB6
        func_800329C4((char *)a0 + 0x20, 0xB6, 1);
    }
}

// ============================================================================
// FUNCTION 3: s11d_hind_800C97F0 - Timer with event trigger (44 bytes)
// ============================================================================
// Pattern: Count-down timer - decrements counter and triggers event when zero
int s11d_hind_800C97F0(void *a0)
{
    unsigned char *base;
    short timer_value;
    int shift_result;

    base = (unsigned char *)a0;

    // Load timer at offset 0x978 (as halfword)
    timer_value = *(short *)(base + 0x978);

    // Check if timer is already at 0
    if (timer_value <= 0) {
        return 0;
    }

    // Decrement timer
    timer_value--;
    *(short *)(base + 0x978) = timer_value;

    // Shift left by 14 bits and check if result >= 0
    shift_result = timer_value << 14;
    if (shift_result >= 0) {
        return 0;
    }

    // Trigger event when timer hits 0
    func_800329C4((char *)a0 + 0x20, 0xB6, 1);

    return 0;
}

// ============================================================================
// FUNCTION 5: s11d_hind_800CA49C - Conditional flag setter (31 instructions)
// ============================================================================
// Pattern: Sets/clears bit flag based on position thresholds
void s11d_hind_800CA49C(void *a0)
{
    unsigned char *base;
    short val1;
    short val2;
    short val3;
    unsigned int flags;
    unsigned int result;

    base = (unsigned char *)a0;

    // Load first comparison value at offset 0x24
    val1 = *(short *)(base + 0x24);

    // Check if val1 < 0xC950 (51536) - interpret as signed comparison
    if ((unsigned short)val1 >= 0xC950U) {
        // Condition fails - clear bit 0x80 in field at offset 0x28
        flags = *(unsigned int *)(base + 0x28);
        result = flags & 0xFF7F;  // Clear bit 7
        *(unsigned int *)(base + 0x28) = result;
        return;
    }

    // Load second comparison value at offset 0x20
    val2 = *(short *)(base + 0x20);

    // Check if val2 < 0xF061 (61537) - interpret as signed comparison
    if ((unsigned short)val2 >= 0xF061U) {
        // Condition fails - clear bit 0x80
        flags = *(unsigned int *)(base + 0x28);
        result = flags & 0xFF7F;
        *(unsigned int *)(base + 0x28) = result;
        return;
    }

    // Check third threshold value
    val3 = *(short *)(base + 0x20);
    if ((unsigned short)val3 >= 0xBB8U) {  // 3000 in decimal
        // Condition fails - clear bit 0x80
        flags = *(unsigned int *)(base + 0x28);
        result = flags & 0xFF7F;
        *(unsigned int *)(base + 0x28) = result;
        return;
    }

    // All conditions passed - set bit 0x80 in field at offset 0x28
    flags = *(unsigned int *)(base + 0x28);
    result = flags | 0x80;
    *(unsigned int *)(base + 0x28) = result;
}

// ============================================================================
// FUNCTION 6: s11d_hind_800CAFEC - Array copy loop (31 instructions)
// ============================================================================
// Pattern: Memory copy operation - copies data from source to destination
void s11d_hind_800CAFEC(void *a0, void *a1)
{
    unsigned short *src;
    unsigned int *src_dword;
    unsigned int *dest;
    unsigned short count;
    unsigned int dword_count;
    int i;

    src = (unsigned short *)a0;
    dest = (unsigned int *)a1;

    // Load count from first halfword of source
    count = *src;
    dword_count = count & 0xFFFF;

    // Load dword count
    src_dword = (unsigned int *)(src + 2);
    dword_count = *src_dword;

    // Decrement main counter
    count--;

    // Exit early if count becomes zero
    if (count == 0) {
        return;
    }

    // Copy loop - process 4 bytes at a time
    src = (unsigned short *)((unsigned char *)src + 6);

    for (i = 0; i < dword_count; i++) {
        unsigned short val1, val2;

        // Read two halfwords
        val1 = *src;
        src++;
        val2 = *src;
        src++;

        // Write as dword
        *dest = ((unsigned int)val1) | (((unsigned int)val2) << 16);
        dest++;
    }

    // Copy remaining halfwords (2 bytes at a time)
    src = (unsigned short *)((unsigned char *)src - 2);
    count--;

    // Copy final elements
    while (count > 0) {
        unsigned short val = *src;
        src++;
        *((unsigned short *)dest) = val;
        count--;
        dest = (unsigned int *)((unsigned char *)dest + 2);
    }
}

// ============================================================================
// FUNCTION 7: s11d_hind_800C99A8 - Stage progression trigger (24 instructions)
// ============================================================================
// Pattern: Reads status flags, triggers progression event on threshold
void s11d_hind_800C99A8(void *a0)
{
    unsigned char *base;
    unsigned int *status_ptr;
    unsigned int status;
    short status_flags;
    short extracted_bits;
    short stage_counter;
    int shifted_value;

    base = (unsigned char *)a0;

    // Load dword at offset 0x910
    status_ptr = (unsigned int *)(base + 0x910);
    status = *status_ptr;

    // Load halfword at offset +6 from status dword
    status_flags = *(short *)((unsigned char *)status_ptr + 6);

    // Extract bits 0x1C00 and shift right by 10 bits
    extracted_bits = (status_flags & 0x1C00) >> 10;

    // Check if extracted value is non-zero
    shifted_value = extracted_bits << 10;
    if (shifted_value == 0) {
        return;
    }

    // Load counter at offset 0x914 and increment
    stage_counter = *(short *)(base + 0x914);
    stage_counter++;

    // Call external function with progression event
    func_800329C4((char *)a0 + 0x20, 0xB6, 1);
}

// ============================================================================
// FUNCTION 8: s11d_hind_800CAF9C - Cleanup dispatcher (stub)
// ============================================================================

void s11d_hind_800CAF9C(void *a0)
{
    // TODO: Decompile from s11d_hind_800CAF9C.s
}
