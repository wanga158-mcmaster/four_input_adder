module d_ff #(
    parameter logic rst_val = 0
) (
    input logic clk,
    input logic rst_n,

    input logic en,
    input logic flush,

    input  logic din,
    output logic dout
);

    logic din_t, dout_t;
    assign din_t = (~flush & din);
    assign dout  = (~flush & dout_t);

    always_ff @(posedge clk) begin
        if (~rst_n) begin
            dout_t <= rst_val;
        end else if (en) begin
            dout_t <= din_t;
        end
    end

endmodule
;

module d_register #(  // synchronous reset d register with flush and enable
    parameter integer _W = 32,
    parameter logic [_W - 1:0] rst_vect = '0
) (
    input logic clk,
    input logic rst_n,

    input logic en,
    input logic flush,

    input  logic [_W - 1:0] din,
    output logic [_W - 1:0] dout
);

    always_ff @(posedge clk) begin
        if (~rst_n) begin
            dout <= rst_vect;
        end else if (flush) begin
            dout <= '0;
        end else if (en) begin
            dout <= din;
        end
    end

endmodule
;
