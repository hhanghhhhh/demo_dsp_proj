/*
不同时钟域参数同步
// 例化模板
auto_param_sync #(
    .DATA_WIDTH      (32)
)para_inst1(
    .rst_n           (),
    .clk_src         (),
    .i_data_src      (),
    .clk_dest        (),
    .o_data_dest     ()
);

*/


module auto_param_sync #(
    parameter DATA_WIDTH = 32
)(
    //-----------------------------------------------------
    // 源时钟域 (Source Domain)
    //-----------------------------------------------------
    input  wire                     clk_src         ,   
    input  wire                     rst_n           ,      
    input  wire [DATA_WIDTH-1:0]    i_data_src      , 

    //-----------------------------------------------------
    // 目的时钟域 (Destination Domain)
    //-----------------------------------------------------
    input  wire                     clk_dest        ,
    output reg  [DATA_WIDTH-1:0]    o_data_dest 
);

//=========================================================
// 1. [源时钟域] 变化检测
//=========================================================
reg [DATA_WIDTH-1:0]    r_src_data;
reg                     r_src_req;
wire                    w_src_ack; // 来自目的域的应答（已同步到src）

// 只有当 当前没有请求 或者 请求已被应答(传输完成) 时，才允许采样新数据
wire src_idle = (r_src_req == w_src_ack);

always @(posedge clk_src or negedge rst_n) begin
    if (!rst_n) begin
        r_src_data <= {DATA_WIDTH{1'b0}};
        r_src_req  <= 1'b0;
    end else begin
        if (src_idle) begin
            if (i_data_src != r_src_data) begin
                r_src_data <= i_data_src;
                r_src_req  <= ~r_src_req;
            end
        end
    end
end

//=========================================================
// 跨时钟域处理
//=========================================================
// 1. 将源域的 Req 同步到 目的域
reg r_dest_req_d0, r_dest_req_d1, r_dest_req_d2;
always @(posedge clk_dest or negedge rst_n) begin
    if (!rst_n) begin
        r_dest_req_d0 <= 1'b0;
        r_dest_req_d1 <= 1'b0;
        r_dest_req_d2 <= 1'b0;
    end else begin
        r_dest_req_d0 <= r_src_req;
        r_dest_req_d1 <= r_dest_req_d0;
        r_dest_req_d2 <= r_dest_req_d1;
    end
end

// 2. 将目的域的 Ack (即 r_dest_req_d2) 同步回 源域
reg r_src_ack_d0, r_src_ack_d1;
always @(posedge clk_src or negedge rst_n) begin
    if (!rst_n) begin
        r_src_ack_d0 <= 1'b0;
        r_src_ack_d1 <= 1'b0;
    end else begin
        r_src_ack_d0 <= r_dest_req_d2;
        r_src_ack_d1 <= r_src_ack_d0;
    end
end

assign w_src_ack = r_src_ack_d1;

//=========================================================
// 2. [目的时钟域] 打拍同步、边沿检测 & 更新
//=========================================================
// 边沿检测：利用 d2 和 d1 的异或
wire w_dest_change = (r_dest_req_d2 ^ r_dest_req_d1);

always @(posedge clk_dest or negedge rst_n) begin
    if (!rst_n) begin
        o_data_dest <= {DATA_WIDTH{1'b0}};
    end else begin
        if (w_dest_change) begin
            o_data_dest <= r_src_data;
        end
    end
end




endmodule