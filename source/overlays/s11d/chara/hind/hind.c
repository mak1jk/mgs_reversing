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

// ============================================================================
// FUNCTION 9: s11d_hind_800C9908 - Initialization routine (45 instructions)
// ============================================================================
// Pattern: Complex initialization with field setup and flag configuration
void s11d_hind_800C9908(void *a0, int a1)
{
    unsigned char *base;
    short status_val;
    short extracted_val;
    unsigned int flags;
    unsigned int result;
    short field_val;

    base = (unsigned char *)a0;

    // Move a0 to v1 (setup base pointer)
    // a1 is parameter passed in

    // Decrement a1 by 1, store at offset 0x914
    a1--;
    *(short *)(base + 0x914) = (short)a1;

    // Set value 1 at offset 0x916
    *(short *)(base + 0x916) = 1;

    // Add 0x210 to a1, store result at offset 0x910
    *(unsigned int *)(base + 0x910) = (unsigned int)(a1 + 0x210);

    // Load halfword from base+6, extract lower 5 bits
    status_val = *(short *)(base + 6);
    extracted_val = status_val & 0x1F;

    // If extracted value is non-zero, store at offset 0x1F0
    if (extracted_val != 0) {
        *(short *)(base + 0x1F0) = extracted_val;
    }

    // Load dword at offset 0x1C8 and check bit 0x10
    flags = *(unsigned int *)(base + 0x1C8);
    if ((flags & 0x10) != 0) {
        // Load halfword from offset 0x22, OR with 0x20
        field_val = *(short *)(base + 0x22);
        result = (unsigned int)field_val | 0x20;
        *(unsigned int *)(base + 0x1C8) = result;
    }

    // Load dword at offset 0x1C8, AND with 0xFF7F (clear bit 7)
    flags = *(unsigned int *)(base + 0x1C8);
    result = flags & 0xFF7F;
    *(unsigned int *)(base + 0x1C8) = result;

    // Load halfword from offset 6, shift right 4 bits, store at offset 0x1F0
    status_val = *(short *)(base + 6);
    extracted_val = status_val >> 4;
    if ((extracted_val & 0x1F) != 0) {
        *(short *)(base + 0x1F0) = extracted_val;
    }
}

// ============================================================================
// FUNCTION 9b: s11d_hind_800C9870 - Mode controller (43 instructions)
// ============================================================================
// Pattern: Multi-mode state controller with conditional field updates
void s11d_hind_800C9870(void *a0)
{
    unsigned char *base;
    unsigned int *status_ptr;
    short status_flags;
    short mode_val;
    unsigned int flags;
    short field_val;

    base = (unsigned char *)a0;

    // Load dword at offset 0x910
    status_ptr = (unsigned int *)(base + 0x910);

    // Load halfword at offset +6 from status dword
    status_flags = *(short *)((unsigned char *)status_ptr + 6);

    // Extract bits 0x300 and shift right 8 bits to get mode value (0-3)
    mode_val = (status_flags & 0x300) >> 8;

    // Branch based on mode value
    if (mode_val != 3) {
        // Not mode 3 - call external function
        func_800329C4(a0, 0, 0);
        return;
    }

    // Check if mode equals 1
    if (mode_val == 1) {
        // Load flags at offset 0x1C8 and check bit 0x2
        flags = *(unsigned int *)(base + 0x1C8);

        if ((flags & 0x2) != 0) {
            // Load halfword from offset 0x22, store at 0x1CC
            field_val = *(short *)(base + 0x22);
            *(short *)(base + 0x1CC) = field_val;
        }

        // Clear byte at 0x1CE
        *(unsigned char *)(base + 0x1CE) = 0;
        return;
    }

    // Mode must be 2
    if (mode_val == 2) {
        // Store 0xFFFF at both 0x1CC and 0x1CE
        *(unsigned int *)(base + 0x1CC) = 0xFFFFFFFF;
        *(unsigned int *)(base + 0x1CE) = 0xFFFFFFFF;
    }

    // Store dword at offset 0x1C4 (store address as dword)
    *(unsigned int *)(base + 0x1C4) = (unsigned int)(base + 0x1C4);
}

