#include "hind.h"

// s11d Hind module - helicopter actor support functions
// Extracted from MIPS assembly in asm/overlays/s11d/

// External function declarations
extern void func_800329C4(void *, int, int);
extern int GV_Time;

// Phase 2 external functions
extern int func_80017CBC(int, int);    // 0x80017CBC - transformation/hash function
extern int func_80017090(int);          // 0x80017090 - secondary transformation
extern void func_800BFC48(void *, void *);  // 0x800BFC48 - assembly/output function
extern int func_80017C4C(int, int);     // 0x80017C4C - angle constraint checker
extern int func_80092508(int, int);     // 0x80092508 - movement physics helper

// Phase 3 external functions
extern int func_80015CE4(int, int);     // 0x80015CE4 - parameter/state checker
extern int func_80015148(void *, void *, void *, void *);  // 0x80015148 - data processor
extern int func_800CA6F8(void *, void *);   // 0x800CA6F8 - handler wrapper
extern int func_800CC160(void *);       // 0x800CC160 - result processor

// Phase 4 external functions
extern void func_800CC248(void *, void *, int);  // 0x800CC248 - data processor
extern void func_800CC3D0(void);        // 0x800CC3D0 - helper function
extern int func_800CC3EA(int);          // 0x800CC3EA - result checker
extern void func_800CC304(void *, void *);  // 0x800CC304 - post-processor

// Additional Phase 4 external functions
extern void NewAnime(void *, void *);  // 0x8005FBC8 - major processor (NewAnime from functions.txt mapping)
extern void func_80019EBC(void *, void *);  // 0x80019EBC - memory copy processor (overlay-specific)
extern int func_80018274(int);          // 0x80018274 - validation checker (DG_QueuePrim equivalent)
extern void func_8001BED0(void *);      // 0x8001BED0 - component processor 1 (overlay-specific)
extern void func_80016CCC(void *, void *);  // 0x80016CCC - setup/initialization (GV_StrCode equivalent)
extern void func_8001D830(void *, void *);  // 0x8001D830 - main processor (DG_GetTexture equivalent)
extern void func_8092D5D8(void *);      // 0x8092D5D8 - complex processor
extern int func_8CC90A08(void *);       // 0x8CC90A08 - state transformation
extern void func_8CC5FCD8(void *, void *);  // 0x8CC5FCD8 - complex handler
extern int func_80032A1C(void *, int, int);  // 0x80032A1C - final dispatch
extern void func_800CA6D8(void *);      // 0x800CA6D8 - initial setup processor
extern void func_800CB178(void *);      // 0x800CB178 - state processor (Phase 5)
extern void func_800CB310(void *);      // 0x800CB310 - complex processor (Phase 5)

// Phase 5 external functions
extern void func_0C005B50(void *, void *);  // 0x0C005B50 - transform operation
extern void func_0C005B60(void *, void *);  // 0x0C005B60 - process step
extern void func_0C005BBE(void *);          // 0x0C005BBE - finalize calculation
extern int func_0C005C13(int);              // 0x0C005C13 - clamp function
extern void func_0C005B77(void *, void *);  // 0x0C005B77 - apply rotation
extern void func_0C0325DB(int, void *);     // 0x0C0325DB - position update

// Phase 5, Function 2 external functions (s11d_hind_800CB3A0)
extern void func_80025B9C(void *);          // 0x80025B9C - prologue setup
extern int func_80020968(void *, void *, int, int, int);  // 0x80020968 - state dispatcher
extern void func_8002561C(void *);          // 0x8002561C - animation controller
extern void func_8002563C(void *, int);     // 0x8002563C - state processor
extern void func_80025708(void *);          // 0x80025708 - animation handler
extern void func_80025634(void *, int);     // 0x80025634 - flag setter
extern void func_80025A7C(void *);          // 0x80025A7C - update function
extern void func_8002096E(void *);          // 0x8002096E - sub-dispatcher
extern void func_800CB05E(void *);          // 0x800CB05E - complex processor
extern void func_80020AA0(void *);          // 0x80020AA0 - loop processor
extern void func_80020AA4(void);             // 0x80020AA4 - condition checker

// Phase 6 external functions
extern void func_006F92(void *, void *);    // 0x006F92 - vector transformation (Phase 6, Function 1)
extern void func_022EE8(void *);            // 0x022EE8 - major processor (Phase 6, Function 1 exit)
extern void func_005B50(void *, void *);    // 0x005B50 - transform 1 (Phase 6, Function 2)
extern void func_005BBE(void *);            // 0x005BBE - transform 2 (Phase 6, Function 2)
extern void func_005C13(int);               // 0x005C13 - transform 3 (Phase 6, Function 2)
extern void func_005B60(void *);            // 0x005B60 - store result (Phase 6, Function 2)
extern void func_005B77(void *, void *);    // 0x005B77 - final transform (Phase 6, Function 2)
extern void func_325DB(int, void *);        // 0x325DB - iterative processor (Phase 6, Function 2)

// Additional Phase 6 external functions
extern void func_024942(void);              // 0x024942 - division/normalization handler (Phase 6, Function 2)
extern void func_03261C(void *);            // 0x03261C - state transition handler (Phase 6, Function 2)

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
// FUNCTION 8: s11d_hind_800CAF9C - Cleanup dispatcher
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

// ============================================================================
// PHASE 2 FUNCTIONS - Complex vector/angle calculations
// ============================================================================

// ============================================================================
// FUNCTION: s11d_hind_800C96D0 - Vector transformation/hash calculation (1103 bytes)
// ============================================================================
// Pattern: Loads 3 halfword values, applies transformations, assembles results
// Calls external functions for data transformation
void s11d_hind_800C96D0(void *a0)
{
    unsigned char *base;
    unsigned short val0, val2, val4;
    short stack_buffer[4];
    unsigned short *result_ptr;
    int transformed0, transformed2, transformed4;
    int temp_result;
    void *data_ptr;

    base = (unsigned char *)a0;

    // Load the three halfword values from input at offsets 0, 2, 4
    val0 = *(unsigned short *)(base + 0);
    val2 = *(unsigned short *)(base + 2);
    val4 = *(unsigned short *)(base + 4);

    // First transformation: call func_80017CBC with 0x800 and val0
    transformed0 = func_80017CBC(0x800, val0);
    stack_buffer[0] = (short)val0;  // Store original

    // Second transformation: call func_80017CBC with 0x800 and val2
    transformed2 = func_80017CBC(0x800, val2);
    *(short *)((unsigned char *)stack_buffer + 2) = (short)val2;

    // Third transformation: call func_80017CBC with 0x80 and val4
    transformed4 = func_80017CBC(0x80, val4);
    *(short *)((unsigned char *)stack_buffer + 4) = (short)val4;

    // Secondary transformation function call
    temp_result = func_80017090(0);

    // Get data pointer - address 0x800C32F0
    data_ptr = (void *)0x800C32F0;

    // Store parameter values on stack
    *(int *)((unsigned char *)stack_buffer + 8) = 0x0001;

    // Final assembly function call
    func_800BFC48(stack_buffer, data_ptr);
}

