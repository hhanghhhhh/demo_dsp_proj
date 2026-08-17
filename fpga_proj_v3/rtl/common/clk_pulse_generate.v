/************************************************
//FILE NAME:        clk_pulse_generate.V
//DESCRIPTION:      clk_pulse_generate
//AUTHOR:           hehang
//DATE:             04/02/25
//LOG:              生成各个时钟的单bit脉冲信号
************************************************/

//************** Module Define *******************//
module clk_pulse_generate
#(
    parameter   DIV             =       100
)
(   
    input                       sys_clk             ,  //系统时钟
    input                       sys_rst             ,  //复位信号，低有效
    output  reg                 clk_pulse           
);

//*************** Parameter & Internal Register ****//
// parameter


// reg
reg	[16:0]  clk_cnt		;

//*************** Main Code ***********************//   
//产生1M频率脉冲信号
always@(posedge sys_clk or negedge sys_rst) begin
    if(!sys_rst) begin
    //initial regs
		clk_cnt     <= 16'd0;
		clk_pulse   <= 1'b0;
    end
	else if(clk_cnt >= DIV - 1'b1)begin
		clk_cnt     <= 16'd0;
		clk_pulse   <= 1'b1;
	end
	else begin
		clk_cnt     <= clk_cnt + 1'b1;
		clk_pulse   <= 1'b0;
    end
end





    
endmodule       

//*************** END OF LINE ********************//