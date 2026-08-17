// Verilog netlist created by Tang Dynasty v5.6.56362
// Mon Apr 17 13:16:49 2023

`timescale 1ns / 1ps
module multiplier
  (
  a,
  clk,
  y,
  p
  );

  input [26:0] a;
  input clk;
  input [17:0] y;
  output [44:0] p;

  wire [44:0] dsp1_syn_1;
  wire [26:0] dsp1_syn_154;
  wire [17:0] dsp1_syn_199;

  PH1_PHY_DSPREG #(
    .CEMUX("SIG"),
    .CLKMUX("SIG"),
    .RSTMODE("ASYNC"),
    .RSTMUX("SIG"),
    .WIDTH(45))
    \dsp1/M_reg  (
    .ce(1'b1),
    .clk(clk),
    .d(dsp1_syn_1),
    .rst(1'b0),
    .q(p));
  PH1_PHY_DSPMULT \dsp1/_mult  (
    .opctrl(2'b11),
    .x(dsp1_syn_154),
    .y(dsp1_syn_199),
    .p(dsp1_syn_1));
  PH1_PHY_DSPMREG #(
    .CEMUX("SIG"),
    .CLKMUX("SIG"),
    .DYNAMIC_DATA("Q"),
    .RSTMODE("ASYNC"),
    .RSTMUX("SIG"),
    .WIDTH(27))
    \dsp1/xa_mreg  (
    .ce(1'b1),
    .clk(clk),
    .d(a),
    .opctrl(1'b1),
    .rst(1'b0),
    .dynamic_q(dsp1_syn_154));
  PH1_PHY_DSPMREG #(
    .CEMUX("SIG"),
    .CLKMUX("SIG"),
    .DYNAMIC_DATA("Q"),
    .RSTMODE("ASYNC"),
    .RSTMUX("SIG"),
    .WIDTH(18))
    \dsp1/y_mreg  (
    .ce(1'b1),
    .clk(clk),
    .d(y),
    .opctrl(1'b1),
    .rst(1'b0),
    .dynamic_q(dsp1_syn_199));

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

