colon "interpreter", interpreter
.start:
    dq xt_dup, xt_find_word

    dq xt_dup
    branch0 .num
    dq xt_cfa

    dq xt_state, xt_fetch
    branch0 .interpret

    dq xt_dup
    dq xt_lit, 1, xt_minus, xt_fetch_char, xt_not
    branch0 .interpret

    dq xt_swap, xt_drop
    dq xt_comma
    dq xt_exit

    .interpret:
    dq xt_swap, xt_drop
    dq xt_execute
    dq xt_exit

    .num:
    dq xt_drop
    dq xt_dup
    dq xt_number
    branch0 .no_such_word

    dq xt_swap
    dq xt_drop
    dq xt_state
    dq xt_fetch
    branch0 .exit

    dq xt_lit, xt_lit, xt_comma
    dq xt_comma

    dq xt_exit                                      

    .no_such_word:
    dq xt_drop, xt_prints
    dq xt_lit, msg_nsw, xt_prints

    .exit:
    dq xt_exit