// ============================================================================
// FUNCTION: s11d_hind_800C9B94 - Angle adjustment controller (1597 bytes)
// ============================================================================
void s11d_hind_800C9B94(void *a0)
{
    unsigned char *base;
    unsigned char *work_ptr;
    short angle_field;
    short param_field;
    int condition_result;
    int boundary_val;
    int delta_val;
    short final_angle;
    int *angle_container;
    int current_angle;
    int negated;
    short val_at_1f2;
    int calc_val;
    short val_at_50;
    short val_at_964;

    base = (unsigned char *)a0;
    work_ptr = base + 32;

    // Load angle field at offset 0x4E (78) and param at offset 0x0A (10)
    angle_field = *(short *)(work_ptr + 0x4E);
    param_field = *(short *)(work_ptr + 0x0A);

    // Call external constraint function
    condition_result = func_80017C4C(angle_field, param_field);

    // Check if result is zero
    if (condition_result != 0) {
        // Load field at offset 0x964
        angle_container = (int *)(base + 0x964);
        current_angle = *angle_container;

        // Boundary checking logic
        if (current_angle >= -4) {
            // Range check
            if (current_angle < 5) {
                // Update angle: clear it
                *angle_container = 0;
            } else {
                // Decrement
                *angle_container = current_angle - 4;
            }
        } else {
            // Signed negative case
            negated = -(short)angle_field;

            // Load from offset 0x1F2 (498)
            val_at_1f2 = *(short *)(work_ptr + 0x1F2);
            calc_val = val_at_1f2 - current_angle;

            // Boundary check
            if (calc_val >= -16) {
                if (calc_val > 17) {
                    delta_val = calc_val + 16;
                } else {
                    delta_val = calc_val + negated;
                }
            } else {
                delta_val = calc_val - 16;
            }

            *angle_container = delta_val;
        }
    }

    // Final operation: read fields and combine
    val_at_50 = *(short *)(work_ptr + 0x50);
    val_at_964 = *(short *)(base + 0x964);
    final_angle = val_at_50 + val_at_964;
    *(short *)(work_ptr + 0x50) = final_angle;
}

// ============================================================================
// FUNCTION: s11d_hind_800C9C7C - Movement physics (1571 bytes)
// ============================================================================
void s11d_hind_800C9C7C(void *a0)
{
    unsigned char *base;
    int *global_ptr;
    int global_val;
    int s0, s1, s2;
    int shifted_val, calc_result;
    int temp_param, result;
    short motion_field;
    short val_at_95c;
    short val_at_gvc;

    base = (unsigned char *)a0;

    // Load global variable at 0x800B0000 - 0x4CF0 = 0x800AB330
    global_ptr = (int *)0x800AB330;
    global_val = *global_ptr;

    // Initialize s2 = a0 (base pointer)
    s2 = (int)base;

    // Check sign of global value
    if (global_val >= 0) {
        s0 = global_val;
    } else {
        s0 = global_val;
        // Arithmetic right shift by 6 (divide by 64) with rounding
        s0 = (global_val + 63) >> 6;
    }

    // First transformation calculation
    // s0 = s0 - (s0 << 6)
    s1 = s0 << 6;
    s0 = s0 - s1;

    // s2 adjustment: s2 = s0 << 6
    s2 = s0 << 6;

    // First external function call
    temp_param = s2;
    func_80092508(temp_param, s0);

    // Second calculation
    temp_param = s0 - 1;
    s1 = temp_param << 7;
    func_80092508(s1, temp_param);

    // Third calculation with branching
    s0 = s2 - s1;
    if (s0 >= 0) {
        // Use as-is
        s1 = s0 >> 6;
    } else {
        s1 = (s0 + 7) >> 6;
    }

    // Load and process motion field at offset 0x66
    motion_field = *(short *)(base + 0x66);
    result = motion_field + s1;
    func_80092508(result, s1);
    *(short *)(base + 0x66) = (short)result;

    // Fourth calculation
    temp_param = result << 5;
    shifted_val = temp_param << 3;

    // Load field at offset 0x95C
    val_at_95c = *(short *)(base + 0x95C);
    calc_result = val_at_95c + shifted_val;

    if (calc_result >= 0) {
        s1 = calc_result >> 8;
    } else {
        s1 = (calc_result + 4095) >> 8;
    }

    func_80092508(calc_result, s1);
    *(short *)(base + 0x6C) = (short)calc_result;

    // Fifth calculation
    temp_param = calc_result << 7;
    shifted_val = temp_param << 0;

    val_at_gvc = *(short *)(base + 0x95C);
    calc_result = val_at_gvc + shifted_val;

    if (calc_result >= 0) {
        s1 = calc_result >> 8;
    } else {
        s1 = (calc_result + 4095) >> 8;
    }

    *(short *)(base + 0x70) = (short)calc_result;
}

// ============================================================================
// PHASE 3 FUNCTIONS - Entry dispatchers and state handlers
// ============================================================================

// ============================================================================
// FUNCTION: s11d_hind_800CB708 - Entry dispatcher with parameter checking (999 bytes)
// ============================================================================
int s11d_hind_800CB708(void *a0, void *a1)
{
    int result;
    void *handler_ptr1;
    void *handler_ptr2;
    void *handler_ptr3;

    // Check parameters using external checker function
    // Parameters: 5 (state ID) and 0x097C (2428 - offset)
    result = func_80015CE4(5, 0x097C);

    // If result is 0, return 0 (exit)
    if (result == 0) {
        return 0;
    }

    // Set up handler function pointers
    handler_ptr1 = (void *)0x800CAF20;   // From LUI 0x800D + offset
    handler_ptr2 = (void *)0x800CAF9C;   // From LUI 0x800D + offset
    handler_ptr3 = (void *)0x800D1E48;   // From LUI 0x800D + offset

    // Call data processor with handlers
    func_80015148(a0, handler_ptr1, handler_ptr2, handler_ptr3);

    // Call handler wrapper with parameters
    func_800CA6F8(a0, a1);

    // Final result processor
    result = func_800CC160(a0);

    // If result is non-zero, return 1
    if (result != 0) {
        return 1;
    }

    return 0;
}

// ============================================================================
// FUNCTION: s11d_hind_800CAE6C - State maintenance handler (1.2K)
// ============================================================================
void s11d_hind_800CAE6C(void *a0)
{
    unsigned char *base;
    int state_val;

    base = (unsigned char *)a0;

    // Load state field at offset 0x910
    state_val = *(int *)(base + 0x910);

    // Decrement state if non-zero
    if (state_val > 0) {
        state_val--;
        *(int *)(base + 0x910) = state_val;
    }
}

// ============================================================================
// FUNCTION: s11d_hind_800CAD9C - Counter reset handler (1.4K)
// ============================================================================
void s11d_hind_800CAD9C(void *a0)
{
    unsigned char *base;
    short counter_val;
    int zero_val;

    base = (unsigned char *)a0;

    // Load counter at offset 0x2424
    counter_val = *(short *)(base + 0x2424);

    // If counter > 0, decrement it
    if (counter_val > 0) {
        counter_val--;
        *(short *)(base + 0x2424) = counter_val;

        // If decremented value reaches specific condition
        zero_val = *(int *)(base + 0x2424);
        if (zero_val == 0) {
            // Clear related field at 0x2426
            *(short *)(base + 0x2426) = 0;
        }
    }
}

// ============================================================================
// PHASE 4 FUNCTIONS - Medium-High complexity handlers
// ============================================================================

