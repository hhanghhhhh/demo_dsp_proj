//************************************************//
//FILE NAME:		DATA_REG.V
//DESCRIPTION:		FPGA与DSP数据交互模块
//AUTHOR:			XI LU
//DATE:				20230329
//LOG:				GREATE
//************************************************//

`include "user/app/Package.v" 

//************** Module Define *******************//
module DATA_REG
(
    input                       sys_clk         ,   //150M时钟
    input                       sys_rst         ,
    input      [6:0]            com_dsp_addr    ,
    //dsp与fpga之间连接引脚  
    input                       to_dsp_flag     ,
    input                       from_dsp_flag   ,
    input      [31:0]           from_dsp_data   ,  
    output  reg[31:0]           to_dsp_data     ,   

    output	reg					o_update,
	output	reg					o_flashwrite,
    output	reg		[15:0]		o_packagenum,
    output	reg		[15:0]		o_currentpackage,
	output	reg		[15:0]		o_writelength,
	input	wire				i_writeover,

	input	wire				i_erasestate,
    
	output	reg					o_ramwrite_writeflag,
    output	reg		[7:0]		o_ramwrite_writeadd,
    output	reg		[7:0]		o_ramwrite_writedata,
    input	wire				i_ramwrite_doneflag,
    
    output	reg					o_ramread_readflag,
    output	reg		[7:0]		o_ramread_readadd,
    input	wire	[7:0]		i_ramread_readdata,
    input	wire				i_ramread_doneflag


);

//*************** Parameter & Internal Register ****//

reg [31:0]   dataw_tmp              ;
reg [31:0]   tmp_dataW_r[0:1]       ;
reg [ 6:0]   addr_tmp               ;
reg [ 6:0]   tmp_addr_r[0:1]        ;
reg          to_dsp_flag_r[0:1]     ;
reg          from_dsp_flag_r[0:1]   ;

reg		FlashWriteFlag;
reg		FlashWriteOver;


//*************** Main Code ***********************//
//FPGA->DSP，将ADC数据读到的数据发送给DSP
always@(posedge sys_clk) begin
    tmp_addr_r[0]       <= com_dsp_addr;
    tmp_addr_r[1]       <= tmp_addr_r[0];
    tmp_dataW_r[0]      <= from_dsp_data;
    tmp_dataW_r[1]      <= tmp_dataW_r[0];
    to_dsp_flag_r[0]    <= to_dsp_flag;
    to_dsp_flag_r[1]    <= to_dsp_flag_r[0];
    from_dsp_flag_r[0]  <= from_dsp_flag;
    from_dsp_flag_r[1]  <= from_dsp_flag_r[0];
    if(tmp_addr_r[0] == tmp_addr_r[1]) begin
        addr_tmp <= tmp_addr_r[0];
    end
    if(tmp_dataW_r[0] == tmp_dataW_r[1]) begin
        dataw_tmp <= tmp_dataW_r[0];
    end
end



//FPGA->DSP，将ADC数据读到的数据发送给DSP
always@(posedge sys_clk or negedge sys_rst) begin
    if(!sys_rst) begin
    //initial regs
        to_dsp_data <= 32'd0;
    end
    else begin
        if(to_dsp_flag_r[1] == 1'b0) begin
            case(addr_tmp)
				7'd100: to_dsp_data <= {31'd0,i_erasestate};
				7'd101: to_dsp_data <= {31'd0,FlashWriteOver};

				7'd124: to_dsp_data <= {24'd0,`VERSION};
            endcase
        end
    end
end


reg[6:0] tmp_addr;
reg[31:0] tmp_dataW;

reg	[15:0]	UpdateData[63:0];   //synthesis keep
//DSP->FPGA，读取DSP发送过来的数据
always@(posedge sys_clk or negedge sys_rst) begin
    if(!sys_rst) begin
    //initial regs
        tmp_addr <= 7'd0;
        tmp_dataW <= 16'd0;
        
		o_update <= 1'b0;
        o_currentpackage <= 16'd0;
        o_writelength <= 16'd0;
        FlashWriteFlag <= 1'b0;
    end
    else begin
    	if(from_dsp_flag_r[1] == 1'b0)
    	begin
            tmp_dataW <= dataw_tmp;
            tmp_addr <= addr_tmp;
    	end
    	else begin
			if(tmp_addr < 7'd64)
            begin
            	UpdateData[tmp_addr] <= tmp_dataW[15:0];
            end
            else
            begin
            	case(tmp_addr)

					7'd110:	o_update <= tmp_dataW[0];
					7'd111:	o_currentpackage <= tmp_dataW[15:0];
					7'd112:	o_writelength <= tmp_dataW[15:0];
					7'd113:	FlashWriteFlag <= tmp_dataW[0];
                
            	endcase
            end
    	end
    end
end

parameter	FLASHSTATE_IDLE			=	8'd0;
parameter	FLASHSTATE_RAMWRITE		=	8'd1;
parameter	FLASHSTATE_RAMWRITEOVER	=	8'd2;
parameter	FLASHSTATE_FLASHWRITE	=	8'd3;

reg		[7:0]	FlashState;
reg		[7:0]	FlashWriteCnt;

always@(posedge sys_clk or negedge sys_rst)
begin
    if(!sys_rst)
	begin
		FlashState <= FLASHSTATE_IDLE;
		FlashWriteCnt <= 8'd0;
		FlashWriteOver <= 1'b0;
    end
    else
	begin
		case(FlashState)
        
        	FLASHSTATE_IDLE:
            begin
            	if(FlashWriteFlag == 1'b1)
                begin
                	FlashState <= FLASHSTATE_RAMWRITE;
                end
            end

			FLASHSTATE_RAMWRITE:
            begin
            	if(FlashWriteCnt < o_writelength)
                begin
    	            o_ramwrite_writeflag <= 1'b1;
    	            o_ramwrite_writeadd <= FlashWriteCnt;
    	            o_ramwrite_writedata <= UpdateData[FlashWriteCnt[7:1]][{FlashWriteCnt[0],3'd0}+:8];
					FlashState <= FLASHSTATE_RAMWRITEOVER;
                end
                else
                begin
                	FlashWriteCnt <= 8'd0;
					o_flashwrite <= 1'b1;
                    FlashState <= FLASHSTATE_FLASHWRITE;
                end
            end

			FLASHSTATE_RAMWRITEOVER:
            begin
            	if(i_ramwrite_doneflag == 1'b1)
                begin
					FlashWriteCnt <= FlashWriteCnt + 8'd1;
                	o_ramwrite_writeflag <= 1'b0;
					FlashState <= FLASHSTATE_RAMWRITE;
                end
            end

			FLASHSTATE_FLASHWRITE:
            begin
				if(FlashWriteFlag == 1'b1)
                begin
        			if(i_writeover == 1'b1)
            		begin
            			o_flashwrite <= 1'b0;
						FlashWriteOver <= 1'b1;
            		end
                end
				else
                begin
                	FlashWriteOver <= 1'b0;
					FlashState <= FLASHSTATE_IDLE;
                end
            end
        
        endcase
    end
end

endmodule

//*************** END OF LINE ********************//	
	
	