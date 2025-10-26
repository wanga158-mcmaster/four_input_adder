`include "four_input_full_adder.sv"

module four_input_adder #(
    parameter integer _W = 32
) (
    input logic clk,
    input logic rst_n,

    input logic en,

    input logic [_W - 1:0] a0,
    input logic [_W - 1:0] a1,
    input logic [_W - 1:0] a2,
    input logic [_W - 1:0] a3,

    output logic [_W - 1:0] b,
    output logic O_F,  // overflow flag
    
    output logic vld
);


    logic [_W:0] c0;
    logic [_W + 1:0] c1;

    logic [_W - 1:0] b_t;
    logic O_F_t;

    assign c0[0] = 0;
    assign c1[0] = 0;
    assign c1[1] = 0;
    
    genvar i;

    generate
        for (i = 0; i < _W; i = i + 1) begin : adder_loop
            four_input_full_adder f_fa (
                .a0_in(a0[i]),
                .a1_in(a1[i]),
                .a2_in(a2[i]),
                .a3_in(a3[i]),
                .c0_in(c0[i]),
                .c1_in(c1[i]),

                .b_out (b_t[i]),
                .c0_out(c0[i+1]),
                .c1_out(c1[i+2])
            );
        end
    endgenerate

    assign O_F_t = (c0[_W] | c1[_W] | c1[_W+1]);

    d_register #(
        ._W(_W)
    ) b_s (
        .clk  (clk),
        .rst_n(rst_n),

        .en(en),
        .flush(0),

        .din (b_t),
        .dout(b)
    );
    d_ff vld_s(
        .clk  (clk),
        .rst_n(rst_n),

        .en(1),
        .flush(0),

        .din (en),
        .dout(vld)
    );
    d_ff O_F_s (
        .clk  (clk),
        .rst_n(rst_n),

        .en(en),
        .flush(0),

        .din (O_F_t),
        .dout(O_F)
    );

endmodule
;