// ============================================================================
// FUNCTION: s11d_hind_800CB054 (1.9K)
// ============================================================================
void s11d_hind_800CB054(void *a0)
{
    unsigned char *base;
    int *data_container;
    int container_val;
    short *field_ptr;
    short field_val;
    int counter;
    int inner_counter;
    void *result_ptr;
    int result;
    unsigned char *scan_ptr;
    unsigned char byte_val1;
    unsigned char byte_val2;

    base = (unsigned char *)a0;

    // Load container pointer at offset 0x4C
    data_container = (int *)(base + 0x4C);
    container_val = *data_container;

    // Load nested pointer at offset 0x8 within container
    if (container_val != 0) {
        int *nested = (int *)((unsigned char *)container_val + 0x8);
        container_val = *nested;
    }

    // Load field pointer and value
    field_ptr = (short *)(base + 0x4C);
    field_val = *(short *)(field_ptr);

    if (field_val != 0) {
        // Initialize counters
        counter = 0;
        result_ptr = (void *)(base + 0x200);

        // Loop processing
        for (counter = 0; counter < 16; counter++) {
            // Load from offset 0x210 + counter
            int loop_val = *(int *)(base + 0x210 + counter);

            // Call external data processor
            func_800CC248(base, result_ptr, loop_val);

            // Advance result pointer
            result_ptr = (void *)((unsigned char *)result_ptr + 8);
        }

        // Store marker at specific offset
        *(short *)(base + 0x1F0) = 0x20;

        // Call final processor
        result = func_800CC3EA(0x72);

        if (result != 0) {
            // Post-process with additional functions
            func_800CC3D0();
            func_800CC304(base, result_ptr);
        }
    }
}

// ============================================================================
// FUNCTION: s11d_hind_800C958C (2.1K) - Data Transformation Coordinator
// ============================================================================
void s11d_hind_800C958C(void *a0)
{
    unsigned char *base;
    short val0;
    short val2;
    short val4;
    short result0;
    short result2;
    short result4;
    unsigned short stack_buf[4];
    void *data_ptr;
    int transform0;
    int transform2;
    int transform4;
    int global_flag;
    int flag_check;

    base = (unsigned char *)a0;

    // === First pass: Transform three halfword values ===
    // Load halfword at offset 0, transform with 0x200 magnitude, subtract 64 (0xFFC0)
    val0 = *(short *)(base + 0x0000);
    transform0 = func_80017CBC(0x200, val0);
    result0 = (short)(transform0 - 0xFFC0);  // Subtract 64 in two's complement
    stack_buf[0] = (unsigned short)result0;

    // Load halfword at offset 2, transform with 0x100 magnitude, subtract 128 (0xFF80)
    val2 = *(short *)(base + 0x0002);
    transform2 = func_80017CBC(0x100, val2);
    result2 = (short)(transform2 - 0xFF80);  // Subtract 128 in two's complement
    stack_buf[1] = (unsigned short)result2;

    // Load halfword at offset 4, transform with 0x40 magnitude, subtract 64 (0xFFC0)
    val4 = *(short *)(base + 0x0004);
    transform4 = func_80017CBC(0x40, val4);
    result4 = (short)(transform4 - 0xFFC0);  // Subtract 64 in two's complement
    stack_buf[2] = (unsigned short)result4;

    // Apply constraint check on transformed value
    func_80017C4C(0x80, 0);

    // Set up data pointer from global data section (0x800C_32D4)
    data_ptr = (void *)0x800C32D4;

    // Call major processor with data pointer and stack buffer
    NewAnime(data_ptr, (void *)stack_buf);

    // Load global flag at 0x800B_B330 and check bits 0-1
    global_flag = *(int *)0x800BB330;
    flag_check = global_flag & 0x3;

    if (flag_check != 0) {
        // === Conditional second pass if flag bits 0-1 set ===
        // Repeat transformation process with different data pointer (0x800C_32F0)

        // Re-transform first value
        val0 = *(short *)(base + 0x0000);
        transform0 = func_80017CBC(0x200, val0);
        result0 = (short)(transform0 - 0xFFC0);
        stack_buf[0] = (unsigned short)result0;

        // Re-transform second value
        val2 = *(short *)(base + 0x0002);
        transform2 = func_80017CBC(0x100, val2);
        result2 = (short)(transform2 - 0xFF80);
        stack_buf[1] = (unsigned short)result2;

        // Re-transform third value
        val4 = *(short *)(base + 0x0004);
        transform4 = func_80017CBC(0x40, val4);
        result4 = (short)(transform4 - 0xFFC0);
        stack_buf[2] = (unsigned short)result4;

        // Apply constraint check
        func_80017C4C(0x80, 0);

        // Set up alternate data pointer (0x800C_32F0)
        data_ptr = (void *)0x800C32F0;

        // Call major processor with alternate data pointer
        NewAnime(data_ptr, (void *)stack_buf);
    }
}

// ============================================================================
// FUNCTION: s11d_hind_800CB178 (2.7K) - Actor Initialization & Setup
// ============================================================================
void s11d_hind_800CB178(void *a0)
{
    unsigned char *base;
    short *init_ptr0;
    short *init_ptr1;
    unsigned char *copy_src;
    unsigned char *copy_dst;
    int copy_word0;
    int copy_word1;
    int processor_result;
    int loop_counter;
    unsigned char *array_ptr;
    unsigned char byte_val;
    unsigned char source_byte;
    int bitfield_val;
    int result_check;

    base = (unsigned char *)a0;

    // === Initialize halfword values in structure ===
    // Set field at offset 0x930 to 0x0190
    init_ptr0 = (short *)(base + 0x0930);
    *init_ptr0 = 0x0190;

    // Set field at offset 0x932 to 0x0320
    init_ptr1 = (short *)(base + 0x0930 + 2);
    *init_ptr1 = 0x0320;

    // === Copy 8-byte data block using unaligned operations ===
    // Source is global data at 0x800B_B39C
    copy_src = (unsigned char *)0x800BB39C;
    copy_dst = base + 0x0928;

    // Copy first 4 bytes (unaligned load/store pattern)
    copy_word0 = *(int *)copy_src;
    *(int *)copy_dst = copy_word0;

    // Copy second 4 bytes
    copy_word1 = *(int *)(copy_src + 4);
    *(int *)(copy_dst + 4) = copy_word1;

    // Call memory processor on copied data
    func_80019EBC(copy_dst, (void *)(base + 0x0938));

    // Validate result
    processor_result = func_80018274(0x1);

    if (processor_result != 0) {
        // Store result to offset 0x924
        *(int *)(base + 0x0924) = processor_result;
    }

    // === Loop through 2 iterations of array initialization ===
    // Initialize with setup function at 0x800D_1E40
    func_80016CCC((void *)(base + 0x0928), (void *)0x800D1E40);

    // Process 2 iterations
    for (loop_counter = 0; loop_counter < 2; loop_counter++) {
        array_ptr = base + 0x0928 + (loop_counter * 0x10);  // 16-byte stride per iteration

        // Write byte pattern 0x2C (44) to offset 7
        *(unsigned char *)(array_ptr + 7) = 0x2C;

        // Write byte pattern 0x09 (9) to offset 3
        *(unsigned char *)(array_ptr + 3) = 0x09;

        // Write byte pattern 0x80 (128) to offset 4
        *(unsigned char *)(array_ptr + 4) = 0x80;

        // Read byte from offset 5 and modify with bitfield operation
        byte_val = *(unsigned char *)(array_ptr + 5);
        bitfield_val = byte_val & 0xFF9F;  // Clear bits 5-6
        bitfield_val |= 0x20;              // Set bit 5
        *(unsigned char *)(array_ptr + 5) = (unsigned char)bitfield_val;

        // Read byte from offset 6 and apply transformation
        source_byte = *(unsigned char *)(array_ptr + 6);
        *(unsigned char *)(array_ptr + 6) = source_byte;

        // Call processor function on this iteration
        func_8001D830((void *)array_ptr, (void *)(base + 0x0938));
    }

    // Set flag bit 0x100 in component field at offset 0x24
    if (processor_result != 0) {
        int *flag_ptr = (int *)(base + 0x0924);
        int flag_val = *flag_ptr;
        flag_val |= 0x100;  // Set bit 8
        *flag_ptr = flag_val;
    }
}