// ============================================================================
// FUNCTION 10: s11d_hind_800CAF20 - Update sequence coordinator (36 instructions)
// ============================================================================
// Pattern: Orchestration function - calls update functions in sequence
void s11d_hind_800CAF20(void *a0)
{
    unsigned char *base;
    short field_val;
    short other_field;

    base = (unsigned char *)a0;

    // Call first update function with base+0x20
    func_80025A7C((char *)base + 0x20);

    // Load halfword at offset 0x22, load another at 0x66
    field_val = *(short *)(base + 0x22);
    other_field = *(short *)(base + 0x66);

    // Add other_field to field_val
    field_val = field_val + other_field;

    // Call second update function
    func_800D2E2((char *)base, field_val);

    // Store result back at 0x22
    *(short *)(base + 0x22) = field_val;

    // Call third update function
    func_80034891((char *)base);

    // Call remaining update functions with base address
    func_800329EE(base);
    func_800329E0E(base);
    func_800B2E9B(base);
}

// ============================================================================
// FUNCTION 11: s11d_hind_800CAF9C - Cleanup dispatcher (25 instructions)
// ============================================================================
// Pattern: Cleanup and sound effect dispatcher
void s11d_hind_800CAF9C(void *a0)
{
    unsigned char *base;
    unsigned int event_flag;

    base = (unsigned char *)a0;

    // Call cleanup function 1
    func_80026033((char *)base + 0x20);

    // Call cleanup function 2
    func_800D2FE((char *)base + 0x9C);

    // Load dword at offset 0x924 (event/state flag)
    event_flag = *(unsigned int *)(base + 0x924);

    // If event flag is non-zero, call sound/event functions
    if (event_flag != 0) {
        func_8001B2B8(base);
        func_8001B701(base);
    }
}

// ============================================================================
// FUNCTION 12: s11d_hind_800CA424 - Vector rotation helper (35 instructions)
// ============================================================================
// Pattern: Vector rotation with validation and math operations
void s11d_hind_800CA424(void *a0)
{
    unsigned char *base;
    unsigned short *src_vec;
    unsigned char local_vec[32];
    unsigned char flag_byte;
    unsigned char shift_val;

    base = (unsigned char *)a0;

    // Load byte at offset 0x1C8
    flag_byte = *(unsigned char *)(base + 0x1C8);

    // Extract bits by right shift 2
    shift_val = flag_byte >> 2;

    // Check if extracted value >= 0x10
    if (shift_val >= 0x10) {
        return;  // Exit early
    }

    // Setup local vector data (stack-based)
    src_vec = (unsigned short *)(base + 0x20);

    // Copy vector bytes with specific pattern
    // Load and copy first set of bytes from source
    *(unsigned int *)&local_vec[0] = *(unsigned int *)src_vec;
    *(unsigned int *)&local_vec[4] = *(unsigned int *)(src_vec + 2);

    // Call vector math function 1
    func_800B5B50(&local_vec[0]);

    // Call vector math function 2
    func_800B5BBE(&local_vec[16]);

    // Store result back at offset 0x6E
    *(unsigned short *)(base + 0x6E) = *(unsigned short *)&local_vec[16];
}

// ============================================================================
// FUNCTION 13: s11d_hind_800CB310 - File loader (41 instructions)
// ============================================================================
// Pattern: File I/O with loop-based data processing
void s11d_hind_800CB310(void *a0)
{
    unsigned char *base;
    unsigned int *dest_ptr;
    unsigned int file_result;
    unsigned int loaded_value;
    unsigned int flags;
    int counter;
    int i;

    base = (unsigned char *)a0;

    // Call file loader with size 0x68
    file_result = func_8002063A(0x68);

    // Check if file load failed
    if (file_result == 0) {
        return;
    }

    // Initialize counter for loop
    counter = 0;
    i = 0;

    // Setup base pointer for loop processing
    dest_ptr = (unsigned int *)(base + 0x93C);

    // Loop to process loaded data (7 iterations)
    for (i = 0; i < 7; i++) {
        // Call file processing function
        loaded_value = func_8002063A(0);

        // Check processing result
        if (loaded_value == 0) {
            break;
        }

        // Check for specific value
        if (counter != i) {
            // Call another file function
            func_80031A2A(file_result);

            // Store result
            *dest_ptr = loaded_value;
            dest_ptr = (unsigned int *)((unsigned char *)dest_ptr + 4);
        }

        counter++;
    }

    // Load flags and OR with 0x10 bit
    flags = *(unsigned int *)(base + 0x1C8);
    flags = flags | 0x10;
    *(unsigned int *)(base + 0x1C8) = flags;
}
