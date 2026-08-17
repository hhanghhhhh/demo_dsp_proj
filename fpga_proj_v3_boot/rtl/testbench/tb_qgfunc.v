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
wire  tirgger, distirgger;

//Clock process
parameter PERIOD = 40;   //25MHz
always #(PERIOD/2) sys_clk = ~sys_clk;


//Unit Instantiate
//例化模块
QGFUNCTION_TRIGGEROFF  hh_test
(
    .sys_clk(sys_clk),           //系统时钟  25M
    .sys_rst(sys_rst),           //复位信号，低有效
    .en(en),                //模块使能信号，高有效
    .data_current(16'd200),      //采样值
    .CurProAD_in(16'd100),          //保护电流门槛
    .offtime_in(16'd1),           //拉下来的时间

    .disTrigger(distirgger)
);


//Stimulus process
initial begin
    sys_clk = 0;
    sys_rst = 0;
    en = 0;
    #100 sys_rst = 1; //reset 信号
    
    #1000 en = 1;
    #1200000 en = 0;

end



assign tirgger = en & (!distirgger);




endmodule