// ============================================================================
// FUNCTION: s11d_hind_800CA504 (3.1K) - State Machine Event Handler
// ============================================================================
void s11d_hind_800CA504(void *a0)
{
    unsigned char *base;
    int flag_word;
    unsigned short state_discriminator;
    short val_at_minus4;
    short val_at_minus2;
    int loop_count;
    int stride_offset;
    void *array_ptr;
    short *value_ptr;
    int state_result;
    int comparison_val;

    base = (unsigned char *)a0;

    // === Early exit check ===
    flag_word = *(int *)(base + 0x01C8);
    if ((flag_word & 0x4) != 0) {
        return;  // Exit if bit 2 is set
    }

    // === Load state discriminator from global ===
    state_discriminator = *(unsigned short *)0x800B4DB4;

    // === State machine with three magic constant paths ===
    if (state_discriminator == 0x5A47) {
        // Path 1: Magic constant 0x5A47 - Clear bits 0-1, set bit 1
        flag_word = *(int *)(base + 0x01C8);
        flag_word &= 0xFFFFFFFC;  // Clear bits 0-1
        flag_word |= 0x2;          // Set bit 1
        *(int *)(base + 0x01C8) = flag_word;
    } else if (state_discriminator == 0x983F) {
        // Path 2: Magic constant 0x983F - Clear bits 0 and 4, set bit 0
        flag_word = *(int *)(base + 0x01C8);
        flag_word &= 0xFFFFFFEE;  // Clear bits 0 and 4
        flag_word |= 0x1;          // Set bit 0
        *(int *)(base + 0x01C8) = flag_word;
    } else if (state_discriminator == 0x6A3E) {
        // Path 3: Magic constant 0x6A3E - Complex comparison with array processing

        // Initialize loop counter
        loop_count = 0;
        array_ptr = (void *)(base + 0x007C);

        // Loop through array with stride = (n * 5) << 2 = 20 bytes per iteration
        while (loop_count < 16) {  // Assume max 16 iterations based on typical patterns
            // Calculate stride offset
            stride_offset = (loop_count * 5) << 2;  // 20 bytes per iteration
            value_ptr = (short *)((unsigned char *)array_ptr + stride_offset);

            // Load halfwords at current-4 and current-2
            val_at_minus4 = *(short *)(value_ptr - 4);
            val_at_minus2 = *(short *)(value_ptr - 2);

            // Perform comparisons and conditional processing
            comparison_val = *(int *)(base + 0x0200);
            if (val_at_minus4 != comparison_val && val_at_minus2 != 0) {
                // Call state transformation function
                state_result = func_8CC90A08(base);

                if (state_result != 0) {
                    // Set flag bit 0x100
                    flag_word = *(int *)(base + 0x01C8);
                    flag_word |= 0x100;
                    *(int *)(base + 0x01C8) = flag_word;

                    // Store results
                    *(short *)(base + 0x01E0) = val_at_minus4;
                    *(short *)(base + 0x01E2) = val_at_minus2;

                    break;  // Exit loop
                }
            }

            loop_count++;
        }
    } else {
        // Default path - minimal operation
        flag_word = *(int *)(base + 0x01C8);
        flag_word |= 0x100;  // Set bit 8
        *(int *)(base + 0x01C8) = flag_word;
    }

    // === Store final state indicator ===
    *(short *)(base + 0x01F8) = 0x1;
}

// ============================================================================
// FUNCTION: s11d_hind_800CABA8 (3.3K) - Physics/Collision Calculator
// ============================================================================
void s11d_hind_800CABA8(void *a0)
{
    unsigned char *base;
    int *component_ptr;
    int component_val;
    int flag_word;
    int global_flag;
    int comparison_val1;
    int comparison_val2;
    int comparison_val3;
    int byte_val_8;
    int byte_val_9;
    int byte_val_10;
    int byte_val_11;
    int mult_result;
    int mult_result2;
    int sign_bit;
    int calc_result1;
    int calc_result2;
    int calc_result3;
    short calc_array[16];
    int array_index;
    int dispatch_result;

    base = (unsigned char *)a0;

    // === Call initial processor ===
    func_800CA6D8(base);

    // === Load component pointer from offset 0x0924 ===
    component_ptr = (int *)(base + 0x0924);
    component_val = *component_ptr;

    // Set flag bit 0x100 in component
    flag_word = *(int *)(component_val + 0x24);
    flag_word |= 0x100;
    *(int *)(component_val + 0x24) = flag_word;

    // === Perform three comparison checks ===
    comparison_val1 = *(int *)(base + 0x01D0);
    comparison_val2 = *(int *)(base + 0x01F8);
    comparison_val3 = *(int *)(base + 0x01FA);

    // Early exit if comparisons fail
    if (comparison_val1 == 0 || comparison_val2 == 0 || comparison_val3 == 0) {
        return;
    }

    // === Check global flag bit 0 ===
    global_flag = *(int *)0x800BB330;
    if ((global_flag & 0x1) == 0) {
        return;  // Exit if bit 0 not set
    }

    // === Call two component processors ===
    func_8001BED0(base);
    func_8092D5D8((void *)component_val);

    // === Clear flag bit 0x100 and call magnitude processor ===
    flag_word = *(int *)(component_val + 0x24);
    flag_word &= ~0x100;  // Clear bit 8
    *(int *)(component_val + 0x24) = flag_word;

    func_80017CBC(0x400, 0);  // Call with magnitude 0x400

    // === Complex fixed-point math calculations ===
    // Load bytes from component at offset 0x0938
    byte_val_8 = *(unsigned char *)((unsigned char *)component_val + 0x0938 + 8) & 0xFF;
    byte_val_9 = *(unsigned char *)((unsigned char *)component_val + 0x0938 + 9) & 0xFF;
    byte_val_10 = *(unsigned char *)((unsigned char *)component_val + 0x0938 + 10) & 0xFF;
    byte_val_11 = *(unsigned char *)((unsigned char *)component_val + 0x0938 + 11) & 0xFF;

    // Multiply byte values and apply fixed-point rounding
    // Pattern: mult = byte1 * byte2; rounded = (mult + (mult >> 31)) >> 1
    mult_result = byte_val_8 * byte_val_9;
    sign_bit = mult_result >> 31;
    calc_result1 = (mult_result + sign_bit) >> 1;

    mult_result2 = byte_val_10 * byte_val_11;
    sign_bit = mult_result2 >> 31;
    calc_result2 = (mult_result2 + sign_bit) >> 1;

    // Combine results
    calc_result3 = calc_result1 + calc_result2;

    // Store calculation results to array
    for (array_index = 0; array_index < 16; array_index++) {
        calc_array[array_index] = (short)(calc_result3 ^ array_index);
    }

    // === Call final complex processor with array ===
    func_8CC5FCD8((void *)calc_array, base);

    // === Call final dispatch function ===
    dispatch_result = func_80032A1C((void *)component_val, 0xB4, 1);  // 0xB4 = 180

    // Store final result marker
    *(int *)(base + 0x01E0) = dispatch_result;
}

