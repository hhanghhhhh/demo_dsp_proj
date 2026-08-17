`include "Package.v"

module Update
(
    input               		sys_clk,
    input               		sys_rst,

	input	wire				i_flashwrite,
    input	wire	[15:0]		i_packagenum,
    input	wire	[15:0]		i_currentpackage,
	input	wire	[15:0]		i_writelength,
	output	reg					o_writeover,

	input	wire				i_update,
	input	wire				i_flashtestread, 
    input	wire				i_flashtestwrite,
	input	wire				i_flashsectorerase,
    input	wire				i_flashblock64erase,
    input	wire	[23:0]		i_flashtestadd,
    input	wire	[15:0]		i_flashtestlength,
    output	wire				o_flashtestover,

	output	reg					o_erasestate,

	input	wire				i_ramwrite_writeflag,
    input	wire	[7:0]		i_ramwrite_writeadd,
    input	wire	[7:0]		i_ramwrite_writedata,
    output	wire				o_ramwrite_doneflag,
    
    input	wire				i_ramread_readflag,
    input	wire	[7:0]		i_ramread_readadd,
    output	wire	[7:0]		o_ramread_readdata,
    output	wire				o_ramread_doneflag,

    output	wire				spi_clk,
    output	wire				spi_cs,
    output	wire				spi_mosi,
    input	wire				spi_miso
);


parameter	STATE_FLAGREAD			=	8'd0;
parameter	STATE_FLAGCHECK			=	8'd1;
parameter	STATE_ERASE				=	8'd2;
parameter	STATE_IDLE				=	8'd3;
parameter	STATE_WRITEOVER			=	8'd4;
parameter	STATE_FLAGERASE			=	8'd5;
parameter	STATE_FLAGWRITE			=	8'd6;
parameter	STATE_END				=	8'd7;
parameter	STATE_OK				=	8'd8;
parameter	STATE_RAMWRITE			=	8'd9;
parameter	STATE_RAMREAD			=	8'd10;
parameter	STATE_DELAY				=	8'd11;


wire			RamWrite_WriteFlag;
wire	[7:0]	RamWrite_WriteAdd;
wire	[7:0]	RamWrite_WriteData;
wire			RamWrite_DoneFlag;
    
wire			RamRead_ReadFlag;
wire	[7:0]	RamRead_ReadAdd;
wire	[7:0]	RamRead_ReadData;
wire			RamRead_DoneFlag;

reg				FlagRamWrite_WriteFlag;
reg		[7:0]	FlagRamWrite_WriteAdd;
reg		[7:0]	FlagRamWrite_WriteData;

reg				FlagRamRead_ReadFlag;
reg		[7:0]	FlagRamRead_ReadAdd;

reg				Boot_Start;
reg		[7:0]	State;
reg		[7:0]	JumpState;
reg		[7:0]	EraseCnt;
reg		[31:0]	DelayCnt;
reg				FlagReadWrite;
reg				FlashCtl;

reg				Flash_ReadFlag;		// synthesis keep
reg				Flash_WriteFlag;	// synthesis keep
reg				Flash_SectorErase;	// synthesis keep
reg				Flash_Block64Erase;	// synthesis keep
reg		[23:0]	Flash_Address;
reg		[15:0]	Flash_Length;
wire			Flash_DoneFlag;

wire			FlashUse_ReadFlag;		// synthesis keep
wire			FlashUse_WriteFlag;		// synthesis keep
wire			FlashUse_SectorErase;	// synthesis keep
wire			FlashUse_Block64Erase;	// synthesis keep
wire	[23:0]	FlashUse_Address;
wire	[15:0]	FlashUse_Length;

reg		[31:0]	MultibootAddress;

//If FlashCtl is high level flash control by update module, else control by upper test flash commands
assign	FlashUse_ReadFlag = (FlashCtl == 1'b1) ? Flash_ReadFlag : i_flashtestread;
assign	FlashUse_WriteFlag = (FlashCtl == 1'b1) ? Flash_WriteFlag : i_flashtestwrite;
assign	FlashUse_SectorErase = (FlashCtl == 1'b1) ? Flash_SectorErase : i_flashsectorerase;
assign	FlashUse_Block64Erase = (FlashCtl == 1'b1) ? Flash_Block64Erase : i_flashblock64erase;
assign	FlashUse_Address = (FlashCtl == 1'b1) ? Flash_Address : i_flashtestadd;
assign	FlashUse_Length = (FlashCtl == 1'b1) ? Flash_Length : i_flashtestlength;
assign	o_flashtestover = Flash_DoneFlag;

//If FlagReadWrite is high level flash ram control by update module, else control by upper test flash commands
assign	RamWrite_WriteFlag = (FlagReadWrite == 1'b0) ? i_ramwrite_writeflag : FlagRamWrite_WriteFlag;
assign	RamWrite_WriteAdd = (FlagReadWrite == 1'b0) ? i_ramwrite_writeadd : FlagRamWrite_WriteAdd;
assign	RamWrite_WriteData = (FlagReadWrite == 1'b0) ? i_ramwrite_writedata : FlagRamWrite_WriteData;
assign	o_ramwrite_doneflag = RamWrite_DoneFlag;
    
assign	RamRead_ReadFlag = (FlagReadWrite == 1'b0) ? i_ramread_readflag : FlagRamRead_ReadFlag;
assign	RamRead_ReadAdd = (FlagReadWrite == 1'b0) ? i_ramread_readadd : FlagRamRead_ReadAdd;
assign	o_ramread_readdata = RamRead_ReadData;
assign	o_ramread_doneflag = RamRead_DoneFlag;


always @(posedge sys_clk or negedge sys_rst)
begin
	if(!sys_rst)
    begin
    	State <= STATE_DELAY;
		JumpState <= STATE_DELAY;
		Boot_Start <= 1'b1;
		Flash_ReadFlag <= 1'b0;
		Flash_WriteFlag <= 1'b0;
		Flash_SectorErase <= 1'b0;
		Flash_Block64Erase <= 1'b0;
		Flash_Address <= 24'd0;
		Flash_Length <= 16'd0;
		o_writeover <= 1'b0;
		EraseCnt <= 8'd0;
		DelayCnt <= 32'd0;
		FlagReadWrite <= 1'b0;
		FlashCtl <= 1'b0;
		o_erasestate <= 1'b1;
		MultibootAddress <= 8'd0;
    end
	else
    begin
    	case(State)

			STATE_DELAY:
            begin
				if(DelayCnt < 32'd1000000)
                begin
                	DelayCnt <= DelayCnt + 32'd1;
                end
				else
                begin
                	DelayCnt <= 32'd0;
					State <= STATE_FLAGREAD;
                end
            end
			//update flag read
			STATE_FLAGREAD:
            begin
            	Flash_ReadFlag <= 1'b1;
				FlashCtl <= 1'b1;
				Flash_Length <= 16'd1;
                Flash_Address <= `FLASHADDR_FLAG;
                State <= STATE_OK;
				JumpState <= STATE_RAMREAD;
            end
			//read flag from flash ram
			STATE_RAMREAD:
            begin
				FlagReadWrite <= 1'b1;
            	FlagRamRead_ReadFlag <= 1'b1;
				FlagRamRead_ReadAdd <= 8'd0;
				State <= STATE_FLAGCHECK;
            end
			//flag check, if flag is high, go to update process, else jump to app project
			STATE_FLAGCHECK:
            begin
            	if(RamRead_DoneFlag == 1'b1)
                begin
                	FlagReadWrite <= 1'b0;
					FlagRamRead_ReadFlag <= 1'b0;

					if(RamRead_ReadData == 8'd1)
                    begin
						o_erasestate <= 1'b1;
						MultibootAddress <= `FLASHADDR_APP;
                    	State <= STATE_ERASE;
                    end
                    else
                    begin
                    	State <= STATE_END;
						if(RamRead_ReadData == 8'd2)
                        begin
                        	MultibootAddress <= `FLASHADDR_BACKUP;
                        end
						else
                        begin
                        	MultibootAddress <= `FLASHADDR_APP;
                        end
                    end

                end
            end
			//flash erase
			STATE_ERASE:
            begin
				if(EraseCnt < 8'd40)
                begin
                	EraseCnt <= EraseCnt + 8'd1;
            		Flash_Block64Erase <= 1'b1;
					FlashCtl <= 1'b1;
                	Flash_Address <= {EraseCnt, 16'd0} + 24'h500000;
                	State <= STATE_OK;
					JumpState <= STATE_ERASE;
                end
                else
                begin
					o_erasestate <= 1'b0;
                	State <= STATE_FLAGERASE;
                end
            end
			//wait for update package
			STATE_IDLE:
            begin
            	if(i_flashwrite == 1'b1)
                begin
            		Flash_WriteFlag <= 1'b1;
					FlashCtl <= 1'b1;
					Flash_Length <= i_writelength;
                	Flash_Address <= {i_currentpackage, 7'd0} + `FLASHADDR_APP;
                	State <= STATE_OK;
					JumpState <= STATE_WRITEOVER;
                end
				else if(i_update == 1'b1)
                begin
                	State <= STATE_END;
                end
            end
			//flash write over
			STATE_WRITEOVER:
            begin
				if(i_flashwrite == 1'b1)
				begin
                	o_writeover <= 1'b1;
                end
                else
                begin
                	o_writeover <= 1'b0;
					State <= STATE_IDLE;
                end
            end
			//erase flash where update flag store
			STATE_FLAGERASE:
            begin
				Flash_SectorErase <= 1'b1;
				FlashCtl <= 1'b1;
				Flash_Address <= `FLASHADDR_FLAG;
                State <= STATE_OK;
				JumpState <= STATE_RAMWRITE;
            end
			//write flag to flash ram
			STATE_RAMWRITE:
            begin
				FlagReadWrite <= 1'b1;
				FlagRamWrite_WriteFlag <= 1'b1;
				FlagRamWrite_WriteAdd <= 8'd0;
				FlagRamWrite_WriteData <= 8'd2;
				State <= STATE_FLAGWRITE;
            end
			//flag write to flash
			STATE_FLAGWRITE:
            begin
				if(RamWrite_DoneFlag == 1'b1)
                begin
					FlagReadWrite <= 1'b0;
					FlashCtl <= 1'b1;
            		Flash_WriteFlag <= 1'b1;
					Flash_Length <= 16'b1;
                	Flash_Address <= `FLASHADDR_FLAG;
                	State <= STATE_OK;
					JumpState <= STATE_IDLE;
                end
            end
			//jump to app project
			STATE_END:
            begin
				if(DelayCnt < 32'd1000000)
                begin
                	DelayCnt <= DelayCnt + 32'd1;
                end
				else
                begin
					
                	Boot_Start <= 1'b0;
                end
            end
			//flash erase, read or write over
			STATE_OK:
            begin
            	if(Flash_DoneFlag == 1'b1)
                begin
					FlashCtl <= 1'b0;
					Flash_ReadFlag <= 1'b0;
					Flash_WriteFlag <= 1'b0;
					Flash_SectorErase <= 1'b0;
					Flash_Block64Erase <= 1'b0;
					State <= JumpState;
                end
            end
		
		endcase
    end
end

//Project jump
PH1_LOGIC_MBOOT_V2#
(
	.MULTBOOT_ADDRSRC_SEL("DYNAMIC"),
	.MULTBOOT_STATIC_ADDR ()
)
MBOOT_inst
(
	.multiboot_en_n_i (Boot_Start),
	.multiboot_code_i (8'hA5),
	.multiboot_addr_i (MultibootAddress)
);

//flash erase, read and write
W25Q64 w25q64
(
    .sys_clk(sys_clk),
    .sys_rst(sys_rst),
    
    .i_readflag(FlashUse_ReadFlag),
    .i_writeflag(FlashUse_WriteFlag),
	.i_sectorerase(FlashUse_SectorErase),
	.i_block64erase(FlashUse_Block64Erase),
    .i_address(FlashUse_Address),
	.i_length(FlashUse_Length),
	.o_doneflag(Flash_DoneFlag),

	.i_ramwrite_writeflag(RamWrite_WriteFlag),
    .i_ramwrite_writeadd(RamWrite_WriteAdd),
    .i_ramwrite_writedata(RamWrite_WriteData),
    .o_ramwrite_doneflag(RamWrite_DoneFlag),
    
    .i_ramread_readflag(RamRead_ReadFlag),
    .i_ramread_readadd(RamRead_ReadAdd),
    .o_ramread_readdata(RamRead_ReadData),
    .o_ramread_doneflag(RamRead_DoneFlag),
    
    .spi_clk(spi_clk),
    .spi_cs(spi_cs),
    .spi_mosi(spi_mosi),
    .spi_miso(spi_miso)
);

endmodule
