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



//Clock process
parameter PERIOD = 40;   //25MHz
always #(PERIOD/2) sys_clk = ~sys_clk;


//Unit Instantiate
//例化模块
//glbl glbl(); //仿真IP时必需加，其余情况不用加
//PH1_PHY_GSR PH1_PHY_GSR();


//Stimulus process
initial begin
    sys_clk = 0;
    sys_rst = 0;
    #100 sys_rst = 1; //reset 信号

end

reg sw_ctrl;
initial begin
    sw_ctrl = 0;
    #500 sw_ctrl = 1;
    #60000000 sw_ctrl = 0;
    #60000000 sw_ctrl = 1;
    #60000000 sw_ctrl = 0;
end


ADJUST_RALAY  hh_inst1
(   
    .sys_clk(sys_clk),           //系统时钟  25M
    .sys_rst(sys_rst),           //复位信号，低有效
    .sw_ctrl(sw_ctrl),           //切换信号
    .pluse_on(),
    .pluse_off()
);





endmodule