// ============================================================================
// PHASE 5, FUNCTION 1: s11d_hind_800C99F4 - Rotation control system (164 bytes)
// ============================================================================
// Pattern: Complex state machine with conditional processing and boundary clamping
// Processes rotation values with transformation, applies constraints, and updates state
void s11d_hind_800C99F4(void *a0)
{
    unsigned char *base;
    unsigned char *stack_work;
    short val_0x23;
    short val_0x27;
    short buf_val1;
    short buf_val2;
    int control_flag;
    int limit_16bit;
    short short_1F0;
    int rotation_angle;
    int clamped_result;
    int global_limit;
    int compare_result;
    short stack_buf[20];

    base = (unsigned char *)a0;

    // Store value to offset 0x918
    *(int *)(base + 0x918) = 0;

    // Load and copy values at offsets 0x23 and 0x27
    val_0x23 = *(short *)(base + 0x23);
    val_0x27 = *(short *)(base + 0x27);

    stack_work = (unsigned char *)stack_buf + 0x1B;
    *(unsigned char *)stack_work = (unsigned char)val_0x23;
    stack_work = (unsigned char *)stack_buf + 0x18;
    *(unsigned char *)stack_work = (unsigned char)(val_0x23 >> 8);

    stack_work = (unsigned char *)stack_buf + 0x1F;
    *(unsigned char *)stack_work = (unsigned char)val_0x27;
    stack_work = (unsigned char *)stack_buf + 0x1C;
    *(unsigned char *)stack_work = (unsigned char)(val_0x27 >> 8);

    // Load control flags from offset 0x910
    control_flag = *(int *)(base + 0x910);

    // Check bit 0 of control flag
    if ((control_flag & 0x1) == 0) {
        // Clear halfwords if bit 0 is not set
        *(short *)((unsigned char *)stack_buf + 0x22) = 0;
        *(short *)((unsigned char *)stack_buf + 0x1A) = 0;
    }

    // Call transform operations
    func_0C005B50((void *)((unsigned char *)stack_buf + 0x18), (void *)((unsigned char *)stack_buf + 0x10));
    func_0C005B60((void *)((unsigned char *)stack_buf + 0x10), NULL);

    // Load limit value and perform comparison
    short_1F0 = *(short *)(base + 0x1F0);
    limit_16bit = (int)short_1F0;

    // Clamp the result to constraint range
    if (limit_16bit < -24) {
        clamped_result = func_0C005C13(-24);
    } else if (limit_16bit > 24) {
        clamped_result = func_0C005C13(24);
    } else {
        clamped_result = func_0C005C13(limit_16bit);
    }

    // Call finalize function
    func_0C005BBE((void *)((unsigned char *)stack_buf + 0x10));

    // Load rotation angle from stack and update field
    rotation_angle = *(short *)((unsigned char *)stack_buf + 0x6E);
    *(unsigned char *)(base + 0x74) = 0x0F;

    buf_val1 = *(short *)((unsigned char *)stack_buf + 0x6E);
    short_1F0 = *(short *)(base + 0x1F0);

    // Update rotation with constraint
    rotation_angle = (int)short_1F0 + (int)buf_val1;
    *(short *)(base + 0x6E) = (short)rotation_angle;

    // Call apply rotation with constraint values
    func_0C005B77((void *)((unsigned char *)stack_buf + 0x10), (void *)((unsigned char *)stack_buf + 0x28));

    // Load rotation value from 0x1F0
    short_1F0 = *(short *)(base + 0x1F0);

    // Check control flag again
    if ((control_flag & 0x1) != 0) {
        // Load additional values for secondary processing
        buf_val1 = *(short *)((unsigned char *)stack_buf + 0x22);
        buf_val2 = *(short *)(base + 0x1DA);

        // Load global limit from memory
        global_limit = *(int *)0x800BBA12;

        // Compute difference
        global_limit = global_limit + (int)buf_val1;
        compare_result = global_limit - (int)buf_val2;

        // Clamp to range [-128, 128]
        if (compare_result < -128) {
            clamped_result = -128;
        } else if (compare_result > 128) {
            clamped_result = 128;
        } else {
            clamped_result = compare_result;
        }

        *(short *)(base + 0x66) = (short)clamped_result;
    } else {
        *(short *)(base + 0x66) = *(short *)((unsigned char *)stack_buf + 0x20);
    }

    // Call position update functions
    func_0C0325DB(100 + (int)base, (void *)((unsigned char *)stack_buf + 0x28));
    func_0C0325DB(104 + (int)base, (void *)((unsigned char *)stack_buf + 0x2C));
}

