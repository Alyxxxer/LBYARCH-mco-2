section .data
    F255        dd  255.0                   ; single-precision scale factor

section .text
bits 64
default rel                                 ; RIP-relative addressing

global imgCvtGrayFloatToInt

imgCvtGrayFloatToInt:
    movsxd  r10, ecx                        ; r10 = height  (sign-extend 32->64)
    movsxd  rax, edx                        ; rax = width
    imul    r10, rax                        ; r10 = total pixels = height*width
    xor     r11, r11                        ; r11 = pixel index i = 0
    movss   xmm1, [F255]                    ; SCALAR SIMD: xmm1 = 255.0f
    test    r10, r10
    jle     .done                           ; guard against empty image

.loop:
    movss   xmm0, [r8 + r11*4]              ; SCALAR SIMD: xmm0 = fImg[i]
    mulss   xmm0, xmm1                      ; SCALAR SIMD: xmm0 = fImg[i] * 255.0f
    cvtss2si eax, xmm0                      ; SCALAR SIMD convert: float -> int32
                                            ; uses MXCSR default rounding mode
                                            ; = round-to-nearest-even
    mov     [r9 + r11], al                  ; iImg[i] = (unsigned char) result

    inc     r11                             ; i++
    cmp     r11, r10
    jl      .loop

.done:
    ret