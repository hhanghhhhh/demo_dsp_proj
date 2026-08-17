//************************************************//
//FILE NAME:		dsp_xintf_ctrl .V
//DESCRIPTION:		FPGA与DSP数据交互模块
//AUTHOR:			hehang
//DATE:				20251218
//LOG:				
//************************************************//
//************** Module Define *******************//
module dsp_xintf_ctrl
(
    input                       sys_clk         ,   //150M时钟
    input                       sys_rst_n       ,
    // DSP Physical Interface
    input      [6:0]            i_dsp_addr      ,
    input                       i_dsp_rd_n      ,
    input                       i_dsp_wr_n      ,
    inout      [31:0]           io_dsp_data     ,
    // RAM Interface
    output wire [6:0]           o_ram_addr      ,   // RAM 地址
    output wire                 o_ram_ce        ,   // RAM 片选 (Chip Enable)
    output wire                 o_ram_we        ,   // RAM 写使能 (Write Enable)
    input  wire [31:0]          i_ram_rdata     ,   // RAM 读数据
    output wire [31:0]          o_ram_wdata     ,   // RAM 写数据
    // dsp wr data
    output reg  [31:0]          dsp_wr_data1    ,
    // dsp rd data
    input  wire [31:0]          fpga_info           
);

//*************** Parameter & Internal Register ****//
// 这两个地址是 dsp 程序中 DataR、DataW 的地址
// 大于这个地址后，DataR、DataW 将会操作 ram
localparam  DSP_WR_ADDR_TH      =   200;
localparam  DSP_RD_ADDR_TH      =   200;
// 这两个地址是 ram 的地址
// ram 分两部分，0 - 512    用于 fpga 写入，dsp 读出
//              512 - 1024 用于 dsp 写入，fpga 读出
localparam  RAM_DSP_RD_BASE      =   0;
localparam  RAM_DSP_WR_BASE      =   512;
// 若相等，则 读写操作的同一个地址
// localparam  RAM_DSP_RD_BASE      =   0;
// localparam  RAM_DSP_WR_BASE      =   0;

reg [ 1:0]   dsp_rd_n_r         ;
reg [ 1:0]   dsp_wr_n_r         ;
wire         dsp_wr_pulse       ;
wire         dsp_rd_active      ;
reg [31:0]   dsp_rd_data        ;

//*************** Main Code ***********************//
assign dsp_wr_pulse = (dsp_wr_n_r[1] == 1'b0) && (dsp_wr_n_r[0] == 1'b1);
assign dsp_rd_active = (dsp_rd_n_r[1] == 1'b0);
assign io_dsp_data = dsp_rd_active ? dsp_rd_data : 32'bz;

// ram 接口
// 地址先减去 dsp 侧的偏移，再加上实际用的 ram 地址偏移
assign o_ram_addr = dsp_rd_active ? (i_dsp_addr - DSP_RD_ADDR_TH + RAM_DSP_RD_BASE) : (i_dsp_addr - DSP_WR_ADDR_TH + RAM_DSP_WR_BASE);
assign o_ram_wdata = io_dsp_data;
assign o_ram_ce   = (dsp_wr_pulse && (i_dsp_addr >= DSP_WR_ADDR_TH)) || (dsp_rd_active && (i_dsp_addr >= DSP_RD_ADDR_TH));
assign o_ram_we = dsp_wr_pulse && (i_dsp_addr >= DSP_WR_ADDR_TH);

//================================================================
// 1. 信号跨时钟域同步 (CDC)
//    只对控制信号进行打拍，不对数据/地址打拍
//================================================================

always@(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n) begin
        dsp_rd_n_r   <= 2'b11;
        dsp_wr_n_r   <= 2'b11;
    end
    else begin
        dsp_rd_n_r <= {dsp_rd_n_r[0], i_dsp_rd_n};
        dsp_wr_n_r <= {dsp_wr_n_r[0], i_dsp_wr_n};
    end
end

//================================================================
// 2. 写逻辑 (DSP -> FPGA)
//    在 dsp_wr_pulse 发生时，直接采样总线数据写入寄存器
//================================================================

always @(posedge sys_clk or negedge sys_rst_n) begin
    if (!sys_rst_n) begin
        dsp_wr_data1 <= 32'd0;
    end else begin
        if (dsp_wr_pulse) begin
            case (i_dsp_addr)
                0: dsp_wr_data1 <= io_dsp_data;
                default: ;
            endcase
        end
    end
end

//================================================================
// 3. 读逻辑 (FPGA -> DSP)
//    准备数据
//================================================================

always @(posedge sys_clk or negedge sys_rst_n) begin
    if (!sys_rst_n) begin
        dsp_rd_data <= 32'd0;
    end else begin
        if (i_dsp_addr >= DSP_RD_ADDR_TH) begin
            // [RAM 区域]：直接锁存 RAM 返回的数据
            dsp_rd_data <= i_ram_rdata;
        end else begin
            case (i_dsp_addr)
                0: dsp_rd_data <= fpga_info;
                default: dsp_rd_data <= 32'd0;
            endcase
        end
    end
end





endmodule

//*************** END OF LINE ********************//	
	
	