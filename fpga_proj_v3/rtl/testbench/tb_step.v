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


//Unit Instantiate
//例化模块
DAC_STEP_AUTO hh111
(
    .sys_clk(sys_clk),           //系统时钟  25M
    .sys_rst(sys_rst),           //复位信号，低有效
    .en(en),                //模块使能信号，高有效
    .dacDATA(16'd60000),           //总电流
    .dac_spi_en_input(1'b1),  //
    
    .dac_per_step(16'd2000),      //每次增加量，AD值

    .dac_en_all(),    
    .stepDATA() //step输出          //最后一个不需要逗号
);



DAC_STEP_test_1 hh222
(
    .sys_clk(sys_clk),           //系统时钟  25M
    .sys_rst(sys_rst),           //复位信号，低有效
    .en(en),                //模块使能信号，高有效
    .dacDATA(16'd60000),
    .dac_spi_en_input(1'b1),     //
    .STEP1_CNT(16'd1150),                //
    .STEP2_CNT(16'd1400),                //
    .STEP3_CNT(16'd1750),                //
    .STEP4_CNT(16'd2000),                //

    .dac_en_all(),    
    .stepDATA() //step输出          //最后一个不需要逗号
);




//Stimulus process
initial begin
    sys_clk = 0;
    sys_rst = 0;
    en = 0;
    #100 sys_rst = 1; //reset 信号
    
    #100 en = 1;
    #150000 en = 0;
    #100 en =1;
end








endmodule