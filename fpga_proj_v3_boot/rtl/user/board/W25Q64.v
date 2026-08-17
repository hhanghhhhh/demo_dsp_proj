module W25Q64
(
    input               		sys_clk,
    input               		sys_rst,
    
    input	wire				i_readflag,
    input	wire				i_writeflag,
	input	wire				i_sectorerase,
	input	wire				i_block64erase,
    input	wire	[23:0]		i_address,
	input	wire	[15:0]		i_length,
	output	reg					o_doneflag,

	input	wire				i_ramwrite_writeflag,
    input	wire	[7:0]		i_ramwrite_writeadd,
    input	wire	[7:0]		i_ramwrite_writedata,
    output	wire				o_ramwrite_doneflag,
    
    input	wire				i_ramread_readflag,
    input	wire	[7:0]		i_ramread_readadd,
    output	wire	[7:0]		o_ramread_readdata,
    output	wire				o_ramread_doneflag,
    
    output	wire				spi_clk,
    output	reg					spi_cs,
    output	wire				spi_mosi,
    input						spi_miso
);


parameter	STATE_IDLE			=	8'd0;
parameter	STATE_WRITEENABLE	=	8'd1;
parameter	STATE_SECTORERASE	=	8'd2;
parameter	STATE_BLOCK64ERASE	=	8'd3;
parameter	STATE_PAGEPROGRAM	=	8'd4;
parameter	STATE_RAMREAD		=	8'd5;
parameter	STATE_DATAWRITE		=	8'd6;
parameter	STATE_READSTART		=	8'd7;
parameter	STATE_DATAREAD		=	8'd8;
parameter	STATE_READOVER		=	8'd9;
parameter	STATE_RAMWRITE		=	8'd10;
parameter	STATE_STATUSREAD	=	8'd11;
parameter	STATE_STATUSCHECK	=	8'd12;
parameter	STATE_OK			=	8'd13;
parameter	STATE_END			=	8'd14;
parameter	STATE_DELAY			=	8'd15;

parameter	SPI_MAXLENGTH		=	16'd32;


reg							Spi_StartFlag;
wire						Spi_DoneFlag;
reg		[15:0]				Spi_TransLength;
reg		[SPI_MAXLENGTH-1:0]	Spi_SendPackage;
wire	[SPI_MAXLENGTH-1:0]	Spi_RecPackage;		// synthesis keep

reg		[7:0]		FlashState;
reg		[7:0]		JumpState;
reg		[31:0]		DelayCnt;
reg					EndFlag;
reg		[15:0]		ByteCnt;

reg					RamWrite_ReadFlag;	// synthesis keep
reg		[7:0]		RamWrite_ReadAdd;	// synthesis keep
wire	[7:0]		RamWrite_ReadData;	// synthesis keep

reg					RamRead_WriteFlag;
reg		[7:0]		RamRead_WriteAdd;
reg		[7:0]		RamRead_WriteData;


