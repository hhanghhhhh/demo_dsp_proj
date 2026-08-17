//************************************************//
//FILE NAME:        ADC.V
//DESCRIPTION:      读取LTC2387 ADC采样芯片数据
//AUTHOR:           hehang
//DATE:             20230330
//LOG:              
//************************************************//

//************** Module Define *******************//
module top
(
    input   clk_external, //晶振时钟，25M

    //FPGA与DSP通讯
    input                       to_dsp_flag,	//与XRD_n连接
    input                       from_dsp_flag,	//与XWE0_n连接
    input      [6:0]            com_dsp_addr,   //与XA1-7连接，仅用7根
    inout      [31:0]           com_dsp_data,   //与XD0-XD31连接
    
    //DCDC
    output      wire            PWM_a,           //PWM_a
    output      wire            PWM_b,           //PWM_b
    
	output	wire		FlashSpi_Cs,
	output	wire		FlashSpi_Clk,
	output	wire		FlashSpi_Mosi,
	input	wire		FlashSpi_Miso
    
);

//*************** Parameter & Internal Register ****//
wire    [31:0]      wire32_from_dsp_data        ;
wire    [31:0]      wire32_to_dsp_data          ;
wire                sys_clk_50m                 ;
wire                sys_clk_150m                ;
wire                sys_clk_200m                ;
wire                sys_clk                     ; 
wire                sys_rst                     ;
wire                sys_rst_pll                 ;

wire			UpdateOk;
wire			UpdateStart;
wire			EraseState;
wire			Update_FlashWrite;
wire	[15:0]	Update_PackageNum;
wire	[15:0]	Update_CurrentPackage;
wire	[15:0]	Update_WriteLength;		// synthesis keep
wire			Update_WriteOver;

wire			FlashTestRead;			// synthesis keep
wire			FlashTestWrite;			// synthesis keep
wire			FlashTestSectorErase;
wire			FlashTestBlock64Erase;
wire	[23:0]	FlashTestAdd;
wire	[15:0]	FlashTestLength;
wire			FlashTestOver;			// synthesis keep
    
wire			FlashRamWrite_WriteFlag;	// synthesis keep
wire	[7:0]	FlashRamWrite_WriteAdd;		// synthesis keep
wire	[7:0]	FlashRamWrite_WriteData;	// synthesis keep
wire			FlashRamWrite_DoneFlag;
    
wire			FlashRamRead_ReadFlag;
wire	[7:0]	FlashRamRead_ReadAdd;
wire	[7:0]	FlashRamRead_ReadData;
wire			FlashRamRead_DoneFlag;



//*************** Main Code ***********************//
assign  com_dsp_data = (to_dsp_flag == 1'b0) ? wire32_to_dsp_data : 32'bz;
assign  wire32_from_dsp_data = (from_dsp_flag == 1'b0) ? com_dsp_data : 32'd0;
assign sys_rst = sys_rst_pll;


assign	PWM_a = 1'b0;           //PWM_a
assign	PWM_b = 1'b0;           //PWM_b
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



DATA_REG    data_reg_1
(
    .sys_clk            (sys_clk_150m               ),
    .sys_rst            (sys_rst                    ),
    .com_dsp_addr       (com_dsp_addr               ),
    //dsp与fpga之间连接
    .to_dsp_flag        (to_dsp_flag                ),
    .from_dsp_flag      (from_dsp_flag              ),
    .from_dsp_data      (wire32_from_dsp_data       ),  
    .to_dsp_data        (wire32_to_dsp_data         ),
    
    .o_update(UpdateStart),
	.o_flashwrite(Update_FlashWrite),
    .o_packagenum(Update_PackageNum),
    .o_currentpackage(Update_CurrentPackage),
	.o_writelength(Update_WriteLength),
	.i_writeover(Update_WriteOver),

	.i_erasestate(EraseState),
    
    .o_ramwrite_writeflag(FlashRamWrite_WriteFlag),
    .o_ramwrite_writeadd(FlashRamWrite_WriteAdd),
    .o_ramwrite_writedata(FlashRamWrite_WriteData),
    .i_ramwrite_doneflag(FlashRamWrite_DoneFlag),
    
    .o_ramread_readflag(FlashRamRead_ReadFlag),
    .o_ramread_readadd(FlashRamRead_ReadAdd),
    .i_ramread_readdata(FlashRamRead_ReadData),
    .i_ramread_doneflag(FlashRamRead_DoneFlag) 

);


Update update
(
    .sys_clk(sys_clk_150m),
    .sys_rst(sys_rst),

	.i_flashwrite(Update_FlashWrite),
    .i_packagenum(Update_PackageNum),
    .i_currentpackage(Update_CurrentPackage),
	.i_writelength(Update_WriteLength),
	.o_writeover(Update_WriteOver),

    .spi_clk(FlashSpi_Clk),
    .spi_cs(FlashSpi_Cs),
    .spi_mosi(FlashSpi_Mosi),
    .spi_miso(FlashSpi_Miso),
    
	.i_update(UpdateStart),
    .i_flashtestread(FlashTestRead),
    .i_flashtestwrite(FlashTestWrite),
	.i_flashsectorerase(FlashTestSectorErase),
    .i_flashblock64erase(FlashTestBlock64Erase),
    .i_flashtestadd(FlashTestAdd),
    .i_flashtestlength(FlashTestLength),
    .o_flashtestover(FlashTestOver),

	.o_erasestate(EraseState),
    
	.i_ramwrite_writeflag(FlashRamWrite_WriteFlag),
    .i_ramwrite_writeadd(FlashRamWrite_WriteAdd),
    .i_ramwrite_writedata(FlashRamWrite_WriteData),
    .o_ramwrite_doneflag(FlashRamWrite_DoneFlag),
    
    .i_ramread_readflag(FlashRamRead_ReadFlag),
    .i_ramread_readadd(FlashRamRead_ReadAdd),
    .o_ramread_readdata(FlashRamRead_ReadData),
    .o_ramread_doneflag(FlashRamRead_DoneFlag)
);




endmodule

//*************** END OF LINE ********************//    
    
    