module top
(
    input                       clk_external    , //晶振时钟，25M

    //FPGA与DSP通讯
    input                       xintf_dsp_rd_n  ,	//与XRD_n连接
    input                       xintf_dsp_wr_n  ,	//与XWE0_n连接
    input      [15:0]           xintf_dsp_addr  ,   //与XA1-XA16连接
    inout      [31:0]           xintf_dsp_data      //与XD0-XD31连接
);

//*************** Parameter & Internal Register ****//
wire                sys_clk_100m                ;   //synthesis keep
wire                sys_rst_n                   ;
wire                pll_locked                  ;
wire    [31:0]      fpga_info                   ;
wire    [31:0]      dsp_reg_data                ;

//*************** Main Code ***********************//
//例化模型
// PLL
PLL_0 U_pll_100m
(
    .refclk             (clk_external           ),
    .reset              (1'b0                   ),
    .lock               (pll_locked             ), //时钟稳定后释放系统复位
    .clk0_out           (sys_clk_100m           )
);

reset_release_sync u_reset_release 
(
    .clk                (sys_clk_100m           ), 
    .async_ready        (pll_locked             ), 
    .rst_n              (sys_rst_n              )
);

// DSP 读 RAM：FPGA 业务侧写 A 口，DSP 通过 B 口读。
wire  [8:0]     dsp_rd_ram_addr ;
wire            dsp_rd_ram_ce   ;
wire  [31:0]    dsp_rd_ram_data ;
wire  [8:0]     dsp_rd_ram_fpga_addr ;
wire            dsp_rd_ram_fpga_ce   ;
wire  [31:0]    dsp_rd_ram_fpga_data ;

// DSP 写 RAM：DSP 通过 A 口写，FPGA 业务侧通过 B 口读。
wire  [8:0]     dsp_wr_ram_addr ;
wire            dsp_wr_ram_ce   ;
wire  [31:0]    dsp_wr_ram_data ;
wire  [31:0]    dsp_wr_ram_fpga_data;
wire  [8:0]     dsp_wr_ram_fpga_addr ;
wire            dsp_wr_ram_fpga_ce   ;


dsp_xintf_ctrl      dsp_xintf_ctrl_inst1
(
    .sys_clk         (sys_clk_100m           ),   //100M时钟
    .sys_rst_n       (sys_rst_n              ),
    .i_dsp_addr      (xintf_dsp_addr         ),
    .i_dsp_rd_n      (xintf_dsp_rd_n         ),
    .i_dsp_wr_n      (xintf_dsp_wr_n         ),
    .io_dsp_data     (xintf_dsp_data         ),
    .o_dsp_rd_ram_addr(dsp_rd_ram_addr       ),
    .o_dsp_rd_ram_ce (dsp_rd_ram_ce         ),
    .i_dsp_rd_ram_data(dsp_rd_ram_data       ),
    .o_dsp_wr_ram_addr(dsp_wr_ram_addr       ),
    .o_dsp_wr_ram_ce (dsp_wr_ram_ce         ),
    .o_dsp_wr_ram_data(dsp_wr_ram_data       ),
    // dsp wr data
    .dsp_wr_data1   (dsp_reg_data            ),

    // dsp rd data
    .fpga_info       (fpga_info              )
);

// 临时 XINTF 硬件验证逻辑：寄存器回显，并把 DSP 写 RAM 搬运到 DSP 读 RAM。
// 验证结束后删除本例化和 rtl/test/xintf_validation_test.v 即可。
xintf_validation_test U_xintf_validation_test
(
    .clk                        (sys_clk_100m              ),
    .rst_n                      (sys_rst_n                 ),
    .i_dsp_reg_data             (dsp_reg_data              ),
    .o_fpga_info                (fpga_info                 ),
    .o_dsp_wr_ram_fpga_addr     (dsp_wr_ram_fpga_addr      ),
    .o_dsp_wr_ram_fpga_ce       (dsp_wr_ram_fpga_ce        ),
    .i_dsp_wr_ram_fpga_data     (dsp_wr_ram_fpga_data      ),
    .o_dsp_rd_ram_fpga_addr     (dsp_rd_ram_fpga_addr      ),
    .o_dsp_rd_ram_fpga_ce       (dsp_rd_ram_fpga_ce        ),
    .o_dsp_rd_ram_fpga_data     (dsp_rd_ram_fpga_data      )
);

XINTF_RAM_DSP_RD_0 U_xintf_ram_dsp_rd
(
    // FPGA 验证逻辑写口
    .dia             (dsp_rd_ram_fpga_data  ),
    .addra           (dsp_rd_ram_fpga_addr  ),
    .cea             (dsp_rd_ram_fpga_ce    ),
    .clka            (sys_clk_100m           ),
    // DSP 读口
    .dob             (dsp_rd_ram_data        ),
    .addrb           (dsp_rd_ram_addr        ),
    .ceb             (dsp_rd_ram_ce          ),
    .clkb            (sys_clk_100m           )
);

XINTF_RAM_DSP_WR_0 U_xintf_ram_dsp_wr
(
    // DSP 写口
    .dia             (dsp_wr_ram_data        ),
    .addra           (dsp_wr_ram_addr        ),
    .cea             (dsp_wr_ram_ce          ),
    .clka            (sys_clk_100m           ),
    // FPGA 验证逻辑读口
    .dob             (dsp_wr_ram_fpga_data   ),
    .addrb           (dsp_wr_ram_fpga_addr  ),
    .ceb             (dsp_wr_ram_fpga_ce    ),
    .clkb            (sys_clk_100m           )
);


endmodule

//*************** END OF LINE ********************//
