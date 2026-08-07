section .data
    F255        dd  255.0                  

section .text
bits 64
default rel                                 

global imgCvtGrayFloatToInt

imgCvtGrayFloatToInt:
    movsxd  r10, ecx                        
    movsxd  rax, edx                        
    imul    r10, rax                        
    xor     r11, r11                        
    movss   xmm1, [F255]                    
    test    r10, r10
    jle     .done                           

.loop:
    movss   xmm0, [r8 + r11*4]              
    mulss   xmm0, xmm1                      
    cvtss2si eax, xmm0                      
                                                                                    
    mov     [r9 + r11], al                  

    inc     r11                             
    cmp     r11, r10
    jl      .loop

.done:
    ret