// ============================================================================
// PHASE 5, FUNCTION 2: s11d_hind_800CB3A0 - Character state dispatcher (364 bytes)
// ============================================================================
// Pattern: Complex state machine with 30+ external calls, multiple branching paths
void s11d_hind_800CB3A0(void *a0)
{
    unsigned char *base;
    unsigned char *work_ptr;
    int dispatch_result;
    int loop_counter;
    int global_val;
    unsigned int flags;
    short halfword_val;
    unsigned char byte_vals[8];

    base = (unsigned char *)a0;

    // === Prologue: Setup work area (offset 0x20) ===
    work_ptr = base + 0x20;

    // Call prologue setup function
    func_80025B9C(work_ptr);

    // === Check initial condition at offset 0x1C8 ===
    dispatch_result = *(int *)(base + 0x1C8);
    if (dispatch_result == -1) {
        return;  // Early exit if flag is -1
    }

    // === State dispatch sequence ===
    // Dispatch code 0x70
    func_80020968(base, base, 0x70, 0, 0);

    // Dispatch code 0x64
    func_80020968(base, base, 0x64, 0, 0);

    // Animation setup calls
    func_8002561C(work_ptr);
    func_8002563C(work_ptr, 1);
    func_80025708(work_ptr);
    func_80025634(work_ptr, -2);

    // === Dispatch code 0x6D with result check ===
    dispatch_result = func_80020968(base, base, 0x6D, 0, 0);
    if (dispatch_result == 0) {
        // Skip major processing
        goto finalization;
    }

    // === Major processing path ===
    func_8002096E(base + 0x9C);

    func_800CB05E(base + 0x9C);
    func_800CB178(work_ptr);
    func_800CB310(work_ptr);
    func_8002096E(base);
    func_8002563C(base, 45);

    // Store state marker byte at 0x59
    *(unsigned char *)(base + 0x59) = 0x02;

    // === Memory copy operations ===
    // Load global data from 0x800BB39C and copy to base offsets
    global_val = *(int *)0x800BB39C;
    byte_vals[0] = *(unsigned char *)(global_val + 0x0003);
    byte_vals[1] = *(unsigned char *)(global_val + 0x0000);
    byte_vals[2] = *(unsigned char *)(global_val + 0x0007);
    byte_vals[3] = *(unsigned char *)(global_val + 0x0004);

    *(unsigned char *)(base + 0x1D3) = byte_vals[0];
    *(unsigned char *)(base + 0x1D0) = byte_vals[1];
    *(unsigned char *)(base + 0x1D7) = byte_vals[2];
    *(unsigned char *)(base + 0x1D4) = byte_vals[3];

    // Clear multiple offsets
    *(int *)(base + 0x1C8) = 0;
    *(int *)(base + 0x1C0) = 0;
    *(int *)(base + 0x1C4) = 0;

    *(short *)(base + 0x1EA) = 0;
    *(short *)(base + 0x1E8) = 0;
    *(short *)(base + 0x1EE) = 0;
    *(short *)(base + 0x1EC) = 0;

    // === Dispatch code 0x73 ===
    dispatch_result = func_80020968(base, base, 0x73, 0, 0);
    if (dispatch_result != 0) {
        // Process animations if condition met
        func_8002096E(NULL);
        func_8002096E(base);
        func_8002096E(base);
        func_8002096E(base);
    }

    // === Dispatch code 0x78 ===
    dispatch_result = func_80020968(base, base, 0x78, 0, 0);
    if (dispatch_result != 0) {
        // Set flag bit
        flags = *(unsigned int *)(base + 0x1C8);
        flags |= 0x1;
        *(unsigned int *)(base + 0x1C8) = flags;
    }

    // === Dispatch code 0x7A ===
    dispatch_result = func_80020968(base, base, 0x7A, 0, 0);
    if (dispatch_result != 0) {
        // Complex processor with offset data
        func_8002096E(NULL);
        func_8002096E(base);

        // Load global data from 0x800C3310
        global_val = *(int *)0x800C3310;
        byte_vals[0] = *(unsigned char *)(global_val + 0x0003);
        byte_vals[1] = *(unsigned char *)(global_val + 0x0000);
        byte_vals[2] = *(unsigned char *)(global_val + 0x0007);
        byte_vals[3] = *(unsigned char *)(global_val + 0x0004);

        *(unsigned char *)(base + 0x1DB) = byte_vals[0];
        *(unsigned char *)(base + 0x1D8) = byte_vals[1];
        *(unsigned char *)(base + 0x1DF) = byte_vals[2];
        *(unsigned char *)(base + 0x1DC) = byte_vals[3];

        goto finalization;
    }

    // === Dispatch code 0x62 ===
    dispatch_result = func_80020968(base, base, 0x62, 0, 0);
    if (dispatch_result == 0) {
        // Clear flag if dispatch failed
        *(int *)(base + 0x0920) = -1;
    } else {
        // Store result to offset 0x0920
        *(int *)(base + 0x0920) = dispatch_result;
    }

    // === Dispatch code 0x67 ===
    dispatch_result = func_80020968(base, base, 0x67, 0, 0);
    if (dispatch_result != 0) {
        // Conditional sub-dispatch for 0x67
        func_80020968(NULL, NULL, 0, 0, 0);
        func_8002096E(base + 0x1D8);
        goto finalization;
    }

    // === Dispatch code 0x61 ===
    dispatch_result = func_80020968(base, base, 0x61, 0, 0);
    if (dispatch_result != 0) {
        // Conditional handler for 0x61
        func_80020968(NULL, NULL, 0, 0, 0);
        func_8002096E(base);
        *(int *)(base + 0x091C) = dispatch_result;
        goto finalization;
    }

    // === Dispatch code 0x79 (condition-dependent) ===
    halfword_val = *(short *)0x800B4D9A;
    if (halfword_val != 0) {
        loop_counter = 0;

        // Loop over items
        if (loop_counter < 1) {
            do {
                func_80020AA0(base);
                loop_counter++;
            } while (loop_counter < 1);
        }

        func_80020AA0(NULL);
    }

    // Store final result to global
    *(int *)0x800C330C = 0;

finalization:
    // Function epilogue
    return;
}

// ============================================================================
// PHASE 6 FUNCTIONS (27-28/28) - FINAL COMPLETION
// ============================================================================

