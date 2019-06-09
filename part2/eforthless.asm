%include "lib.inc"
%include "macro.inc"
global _start

%define pc r15
%define w r14
%define rstack r13

section .bss
resq 1023
rstack_start: resq 1
input_buf: resb 1024
user_dict: resq 65536
user_mem: resq 65536

section .rodata
msg_nsw: db ": no such word", 10, 0
msg_stack: db "Stack top reached bottom", 0

section .text
%include "words.inc"

section .data
dict_head: dq user_dict
dp: dq user_mem
state: dq 0
last_word: dq _lw
stack_base: dq 0

section .text

next:
    mov w, [pc]
    add pc, 8
    jmp [w]
_start: jmp i_init

