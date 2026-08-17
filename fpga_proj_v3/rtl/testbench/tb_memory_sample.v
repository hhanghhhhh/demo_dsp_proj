`timescale 1ns/1ps

module tb_MEMOR_SAMPLE;

    //==========================================
    // 时钟与复位信号
    //==========================================
    reg sys_clk;
    reg sys_rst;
    reg en;
    reg [15:0] intervl_in;
    reg [15:0] data_sample;
    reg next;

    wire [7:0] start_addr;
    wire [15:0] out_data[0:9];

reg  signed [16:0]     test_data_samp;

always@(posedge sys_clk or negedge sys_rst) begin
    if (!sys_rst) begin
        test_data_samp <= $signed(16'd0);
    end else begin
        if (test_data_samp > $signed(16'd1000)) begin
            test_data_samp <= $signed(-16'd1000);
        end else begin
            test_data_samp <= test_data_samp + $signed(16'd1);
        end
    end
end


    //==========================================
    // 实例化被测模块
    //==========================================
    MEMOR_SAMPLE dut (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        .en(en),
        .intervl_in(intervl_in),
        .data_sample(test_data_samp),   //data_sample
        .next(next),
        .start_addr(start_addr),
        .out_data1 (out_data[0]), 
        .out_data2 (out_data[1]), 
        .out_data3 (out_data[2]), 
        .out_data4 (out_data[3]), 
        .out_data5 (out_data[4]), 
        .out_data6 (out_data[5]), 
        .out_data7 (out_data[6]), 
        .out_data8 (out_data[7]), 
        .out_data9 (out_data[8]), 
        .out_data10(out_data[9])
    );

    //==========================================
    // 生成时钟 50MHz (周期 20ns)
    //==========================================
    initial begin
        sys_clk = 0;
        forever #10 sys_clk = ~sys_clk;
    end




    //==========================================
    // 初始化过程
    //==========================================
    integer i;

    initial begin
        sys_rst = 0;
        en = 0;
        intervl_in = 16'd50;      // 每隔 50 个时钟写入一次
        data_sample = 16'd0;
        next = 0;

        #200;
        sys_rst = 1;
        #700;
        en = 1;

        //==========================================
        // 模拟采样：写入 200 个采样点
        //==========================================
        for (i = 0; i < 200; i = i + 1) begin
            data_sample = i;          // 模拟采样值
            #(intervl_in * 20);       // 等待 intervl_in * clk 周期
        end

        en = 0;   // 停止采样写入
        #1000;

        //==========================================
        // 读取阶段：翻转 next 信号，每次读取 10 点
        //==========================================
        $display("开始读取数据...");
        for (i = 0; i < 20; i = i + 1) begin
            #100;
            next = ~next;     // 翻转 next
            #2000;            // 等待模块读完 10 点数据
            $display("批次 %0d: 起始地址=%0d", i, start_addr);
            $display("out_data = {%0d, %0d, %0d, %0d, %0d, %0d, %0d, %0d, %0d, %0d}",
                     out_data[0], out_data[1], out_data[2], out_data[3], out_data[4],
                     out_data[5], out_data[6], out_data[7], out_data[8], out_data[9]);
        end

        #2000;
        $display("仿真结束。");
    end

endmodule