// ============================================================================
// FUNCTION 27: s11d_hind_800CA6D8 - Multi-way state processor (8.1 KB)
// ============================================================================
// Pattern: 4-way switch with fixed-point arithmetic, safe division, bit manipulation
// Key characteristics:
// - State-based dispatch (4 cases: 0, 1, 2, 3)
// - Fixed-point division using 0x66666667 constant
// - Global data dependencies (0x800BB39C, 0x800BBA12, 0x800BB330)
// - Bit field operations on offset 0x1C8
// - 2 external function calls
void s11d_hind_800CA6D8(void *a0)
{
    unsigned char *base;
    unsigned char *sp_buf;
    short state;
    short global_vec_x, global_vec_y;
    short param_val;
    short offset_1F8, offset_1FA;
    short offset_1D0, offset_1E0;
    short result_x, result_y;
    short ext_result_x, ext_result_y;
    short buf[4];
    int div_val, div_hi;
    int temp_int;
    unsigned int bitfield;
    int constraint_cmp;
    short stored_val;
    int temp_div;
    short *stack_ptr;

    base = (unsigned char *)a0;
    sp_buf = base + 0x10;
    state = *(short *)(base + 0x1CE);

    switch (state) {
        case 0:
            /* CASE_0: Clear bit 0x08, load vector from global, setup stack, call func_006F92 */
            bitfield = *(unsigned int *)(base + 0x1C8);
            bitfield &= 0xFFF7;  /* Clear bit 0x08 */
            *(unsigned int *)(base + 0x1C8) = bitfield;

            /* Load 8-byte vector from global 0x800BB39C */
            stack_ptr = (short *)(sp_buf + 3);
            global_vec_x = *(short *)0x800BB39C;
            global_vec_y = *(short *)0x800BB3A0;
            *(short *)(base + 0x1D0) = global_vec_x;
            *(short *)(base + 0x1D2) = global_vec_y;

            /* Setup stack parameters */
            param_val = 0x7530;  /* 30000 in decimal */
            *(short *)(sp_buf + 4) = param_val;
            *(int *)(sp_buf + 0) = 1;

            /* Call external function */
            func_006F92(base, sp_buf);
            break;

        case 1:
            /* CASE_1: Fixed-point division with constraint checking and result storage */
            /* Check constraint at offset 0x1FA < 0x0800 */
            constraint_cmp = *(unsigned int *)(base + 0x1C8);
            constraint_cmp &= 0xFFF7;
            offset_1FA = *(short *)(base + 0x1FA);
            offset_1FA &= 0x0FFF;

            if (offset_1FA < 0x0800) {
                offset_1F8 = *(short *)(base + 0x1F8);
                temp_int = offset_1F8 + 0xFFA0;  /* -96 in two's complement */
            } else {
                offset_1FA = *(short *)(base + 0x1FA);
                temp_int = offset_1FA + -160;  /* 0xFF60 */
                *(short *)(base + 0x1EE) = temp_int;
                *(short *)(base + 0x1D0) = offset_1FA;

                /* Clear bit 0x40 */
                bitfield = *(unsigned int *)(base + 0x1C8);
                bitfield &= 0xFFBF;
                *(unsigned int *)(base + 0x1C8) = bitfield;

                /* Check bit 0x100 */
                bitfield = *(unsigned int *)(base + 0x1C8);
                bitfield &= 0x0100;
                if (bitfield != 0) {
                    /* Get values for fixed-point division */
                    offset_1FA = *(short *)(base + 0x1E2);
                    offset_1F8 = *(short *)(base + 0x1E0);
                    param_val = *(short *)(base + 0x1E0);

                    /* Subtract and compare */
                    result_x = offset_1FA - offset_1F8;

                    if (result_x >= 0) {
                        offset_1FA = *(short *)(base + 0x1FA);
                        offset_1F8 = *(short *)(base + 0x1F8);
                        result_y = offset_1FA - offset_1F8;

                        bitfield = *(unsigned int *)(base + 0x1C8);
                        bitfield |= 0x0040;

                        if (result_y < 0) {
                            *(unsigned int *)(base + 0x1C8) = bitfield;
                            result_y = result_y - result_x;
                        }

                        /* Fixed-point division: mult by 0x66666667, extract high */
                        div_val = (int)(((long long)result_y * 0x66666667LL) >> 32);
                        div_hi = div_val >> 7;
                        result_y = div_hi >> 3;
                        temp_div = result_y - (offset_1F8 << 6);

                        /* Safe division check */
                        if (temp_div != 0) {
                            /* Store result */
                            *(short *)(base + 0x1E4) = result_y;
                        } else {
                            /* Trap on divide by zero */
                            __builtin_trap();
                        }

                        /* Additional check for overflow */
                        if (temp_div == -1) {
                            __builtin_trap();
                        } else {
                            stored_val = *(short *)(base + 0x1D2);
                            *(short *)(base + 0x1D2) = result_y;
                        }
                        break;
                    } else {
                        /* Global data operations */
                        offset_1F8 = *(short *)0x800BBA12;
                        offset_1FA = -450;  /* 0xFE3E */
                        *(short *)(base + 0x1D2) = offset_1FA;

                        bitfield = *(unsigned int *)0x800BB330;
                        param_val = *(short *)(base + 0x1FC);
                        param_val &= 0x0001;
                        *(short *)(base + 0x1D4) = param_val;

                        if (param_val == 0) {
                            bitfield = *(unsigned int *)(base + 0x1C8);
                            bitfield &= 0x0100;
                            if (bitfield == 0) {
                                *(unsigned int *)(base + 0x1C8) |= 0x0040;

                                offset_1D0 = *(short *)(base + 0x1D0);
                                offset_1FA = *(short *)0x800BBA10;
                                result_x = offset_1D0 - offset_1FA;

                                if (result_x < 0) {
                                    result_x = result_x - result_x;
                                }

                                div_val = (int)(((long long)result_x * 0x66666667LL) >> 32);
                                *(short *)(base + 0x1E4) = 0x0040;
                                div_hi = div_val >> 7;
                                result_y = div_hi >> 3;
                                stored_val = *(short *)(base + 0x1D2);
                                result_x = result_y + stored_val;
                                *(short *)(base + 0x1D2) = result_x;

                                bitfield = *(unsigned int *)(base + 0x1C8);
                                bitfield &= 0xFEFF;
                                break;
                            }
                        }
                    }
                }
                break;
            }
            break;

        case 2:
            /* CASE_2: Global flag with vector handling */
            /* Set bit 0x08 */
            bitfield = *(unsigned int *)(base + 0x1C8);
            bitfield |= 0x0008;
            *(unsigned int *)(base + 0x1C8) = bitfield;

            /* Load from global and process vectors */
            temp_int = *(unsigned int *)0x800B9F4;

            /* Load vector data and process */
            global_vec_x = *(short *)0x800BBA10;

            param_val = *(short *)(base + 0x0078);
            offset_1F8 = *(short *)0x800BBA10;
            offset_1FA = -1920;  /* 0xF830 */
            *(short *)(sp_buf + 2) = offset_1FA;
            offset_1D0 = *(short *)(base + 0x0064);
            offset_1E0 = *(short *)(sp_buf + 4);
            result_x = offset_1D0 - offset_1E0;

            if (result_x < 0) {
                offset_1E0 = *(short *)(sp_buf + 4);
                offset_1E0 = offset_1E0 + 2400;  /* 0x0960 */
                *(short *)(sp_buf + 4) = offset_1E0;
            }
            offset_1E0 = *(short *)(base + 0x0064);
            offset_1E0 = offset_1E0 + -1920;  /* 0xF6A0 */

            /* Vector store operations */
            stored_val = *(short *)(sp_buf + 4);
            offset_1D0 = *(short *)(base + 0x0064);
            result_y = offset_1D0 >> 7;
            result_x = result_y - stored_val;
            *(short *)(sp_buf + 4) = result_x;

            /* Load and store vector data */
            ext_result_x = *(short *)(sp_buf + 3);
            ext_result_y = *(short *)(sp_buf + 4);
            *(short *)(base + 0x1D3) = ext_result_x;
            *(short *)(base + 0x1D0) = ext_result_y;
            *(short *)(base + 0x1D7) = ext_result_y;
            *(short *)(base + 0x1D4) = ext_result_x;
            break;

        case 3:
            /* CASE_3: Similar to case 2 but different offset */
            offset_1FA = *(short *)0x800BBA14;
            offset_1D0 = *(short *)(base + 0x0064);
            result_x = offset_1FA - offset_1D0;

            if (result_x < 0) {
                offset_1E0 = *(short *)(sp_buf + 4);
                offset_1E0 = offset_1E0 + -1920;  /* 0xF6A0 */
                offset_1E0 = *(short *)(sp_buf + 4);
                offset_1E0 = offset_1E0 + 2400;  /* 0x0960 */
                *(short *)(sp_buf + 4) = offset_1E0;
            }

            param_val = *(short *)(base + 0x0064);
            result_y = param_val >> 5;
            result_x = (result_y & 0x07C0) >> 6;
            stored_val = result_y >> 3;
            result_x = result_x + stored_val;
            stored_val = *(short *)(base + 0x1D0);
            result_y = result_x >> 3;
            result_x = result_y + stored_val;
            *(short *)(base + 0x1D0) = result_x;
            break;

        default:
            break;
    }

    /* Final call to external exit function */
    func_022EE8(base + 0x1E28);
}

