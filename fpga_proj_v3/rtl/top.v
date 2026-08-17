module top
(
    input                       clk_external    , //晶振时钟，25M

    //FPGA与DSP通讯
    input                       xintf_dsp_rd_n  ,	//与XRD_n连接
    input                       xintf_dsp_wr_n  ,	//与XWE0_n连接
    input      [ 6:0]           xintf_dsp_addr  ,   //与XA1-7连接，仅用7根
    inout      [31:0]           xintf_dsp_data      //与XD0-XD31连接
);

//*************** Parameter & Internal Register ****//
wire                sys_clk_50m                 ;
wire                sys_clk_150m                ;   //synthesis keep
wire                sys_clk_200m                ;
wire                sys_clk                     ; 
wire                sys_rst_n                   ;
wire                sys_rst_pll                 ;
wire    [31:0]      fpga_info                   ;

//*************** Main Code ***********************//
localparam  FPGA_VERSION = 1;
assign fpga_info[31:16] = FPGA_VERSION;
assign fpga_info[16: 0] = 8'd123;


assign sys_rst_n = sys_rst_pll;



//例化模型
// PLL
ADC_sys_clk_PLL U_adc_pll
(
    .refclk             (clk_external           ),
    .reset              (1'b0                   ),
    .lock               (sys_rst_pll            ), //时钟稳定后输出1，该信号作为整体的复位信号
    .clk0_out           (sys_clk_200m           ),
    .clk1_out           (sys_clk                ), //25M,系统时钟
    .clk2_out           (sys_clk_150m           ),
    .clk3_out           (sys_clk_50m            )
);



wire  [31:0]    rama_rdata      ;
wire  [31:0]    rama_wdata      ;
wire            rama_ce         ;
wire            rama_we         ;
wire  [15:0]    rama_addr       ;


dsp_xintf_ctrl      dsp_xintf_ctrl_inst1
(
    .sys_clk         (sys_clk_150m           ),   //150M时钟
    .sys_rst_n       (sys_rst_n              ),
    .i_dsp_addr      (xintf_dsp_addr         ),
    .i_dsp_rd_n      (xintf_dsp_rd_n         ),
    .i_dsp_wr_n      (xintf_dsp_wr_n         ),
    .io_dsp_data     (xintf_dsp_data         ),
    .o_ram_addr      (rama_addr              ),   // RAM 地址
    .o_ram_ce        (rama_ce                ),   // RAM 片选 (Chip Enable)
    .o_ram_we        (rama_we                ),   // RAM 写使能 (Write Enable)
    .i_ram_rdata     (rama_rdata             ),   // RAM 读数据
    .o_ram_wdata     (rama_wdata             ),   // RAM 写数据
    // dsp wr data

    // dsp rd data
    .fpga_info       (fpga_info              )
);

reg_ram_ip  dsp_xintf_ram_ip_inst1
( 
	.doa             (rama_rdata            ), 
    .dia             (rama_wdata            ), 
    .addra           (rama_addr             ), 
    .cea             (rama_ce               ), 
    .clka            (sys_clk_150m          ), 
    .wea             (rama_we               ), 
    .rsta            (1'b0                  ),

	.dob             (), 
    .dib             (), 
    .addrb           (), 
    .ceb             (), 
    .clkb            (), 
    .web             (), 
    .rstb            (1'b0                  )
);


auto_param_sync #(
    .DATA_WIDTH      (32)
)para_inst1(
    .rst_n           (sys_rst_n         ),
    .clk_src         (sys_clk_150m      ),
    .i_data_src      (),
    .clk_dest        (),
    .o_data_dest     ()
);



endmodule

//*************** END OF LINE ********************//    
    
    