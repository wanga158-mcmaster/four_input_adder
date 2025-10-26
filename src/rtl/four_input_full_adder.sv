`include "d_register.sv"
`include "bit_count_lut.sv"

module four_input_full_adder (
    input logic a0_in,
    input logic a1_in,
    input logic a2_in,
    input logic a3_in,
    input logic c0_in,
    input logic c1_in,

    output logic b_out,
    output logic c0_out,
    output logic c1_out
);

    logic [5:0] p_in;  // packed input
    assign p_in = {a0_in, a1_in, a2_in, a3_in, c0_in, c1_in};

    assign b_out = ^p_in; // the first bit is set if the number of set bits in input is 1 -> use XOR reduction

    logic set_2, set_3, set_6;

    check_2 chk_2 (
        .d_in (p_in),
        .d_out(set_2)
    );
    check_3 chk_3 (
        .d_in (p_in),
        .d_out(set_3)
    );
    check_6 chk_6 (
        .d_in (p_in),
        .d_out(set_6)
    );

    assign c0_out = (set_2 | set_3 | set_6); // second bit is set if sum is 2, 3, or 6

    logic set_4, set_5;

    check_4 chk_4 (
        .d_in (p_in),
        .d_out(set_4)
    );
    check_5 chk_5 (
        .d_in (p_in),
        .d_out(set_5)
    );

    assign c1_out = (set_4 | set_5 | set_6); // third bit is set if sum > 3

endmodule
;
