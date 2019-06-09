native "drop", drop
    add rsp, 8
    jmp next

native "swap", swap
    pop rax
    pop rdx
    push rax
    push rdx
    jmp next

native "dup", dup
    push qword [rsp]
    jmp next

native "rot", rot
    pop rax
    pop rdx
    pop rcx
    push rax
    push rcx
    push rdx
    jmp next

native "+", plus
    pop rax
    pop rdx
    add rax, rdx
    push rax
    jmp next

native "*", mult
    pop rax
    pop rdx
    imul rdx
    push rax
    jmp next

native "/", divd
    pop rcx
    pop rax
    cqo
    idiv rcx
    push rax
    jmp next

native "%", mod
    pop rcx
    pop rax
    cqo
    idiv rcx
    push rdx
    jmp next

native "-", minus
    pop rax
    pop rdx
    sub rdx, rax
    push rdx
    jmp next

native "<", less
    pop rdx
    pop rax
    cmp rax, rdx
    setl al
    movzx rax, al
    push rax
    jmp next

native "not", not 
    pop rax
    test rax, rax
    setz al
    movzx rax, al
    push rax
    jmp next

native "=", equals
    pop rax
    pop rdx
    cmp rax, rdx
    sete al
    movzx rax, al
    push rax
    jmp next

native "land", land
    pop rax
    pop rdx
    test rax, rax
    jz .false
    push rdx
    jmp next
.false:
    push rax
    jmp next

native "lor", lor
    pop rax
    pop rdx
    test rax, rax
    jnz .true
    push rdx
    jmp next
.true:
    push rax
    jmp next

native "and", and
    pop rax
    pop rdx
    and rax, rdx
    push rax
    jmp next

native "or", or
    pop rax
    pop rdx
    or rax, rdx
    push rax
    jmp next

colon "'", tick, 1
    dq xt_inbuf
    dq xt_word
    dq xt_drop
    dq xt_inbuf
    dq xt_find_word, xt_dup
    branch0 .no_such_word
    dq xt_cfa
    dq xt_exit
    .no_such_word:
    dq xt_inbuf, xt_prints
    dq xt_lit, msg_nsw, xt_prints
    dq xt_exit
    
colon "count", count
    dq xt_dup
.loop:
    dq xt_dup
    dq xt_fetch_char
    branch0 .exit
    dq xt_lit, 1, xt_plus
    branch .loop
.exit:
    dq xt_swap, xt_minus, xt_exit

native ".", dot
    cmp rsp, [stack_base]
    je .stack_end
    pop rdi
    call print_int
    call print_newline
    jmp next
    .stack_end:
    mov rdi, msg_stack
    call print_string
    call print_newline
    jmp next

native ".S", show_stack
    mov rdx, rsp
    .loop:
    mov rdi, [rdx]
    push rdx
    call print_int
    call print_newline
    pop rdx
    add rdx, 8
    cmp rdx, [stack_base]
    jl .loop
    jmp next

native "init", init
    mov qword [state], 0
    mov rstack, rstack_start
    mov pc, interpreter_stub
    mov [stack_base], rsp
    jmp next

native "docol", docol
    sub rstack, 8
    mov [rstack], pc
    add w, 8
    mov pc, w
    jmp next

native "exit", exit
    rpop pc
    jmp next

native "r>", from_r
    rpop rax
    push rax
    jmp next

native ">r", to_r
    pop rax
    rpush rax
    jmp next

native "r@", r_fetch
    push qword[rstack]
    jmp next

native "find_word", find_word
    mov rax, [state]
    mov rsi, [last_word]
    test rax, rax
    jz .loop
    mov rsi, [rsi]
    .loop:
        mov rdi, [rsp]
        push rsi
        add rsi, 9
        call string_equals
        pop rsi
        test rax, rax
        jnz .found

        mov rsi, [rsi]
        test rsi, rsi
        jnz .loop
    .not_found:
        mov qword [rsp], 0
        jmp next
    .found:
        mov [rsp], rsi
        jmp next

native "cfa", cfa
    pop rsi
    add rsi, 9
    .loop:
    mov al, [rsi]
    test al, al
    jz .end
    inc rsi
    jmp .loop

    .end:
    add rsi, 2
    push rsi
    jmp next


native "emit", emit
    pop rdi
    call print_char
    jmp next

native "word", word
    pop rdi
    call read_word
    push rdx
    jmp next

native "number", number
    pop rdi
    call parse_int
    push rax
    push rdx
    jmp next

native "prints", prints
    pop rdi
    call print_string
    jmp next

native "bye", bye
    mov rax, 60
    xor rdi, rdi
    syscall

native "branch", branch
    mov pc, [pc]
    jmp next

native "0branch", branch0
    pop rax
    test rax, rax
    jnz .skip
    mov pc, [pc]
    jmp next
.skip:
    add pc, 8
    jmp next

native "lit", lit
    push qword [pc]
    add pc, 8
    jmp next

const inbuf, input_buf

const in_fd, in_fd

const dp, dp

const last_word, last_word

const state, state

const dict_head, [dict_head]

native "execute", execute
    pop rax
    mov w, rax
    jmp [rax]


native "@", fetch
    pop rax
    push qword[rax]
    jmp next

native "!", write
    pop rax
    pop rdx
    mov [rax], rdx
    jmp next

native "c!", write_char
    pop rax
    pop rdx
    mov [rax], dl
    jmp next

native "c@", fetch_char
    pop rax
    movzx rax, byte [rax]
    push rax
    jmp next

native ",", comma
    mov rax, [dict_head]
    pop qword [rax]
    add qword [dict_head], 8
    jmp next

native "c,", char_comma
    mov rax, [dict_head]
    pop rdx
    mov [rax], dl
    add qword[dict_head], 1
    jmp next


native "create", create
    ; link
    mov rcx, [last_word]
    mov rsi, [dict_head]
    mov [rsi], rcx
    mov [last_word], rsi
    add rsi, 8
    mov byte [rsi], 0
    inc rsi

    ; name
    pop rdi
    push rsi
    call string_copy
    pop rsi
    push rsi
    mov rdi, rsi
    call string_length
    pop rsi
    add rsi, rax
    ; flags
    inc rsi
    pop rax
    mov [rsi], al
    inc rsi

    mov [dict_head], rsi
    jmp next

colon ":", colon
    .restart:
    dq xt_inbuf, xt_word
    branch0 .restart
    dq xt_lit, 0, xt_inbuf, xt_create
    dq xt_lit, 1, xt_state, xt_write
    dq xt_lit, i_docol, xt_comma
    dq xt_exit

colon ";", semicolon, 1
    dq xt_lit, 0, xt_state, xt_write
    dq xt_lit, xt_exit, xt_comma
    dq xt_exit

%include "interpreter.inc"

section .rodata
interpreter_stub:
    dq xt_lit, 0    ; stdin
    dq xt_interpret_fd
    dq xt_bye

colon "interpret-fd", interpret_fd
    dq xt_lit, in_fd, xt_fetch, xt_to_r
    dq xt_lit, in_fd
    dq xt_write
    .loop:
    dq xt_inbuf, xt_word
    branch0 .end_of_input

    dq xt_inbuf
    dq xt_interpreter

    branch .loop

    .end_of_input:
    dq xt_from_r
    dq xt_lit, in_fd, xt_write
    dq xt_exit