always @(posedge sys_clk or negedge sys_rst)
begin
	if(!sys_rst)
    begin
    	FlashState <= STATE_IDLE;
		JumpState <= STATE_IDLE;
		Spi_StartFlag <= 1'b0;
		Spi_TransLength <= 16'd0;
		Spi_SendPackage <= 0;
        o_doneflag <= 1'b0;
        DelayCnt <= 32'd0;
		spi_cs <= 1'b1;
		EndFlag <= 1'b0;
		ByteCnt <= 16'd0;
		RamWrite_ReadFlag <= 1'b0;
		RamWrite_ReadAdd <= 8'd0;
		RamRead_WriteFlag <= 1'b0;
		RamRead_WriteAdd <= 8'd0;
		RamRead_WriteData <= 8'd0;
    end
    else
    begin
    	case(FlashState)
        
        	STATE_IDLE:
            begin
            	if( i_sectorerase == 1'b1 || 
                	i_block64erase == 1'b1 ||
                    i_writeflag == 1'b1 )
                begin
                	FlashState <= STATE_WRITEENABLE;
                end
				else if(i_readflag == 1'b1)
                begin
					FlashState <= STATE_READSTART;
                end
				else
                begin
					JumpState <= STATE_IDLE;
					Spi_StartFlag <= 1'b0;
					Spi_TransLength <= 16'd0;
					Spi_SendPackage <= 0;
        			o_doneflag <= 1'b0;
                end
            end
            //flash erase or write need to enable first
            STATE_WRITEENABLE:
            begin
				spi_cs <= 1'b0;
            	Spi_StartFlag <= 1'b1;
                Spi_TransLength <= 16'd8;
                Spi_SendPackage[SPI_MAXLENGTH-1:SPI_MAXLENGTH-8] <= 8'h06;
                FlashState <= STATE_OK;
				EndFlag <= 1'b1;
				if(i_sectorerase == 1'b1)
                begin
                	JumpState <= STATE_SECTORERASE;
                end
                else if(i_block64erase == 1'b1)
                begin
                	JumpState <= STATE_BLOCK64ERASE;
                end
                else if(i_writeflag == 1'b1)
                begin
                	JumpState <= STATE_PAGEPROGRAM;
                end
            end
			//spi send sector erase command and address
			STATE_SECTORERASE:
            begin
				spi_cs <= 1'b0;
            	Spi_StartFlag <= 1'b1;
                Spi_TransLength <= 16'd32;
                Spi_SendPackage[SPI_MAXLENGTH-9:SPI_MAXLENGTH-32] <= i_address;
				Spi_SendPackage[SPI_MAXLENGTH-1:SPI_MAXLENGTH-8] <= 8'h20;
				EndFlag <= 1'b1;
				JumpState <= STATE_STATUSREAD;
                FlashState <= STATE_OK;
            end
			//spi send block64 erase command and address
			STATE_BLOCK64ERASE:
            begin
				spi_cs <= 1'b0;
            	Spi_StartFlag <= 1'b1;
                Spi_TransLength <= 16'd32;
                Spi_SendPackage[SPI_MAXLENGTH-9:SPI_MAXLENGTH-32] <= i_address;
				Spi_SendPackage[SPI_MAXLENGTH-1:SPI_MAXLENGTH-8] <= 8'hD8;
				EndFlag <= 1'b1;
				JumpState <= STATE_STATUSREAD;
                FlashState <= STATE_OK;
            end
            //spi send page program command and address
            STATE_PAGEPROGRAM:
            begin
				spi_cs <= 1'b0;
            	Spi_StartFlag <= 1'b1;
                Spi_TransLength <= 16'd32;
				Spi_SendPackage[SPI_MAXLENGTH-9:SPI_MAXLENGTH-32] <= i_address;
				Spi_SendPackage[SPI_MAXLENGTH-1:SPI_MAXLENGTH-8] <= 8'h02;
				EndFlag <= 1'b0;
				JumpState <= STATE_RAMREAD;
                FlashState <= STATE_OK;
            end
			//read data from flash ram
			STATE_RAMREAD:
            begin
				RamWrite_ReadFlag <= 1'b1;
				RamWrite_ReadAdd <= ByteCnt;
				FlashState <= STATE_DATAWRITE;
            end
			//spi send write data
			STATE_DATAWRITE:
            begin
            	if(o_ramwrite_doneflag == 1'b1)
                begin
                	RamWrite_ReadFlag <= 1'b0;
            		Spi_StartFlag <= 1'b1;
                	Spi_TransLength <= 16'd8;
					Spi_SendPackage[SPI_MAXLENGTH-1:SPI_MAXLENGTH-8] <= RamWrite_ReadData;
					if(ByteCnt < i_length-1)
                    begin
						ByteCnt <= ByteCnt + 16'd1;
						EndFlag <= 1'b0;
						JumpState <= STATE_RAMREAD;
                    end
                    else
                    begin
						ByteCnt <= 16'd0;
						EndFlag <= 1'b1;
						JumpState <= STATE_STATUSREAD;
                    end
                	FlashState <= STATE_OK;
                end
            end
			//spi send read command and address
			STATE_READSTART:
            begin
				spi_cs <= 1'b0;
            	Spi_StartFlag <= 1'b1;
                Spi_TransLength <= 16'd32;
				Spi_SendPackage[SPI_MAXLENGTH-9:SPI_MAXLENGTH-32] <= i_address;
				Spi_SendPackage[SPI_MAXLENGTH-1:SPI_MAXLENGTH-8] <= 8'h03;
				EndFlag <= 1'b0;
				JumpState <= STATE_DATAREAD;
                FlashState <= STATE_OK;
            end
			//spi read data
			STATE_DATAREAD:
            begin
				Spi_StartFlag <= 1'b1;
                Spi_TransLength <= 16'd8;
				Spi_SendPackage[SPI_MAXLENGTH-1:SPI_MAXLENGTH-8] <= 8'hff;
				JumpState <= STATE_READOVER;
                FlashState <= STATE_OK;
            	if(ByteCnt < i_length-1)
                begin
					EndFlag <= 1'b0;
                end
                else
                begin
					EndFlag <= 1'b1;
                end
            end
			//spi read over and write data to flash ram ip
			STATE_READOVER:
            begin
            	ByteCnt <= ByteCnt + 16'd1;
				RamRead_WriteFlag <= 1'b1;
				RamRead_WriteAdd <= ByteCnt;
				RamRead_WriteData <= Spi_RecPackage[7:0];
				FlashState <= STATE_RAMWRITE;
            end
			//write to flash ram over
			STATE_RAMWRITE:
            begin
            	if(o_ramread_doneflag == 1'b1)
                begin
                	RamRead_WriteFlag <= 1'b0;
					if(ByteCnt < i_length)
                    begin
                    	FlashState <= STATE_DATAREAD;
                    end
                    else
                    begin
						ByteCnt <= 16'd0;
						o_doneflag <= 1'b1;
                    	FlashState <= STATE_END;
                    end
                end
            end
			//read write over status
			STATE_STATUSREAD:
            begin
				spi_cs <= 1'b0;
            	Spi_StartFlag <= 1'b1;
                Spi_TransLength <= 16'd16;
                Spi_SendPackage[SPI_MAXLENGTH-1:SPI_MAXLENGTH-16] <= 16'h05ff;
				EndFlag <= 1'b1;
				JumpState <= STATE_STATUSCHECK;
                FlashState <= STATE_OK;
            end
			//check write over flag
			STATE_STATUSCHECK:
            begin
            	if(Spi_RecPackage[0] == 1'b1)
                begin
                	FlashState <= STATE_STATUSREAD;
                end
                else
                begin
					o_doneflag <= 1'b1;
                	FlashState <= STATE_END;
                end
            end
            //spi over
            STATE_OK:
            begin
            	Spi_StartFlag <= 1'b0;
				if(Spi_DoneFlag == 1'b1)
                begin
					if(EndFlag == 1'b1)
                    begin
						spi_cs <= 1'b1;
                    	FlashState <= STATE_DELAY;
                    end
                    else
                    begin
                    	FlashState <= JumpState;
                    end
                end
            end
            //delay while spi transmit over
            STATE_DELAY:
            begin
            	if(DelayCnt < 32'd1000)
                begin
                	DelayCnt <= DelayCnt + 32'd1;
                end
                else
                begin
                	DelayCnt <= 32'd0;
                    FlashState <= JumpState;
                end
            end
			//work over
			STATE_END:
            begin
            	if( i_sectorerase == 1'b0 && 
                    i_block64erase == 1'b0 && 
                    i_writeflag == 1'b0 && 
                    i_readflag == 1'b0 )
                begin
                	o_doneflag <= 1'b0;
					FlashState <= STATE_IDLE;
                end
            end
        
        endcase
    end
end


SPI #
(
	.SYSCLKFREQ(50000000),
	.UARTBAUD(1000000), 
	.MAXLENGTH(SPI_MAXLENGTH)
)
flash
(
	.clk(sys_clk),			//system clk
    .rst_n(sys_rst),		//system reset
    
    .i_startflag(Spi_StartFlag),
    .o_doneflag(Spi_DoneFlag),
    .i_translength(Spi_TransLength),
    .i_sendpackage(Spi_SendPackage),
    .o_recpackage(Spi_RecPackage),
    
    .spi_clk(spi_clk),
    .spi_cs(),
    .spi_mosi(spi_mosi),
    .spi_miso(spi_miso)
);

Ram_W25Q64 ram_w25q64
(
	.i_clk(sys_clk),
    .i_rst(sys_rst),

    //write flash
    .i_tx_readflag(RamWrite_ReadFlag),
    .i_tx_writeflag(i_ramwrite_writeflag),
    .i_tx_readadd(RamWrite_ReadAdd),
    .i_tx_writeadd(i_ramwrite_writeadd),
    .i_tx_writedata(i_ramwrite_writedata),
    .o_tx_readdata(RamWrite_ReadData),
    .o_tx_doneflag(o_ramwrite_doneflag),

	//read flash
    .i_rx_readflag(i_ramread_readflag),
    .i_rx_writeflag(RamRead_WriteFlag),
    .i_rx_readadd(i_ramread_readadd),
    .i_rx_writeadd(RamRead_WriteAdd),
    .i_rx_writedata(RamRead_WriteData),
    .o_rx_readdata(o_ramread_readdata),
    .o_rx_doneflag(o_ramread_doneflag)
);

endmodule
