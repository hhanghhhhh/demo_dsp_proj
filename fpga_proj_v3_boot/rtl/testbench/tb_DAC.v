//************************************************//
//FILE NAME:        testbench.V
//DESCRIPTION:      testbench
//AUTHOR:           hehang
//DATE:             07/10/23
//LOG:              testbench
//************************************************//

`timescale 1ns / 100ps

module HH_tb();

//变量
reg sys_clk, sys_rst;
reg en;


//Clock process
parameter PERIOD = 40;   //25MHz
always #(PERIOD/2) sys_clk = ~sys_clk;

reg[11:0] senddata;
//Unit Instantiate
//例化模块
MCP4725_ctrl  inst_ctrl
(
.sys_clk       (sys_clk),
.sys_rst       (sys_rst),
.send_data     (senddata),

.iic_scl       (),
.iic_sda       ()
);



//Stimulus process
initial begin
    sys_clk = 0;
    sys_rst = 0;
    en = 0;
    #100 sys_rst = 1; //reset 信号
    senddata = 3;
    #400000 senddata = 5;
end








endmodule