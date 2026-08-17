// Temporary DSP/FPGA XINTF hardware validation.
// Delete this file and its top.v instance after validation.
module xintf_validation_test
(
    input  wire        clk,
    input  wire        rst_n,
    input  wire [31:0] i_dsp_reg_data,
    output wire [31:0] o_fpga_info,

    output wire [8:0]  o_dsp_wr_ram_fpga_addr,
    output wire        o_dsp_wr_ram_fpga_ce,
    input  wire [31:0] i_dsp_wr_ram_fpga_data,

    output wire [8:0]  o_dsp_rd_ram_fpga_addr,
    output wire        o_dsp_rd_ram_fpga_ce,
    output wire [31:0] o_dsp_rd_ram_fpga_data
);

reg [8:0] scan_addr;
reg [1:0] copy_phase;

// Register-direction test: fixed FPGA signature plus DSP write-data echo.
assign o_fpga_info = {16'hA55A, i_dsp_reg_data[15:0]};

// Read every location from the DSP-write RAM.
assign o_dsp_wr_ram_fpga_addr = scan_addr;
assign o_dsp_wr_ram_fpga_ce = (copy_phase == 2'd0);

// Allow two clocks for the read path, then copy to the same address.
assign o_dsp_rd_ram_fpga_addr = scan_addr;
assign o_dsp_rd_ram_fpga_ce = (copy_phase == 2'd2);
assign o_dsp_rd_ram_fpga_data = i_dsp_wr_ram_fpga_data;

always @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        scan_addr <= 9'd0;
        copy_phase <= 2'd0;
    end else begin
        if(copy_phase == 2'd2) begin
            copy_phase <= 2'd0;
            scan_addr <= scan_addr + 9'd1;
        end else begin
            copy_phase <= copy_phase + 2'd1;
        end
    end
end

endmodule
