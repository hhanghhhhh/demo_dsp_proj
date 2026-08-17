//************************************************//
//FILE NAME:        testbench.V
//DESCRIPTION:      testbench
//AUTHOR:           hehang
//DATE:             07/10/23
//LOG:              testbench
//************************************************//

`timescale 1ns / 100ps
`define sys_clk_period_ns 40.0

module knob_detect_tb();

localparam      POSITION1    =   2'b00;
localparam      POSITION2    =   2'b10;
localparam      POSITION3    =   2'b11;
localparam      POSITION4    =   2'b01;

//reg
reg         sys_clk         ;
reg         sys_rst         ;
reg         read_flag       ;
reg  [1:0]  position        ;

reg         pushbutton      ;

//sys_clk
initial sys_clk = 0;
always #(`sys_clk_period_ns / 2) sys_clk = ~sys_clk;

//sys_rst
initial begin
    sys_rst = 0;
    #100 sys_rst = 1;
end


//Unit Instantiate
knob_detect inst13
(   
    .sys_clk             (sys_clk   ),  // 25M
    .sys_rst             (sys_rst   ),
    .read_flag           (read_flag ),
    .knob_outa           (position[1]),
    .knob_outb           (position[0]),
    .pushbutton          (pushbutton),
    .value               ()
);


//Stimulus process
initial begin
    read_flag = 1'b0;
    position = POSITION1;
    pushbutton = 1'b0;
    #10000000
    pushbutton = 1'b1;
    #5000000
    pushbutton = 1'b0;
    #10000000
    pushbutton = 1'b1;
    #20000000
    pushbutton = 1'b0;
    #5000000
    read_flag = 1;
    #1000000
    read_flag = 0;
    #1000000
    position = POSITION2;
    #1000000
    position = POSITION3;
    #1000000
    position = POSITION4;
    #1000000
    position = POSITION1;
    #1000000
    position = POSITION2;
    #1000000
    position = POSITION1;
    #1000000
    position = POSITION4;
    #1000000
    position = POSITION3;
    #1000000
    read_flag = 1;
    #1000000
    read_flag = 0;
    #1000000
    position = POSITION2;
    #1000000
    position = POSITION1;
    #1000000
    read_flag = 1;
    #1000000
    read_flag = 0;
    #1000000
    position = POSITION2;
    #1000000
    position = POSITION3;
end







endmodule