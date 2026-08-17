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
    input                       sys_clk         ,   //100M时钟
    input                       sys_rst_n       ,
    // DSP Physical Interface
    input      [15:0]           i_dsp_addr      ,
    input                       i_dsp_rd_n      ,
    input                       i_dsp_wr_n      ,
    inout      [31:0]           io_dsp_data     ,
    // DSP 读 RAM（FPGA -> DSP）
    output wire [8:0]           o_dsp_rd_ram_addr,
    output wire                 o_dsp_rd_ram_ce  ,
    input  wire [31:0]          i_dsp_rd_ram_data,
    // DSP 写 RAM（DSP -> FPGA）
    output wire [8:0]           o_dsp_wr_ram_addr,
    output wire                 o_dsp_wr_ram_ce  ,
    output wire [31:0]          o_dsp_wr_ram_data,
    // dsp wr data
    output reg  [31:0]          dsp_wr_data1    ,
    // dsp rd data
    input  wire [31:0]          fpga_info           
);

//*************** Parameter & Internal Register ****//
// DSP 地址 200-711 分别映射到读 RAM/写 RAM 的 0-511。
// 读写使用两块独立 RAM，相同 DSP 地址在两个方向上不会互相覆盖。
localparam [15:0] DSP_RAM_ADDR_BASE = 16'd200;
localparam [15:0] DSP_RAM_ADDR_END  = 16'd712;

(* async_reg = "true" *) reg [ 1:0]   dsp_rd_n_r         ;
(* async_reg = "true" *) reg [ 1:0]   dsp_wr_n_r         ;
wire         dsp_wr_pulse       ;
wire         dsp_rd_active      ;
wire         dsp_ram_addr_valid ;
reg [31:0]   dsp_rd_data        ;

//*************** Main Code ***********************//
assign dsp_wr_pulse = !dsp_wr_n_r[1] && dsp_wr_n_r[0];
assign dsp_rd_active = !dsp_rd_n_r[1];
assign io_dsp_data = dsp_rd_active ? dsp_rd_data : 32'bz;

// 两块 RAM 共用 DSP 逻辑窗口，但读写使能和数据通路完全分离。
assign dsp_ram_addr_valid  = (i_dsp_addr >= DSP_RAM_ADDR_BASE)
                             && (i_dsp_addr < DSP_RAM_ADDR_END);
assign o_dsp_rd_ram_addr  = i_dsp_addr - DSP_RAM_ADDR_BASE;
assign o_dsp_rd_ram_ce    = dsp_rd_active && dsp_ram_addr_valid;
assign o_dsp_wr_ram_addr  = i_dsp_addr - DSP_RAM_ADDR_BASE;
assign o_dsp_wr_ram_ce    = dsp_wr_pulse && dsp_ram_addr_valid;
assign o_dsp_wr_ram_data  = io_dsp_data;

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
        if (dsp_ram_addr_valid) begin
            // [RAM 区域]：DSP 只从独立的读 RAM 取数。
            dsp_rd_data <= i_dsp_rd_ram_data;
        end else begin
            case (i_dsp_addr)
                16'd0: dsp_rd_data <= fpga_info;
                default: dsp_rd_data <= 32'd0;
            endcase
        end
    end
end





endmodule

//*************** END OF LINE ********************//
