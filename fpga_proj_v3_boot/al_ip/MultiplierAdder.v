// Verilog netlist created by Tang Dynasty v5.6.56362
// Fri Apr 21 15:32:06 2023

`timescale 1ns / 1ps
module Adder
  (
  a,
  clk,
  z,
  p
  );

  input [53:0] a;
  input clk;
  input [53:0] z;
  output [53:0] p;

  wire [53:0] dsp0_syn_1;
  wire [26:0] dsp0_syn_227;
  wire [26:0] dsp0_syn_281;
  wire [53:0] dsp0_syn_55;

  PH1_PHY_DSPREG #(
    .CEMUX("SIG"),
    .CLKMUX("SIG"),
    .RSTMODE("ASYNC"),
    .RSTMUX("SIG"),
    .WIDTH(54))
    \dsp0/POU_p_reg  (
    .ce(1'b1),
    .clk(clk),
    .d(dsp0_syn_1),
    .rst(1'b0),
    .q(p));
  PH1_PHY_DSPREG #(
    .CEMUX("SIG"),
    .CLKMUX("SIG"),
    .RSTMODE("ASYNC"),
    .RSTMUX("SIG"),
    .WIDTH(54))
    \dsp0/Z_reg  (
    .ce(1'b1),
    .clk(clk),
    .d(z),
    .rst(1'b0),
    .q(dsp0_syn_55));
  PH1_PHY_DSPTADD #(
    .CI_INVERT("NO"),
    .INV_OPCTRL(4'b0000),
    .RND_CONST(54'b0),
    .USE_OVERFLOW("S53"),
    .X1_EXTEND("NO"),
    .Y1_ROUND("NO"),
    .Z1_SHIFT("NO"))
    \dsp0/_tadd  (
    .ci(1'b0),
    .ci_special(1'b0),
    .opctrl(9'b001100000),
    .x0({dsp0_syn_227,dsp0_syn_281}),
    .x1_special({dsp0_syn_227,dsp0_syn_281}),
    .y0(dsp0_syn_55),
    .y1_special(dsp0_syn_55),
    .z0(p),
    .z1_special(p),
    .sum(dsp0_syn_1));
  PH1_PHY_DSPMREG #(
    .CEMUX("SIG"),
    .CLKMUX("SIG"),
    .DYNAMIC_DATA("Q"),
    .RSTMODE("ASYNC"),
    .RSTMUX("SIG"),
    .WIDTH(27))
    \dsp0/xa_mreg  (
    .ce(1'b1),
    .clk(clk),
    .d(a[53:27]),
    .opctrl(1'b1),
    .rst(1'b0),
    .dynamic_q(dsp0_syn_227));
  PH1_PHY_DSPMREG #(
    .CEMUX("SIG"),
    .CLKMUX("SIG"),
    .DYNAMIC_DATA("Q"),
    .RSTMODE("ASYNC"),
    .RSTMUX("SIG"),
    .WIDTH(27))
    \dsp0/xb_mreg  (
    .ce(1'b1),
    .clk(clk),
    .d(a[26:0]),
    .opctrl(1'b1),
    .rst(1'b0),
    .dynamic_q(dsp0_syn_281));

  // synthesis translate_off
  glbl glbl();
  always @(*) begin
    glbl.gsr <= PH1_PHY_GSR.gsr;
    glbl.gsrn <= PH1_PHY_GSR.gsrn;
    glbl.done_gwe <= PH1_PHY_GSR.done_gwe;
    glbl.usr_gsrn_en <= PH1_PHY_GSR.usr_gsrn_en;
  end
  // synthesis translate_on

endmodule 