// ============================================================================
// FUNCTION 28: s11d_hind_800C9D60 - Complex control processor (11.3 KB)
// ============================================================================
// Pattern: Multi-stage state machine with safe division, vector transformations
// Key characteristics:
// - 6+ states on field at offset 0x1E8
// - 72-byte stack frame (6 saved registers: s0-s5)
// - 8 external function calls with parameter dependencies
// - Safe division pattern used 3 times (with trap handling)
// - Unaligned vector load/store (LWL/LWR sequences)
// - Counter management at 0x1EA, 0x958, 0x95C
void s11d_hind_800C9D60(void *a0)
{
    /* Saved register equivalents */
    unsigned char *base;           /* s2 register */
    unsigned char *ptr_s3;         /* s3 = base + 0x093C */
    unsigned char *ptr_s4;         /* s4 = base + 0x006C */
    unsigned char *ptr_s5;         /* s5 = base + 0x0064 */
    unsigned char *ptr_s0;         /* s0 - additional work pointer */
    unsigned char *ptr_s1;         /* s1 - additional work pointer */

    /* State machine variables */
    short state;
    short state_check;
    short counter_1EA;

    /* Division and arithmetic */
    int global_flag;
    int div_operand_C, div_operand_4, div_operand_10;
    int quotient1, quotient2;
    int temp_mult;
    short result_val;
    int temp_div_result;

    /* Vector operations */
    unsigned char vec_storage[32];  /* Stack storage for vectors */
    short vec_x, vec_y;
    short *vec_ptr;

    /* Bitfield and flags */
    int bitfield_1C8;

    /* Loop variables */
    short loop_offset;
    short loop_count;
    int i;

    /* Temporary values */
    short temp_short;
    int temp_int;
    short loaded_val;

    base = (unsigned char *)a0;

    /* Initialize pointer registers (from assembly prologue) */
    ptr_s3 = base + 0x093C;  /* s3 */
    ptr_s5 = base + 0x0064;  /* s5 */
    ptr_s4 = base + 0x006C;  /* s4 */

    /* Read state from offset 0x1E8 */
    state = *(short *)(base + 0x1E8);

    /* Early exit if state <= 0 (assembly line 19: blez) */
    if (state <= 0) {
        goto state_2_plus_check;
    }

    /* ===== STATE 0-1 HANDLER ===== */
    /* Safe Division Pattern 1 (assembly lines 21-58) */

    /* Load global flag */
    global_flag = *(int *)0x800BB330;

    /* Division 1: global_flag / value_at_0x000C */
    div_operand_C = *(int *)(base + 0x000C);

    /* Safe division with trap handling */
    if (div_operand_C == 0) {
        __builtin_trap();  /* break 7 */
    }
    if (div_operand_C == -1 && global_flag == (int)0x80000000) {
        __builtin_trap();  /* break 6 */
    }
    quotient1 = global_flag / div_operand_C;

    /* Division 2: quotient1 / 0x1000 */
    if (0x1000 == 0) {
        __builtin_trap();
    }
    quotient2 = quotient1 / 0x1000;

    /* Multiply: quotient1 * quotient2 */
    temp_mult = quotient1 * quotient2;

    /* Call external function */
    func_024942();

    /* Load value from offset 0x0018 and mult with quotient2 */
    result_val = *(short *)(base + 0x0018);
    temp_mult = result_val * quotient2;

    /* Extract result with sign extension */
    if (temp_mult < 0) {
        temp_mult = temp_mult + 0x0FFF;
    }
    result_val = temp_mult >> 12;  /* sra by 12 */

    /* Store result to ptr_s3 + 0x0004 */
    *(short *)(ptr_s3 + 0x0004) = result_val;

state_2_plus_check:
    /* Re-read state */
    state = *(short *)(base + 0x1E8);

    /* Check if state < 2 (assembly line 66: slti v0, v0, 2) */
    state_check = (state < 2) ? 1 : 0;
    if (state_check != 0) {
        goto exit_function;
    }

    /* ===== STATE 2+ HANDLER ===== */
    /* Safe Division Pattern 2 (assembly lines 68-100) */

    /* Load global flag again */
    global_flag = *(int *)0x800BB330;

    /* Division 1: global_flag / value_at_0x0004 */
    div_operand_4 = *(int *)(base + 0x0004);

    /* Safe division with trap handling */
    if (div_operand_4 == 0) {
        __builtin_trap();
    }
    if (div_operand_4 == -1 && global_flag == (int)0x80000000) {
        __builtin_trap();
    }
    quotient1 = global_flag / div_operand_4;

    /* Division 2: quotient1 / 0x1000 */
    quotient2 = quotient1 / 0x1000;

    /* Multiply: quotient1 * quotient2 */
    temp_mult = quotient1 * quotient2;

    /* Call external function */
    func_024942();

    /* ===== Additional Multiplication Block (Assembly lines 101-115) ===== */
    /* Load value from offset 0x0010 */
    div_operand_10 = *(int *)(base + 0x0010);

    /* Subtract from quotient2 and multiply */
    temp_div_result = quotient2 - div_operand_10;
    temp_mult = temp_div_result * quotient1;

    /* Call func_024942 again */
    func_024942();

    /* Load from offset 0x0010, subtract quotient2, multiply */
    loaded_val = *(short *)(base + 0x0010);
    temp_mult = (loaded_val - quotient2) * quotient1;

    /* Sign extension */
    if (temp_mult < 0) {
        temp_mult = temp_mult + 0x0FFF;
    }
    result_val = temp_mult >> 12;

    /* Store result to ptr_s3 + 0x0002 */
    *(short *)(ptr_s3 + 0x0002) = result_val;

    /* ===== State Re-Check and Dispatch (Assembly lines 116-129) ===== */
    state = *(short *)(base + 0x1E8);

    /* Check if state == 1 */
    if (state == 1) {
        goto state_1_specific;
    }

    /* Check if state < 2 */
    if (state < 2) {
        goto exit_function;
    }

    /* Check if state == 2 */
    if (state != 2) {
        goto state_3_plus;
    }

    /* ===== Counter Check at 0x1EA (Assembly line 130) ===== */
    counter_1EA = *(short *)(base + 0x1EA);
    if (counter_1EA != 0) {
        goto skip_vector_transform;
    }

    /* ===== Vector Transformation Block (Assembly lines 133-168) ===== */
    /* Load global vector from 0x800BBA10 using unaligned pattern */
    vec_ptr = (short *)0x800BBA10;
    vec_x = vec_ptr[0];
    vec_y = vec_ptr[1];

    /* Store to stack */
    *(short *)(vec_storage + 0x20) = vec_x;
    *(short *)(vec_storage + 0x22) = vec_y;

    /* Alternative: Check bit 0 of field 0x1C8 */
    bitfield_1C8 = *(int *)(base + 0x1C8);
    if (bitfield_1C8 & 0x0001) {
        /* Load from stored location at 0x0910 instead */
        vec_x = *(short *)(base + 0x0910);
        vec_y = *(short *)(base + 0x0912);
        *(short *)(vec_storage + 0x20) = vec_x;
        *(short *)(vec_storage + 0x22) = vec_y;
    }

    /* ===== Five Sequential External Function Calls (Assembly lines 169-187) ===== */
    /* Call 1: func_005B50 with stack parameters */
    func_005B50((void *)(vec_storage + 0x20), (void *)(vec_storage + 0x18));

    /* Call 2: func_005BBE */
    func_005BBE((void *)(vec_storage + 0x10));

    /* Call 3: func_005C13 with value from stack */
    temp_short = *(short *)(vec_storage + 0x12);
    func_005C13(temp_short);

    /* Call 4: func_005B60 - store result */
    temp_short = *(short *)(vec_storage + 0x12);
    temp_int = temp_short + quotient2;
    *(int *)(base + 0x0960) = temp_int;
    func_005B60((void *)(vec_storage + 0x10));

    /* Call 5: func_005B77 with offset */
    loop_offset = *(short *)(base + 0x1F0);
    func_005B77((void *)(vec_storage + 0x10), (void *)(ptr_s4 + 0x968));

    /* ===== Counter Update (Assembly line 188) ===== */
    *(short *)(base + 0x1EA) = 1;

skip_vector_transform:
    /* ===== Loop Offset Calculation (Assembly lines 190-198) ===== */
    loop_offset = *(short *)(base + 0x1F0);
    if (loop_offset < 0) {
        loop_offset = loop_offset + 7;
    }
    loop_count = loop_offset >> 3;  /* Divide by 8 */

    /* ===== Loop with func_325DB (Assembly line 197) ===== */
    for (i = 0; i < loop_count; i++) {
        func_325DB(loop_count, ptr_s5);
    }

state_1_specific:
    /* State 1 specific handling placeholder */
    /* (Details in Part 2) */
    goto exit_function;

state_3_plus:
    /* State 3+ handling placeholder */
    /* (Details in Part 2) */

exit_function:
    /* Function epilogue */
    return;
}
