//************************************************//
//FILE NAME:        testbench.V
//DESCRIPTION:      testbench
//AUTHOR:           hehang
//DATE:             07/10/23
//LOG:              testbench
//************************************************//

`timescale 1ns / 100ps
`define sys_clk_period_ns 40.0

module key_detect_tb();

//reg
reg         sys_clk         ;
reg         sys_rst         ;
reg [4:0]   key_row         ;
wire [4:0]  key_col         ;
reg [2:0]   row;
reg [2:0]   col;
//sys_clk
initial sys_clk = 0;
always #(`sys_clk_period_ns / 2) sys_clk = ~sys_clk;

//sys_rst
initial begin
    sys_rst = 0;
    #100 sys_rst = 1;
end


//Unit Instantiate
key_detect inst123
(   
    .sys_clk             (sys_clk),   // 25M
    .sys_rst             (sys_rst),
    .key_row_i           (key_row),   // 行
    .key_col_o           (key_col),   // 列
    .update_pulse        (),
    .key_value           ()
);


//Stimulus process
initial begin
    #20000000;
    row = 1;
    col = 3;
    #20000000;
    row = 0;
    col = 0;
    #20000000;
    row = 2;
    col = 4;
    #20000000;
    row = 0;
    col = 0;
    #20000000;
    row = 3;
    col = 3;
    #20000000;
    row = 0;
    col = 0;
end



always@(posedge sys_clk or negedge sys_rst) begin
    if(!sys_rst) begin
    //initial regs
        key_row = 5'b11111;
    end
    else begin
        if(col == 0) begin
            key_row = 5'b11111;
        end
        else begin
            if(((~key_col) >> (col - 1)) & 5'b00001 == 5'b00001) begin
                key_row[row - 1] = 1'b0;
            end
            else begin
                key_row = 5'b11111;
            end
        end
    end
end




endmodule