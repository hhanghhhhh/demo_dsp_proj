/************************************************
//FILE NAME:        async_double_flopping.V
//DESCRIPTION:      async_double_flopping
//AUTHOR:           hehang
//DATE:             04/01/25
//LOG:              
************************************************/

//************** Module Define *******************//
module async_double_flopping
#(
    parameter       DATA_WIDTH          =               16
)
(   
    input                               clk_data_out    ,  
    input                               sys_rst         ,
    input           [DATA_WIDTH-1:0]    data_in         ,
    output  wire    [DATA_WIDTH-1:0]    data_out              
);

//*************** Parameter & Internal Register ****//
// parameter


// reg
reg    [DATA_WIDTH-1:0]    data_in_r1              ;
reg    [DATA_WIDTH-1:0]    data_in_r2              ;

//*************** Main Code ***********************//   
assign  data_out    =   data_in_r2;

always@(posedge clk_data_out or negedge sys_rst) begin
    if(!sys_rst) begin
    //initial regs
        data_in_r1 <= 0;
        data_in_r2 <= 0;
    end
    else begin
        data_in_r1 <= data_in;
        data_in_r2 <= data_in_r1;
    end
end








    
endmodule       

//*************** END OF LINE ********************//