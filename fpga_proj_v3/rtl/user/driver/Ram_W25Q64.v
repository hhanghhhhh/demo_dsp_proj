module Ram_W25Q64
(
	input   wire           	i_clk			,
    input   wire           	i_rst			,
    
    input					i_tx_readflag	,
    input					i_tx_writeflag	,
    input			[7:0]	i_tx_readadd	,
    input			[7:0]	i_tx_writeadd	,
    input			[7:0]	i_tx_writedata	,
    output	wire	[7:0]	o_tx_readdata	,
    output	reg				o_tx_doneflag	,

    input					i_rx_readflag	,
    input					i_rx_writeflag	,
    input			[7:0]	i_rx_readadd	,
    input			[7:0]	i_rx_writeadd	,
    input			[7:0]	i_rx_writedata	,
    output	wire	[7:0]	o_rx_readdata	,
    output	reg				o_rx_doneflag
);


parameter	STATE_IDLE	=	1'b0;
parameter	STATE_WORK	=	1'b1;


reg	[7:0]	tx_address;
reg			tx_wea;
reg			tx_rsta;
reg			tx_cea;
reg			tx_ocea;
reg			tx_state;
reg	[7:0]	tx_cnt;

reg	[7:0]	rx_address;
reg			rx_web;
reg			rx_rstb;
reg			rx_ceb;
reg			rx_oceb;
reg			rx_state;
reg	[7:0]	rx_cnt;


always @(posedge i_clk or negedge i_rst)
begin
	if(!i_rst)
    begin
    	tx_address <= 8'd0;
        tx_wea <= 1'b0;
        tx_rsta <= 1'b1;
        tx_cea <= 1'b0;
        tx_ocea <= 1'b1;
        o_tx_doneflag <= 1'b0;
        tx_cnt <= 8'd0;
        tx_state <= STATE_IDLE;
    end
    else
    begin
    	tx_rsta <= 1'b0;
        
    	case(tx_state)
        
        	STATE_IDLE:
        	begin
        		if(i_tx_readflag == 1'b1)
            	begin
            		tx_state <= STATE_WORK;
                	tx_address <= i_tx_readadd;
                	tx_wea <= 1'b0;
                	tx_cea <= 1'b1;
        			tx_ocea <= 1'b1;
            	end
            	else if(i_tx_writeflag == 1'b1)
            	begin
            		tx_state <= STATE_WORK;
                	tx_address <= i_tx_writeadd;
                	tx_wea <= 1'b1;
                	tx_cea <= 1'b1;
        			tx_ocea <= 1'b1;
            	end
        	end
        
        	STATE_WORK:
        	begin
        		if(tx_cnt < 8'd1)
                	tx_cnt <= tx_cnt + 8'd1;
                else
                begin
                	tx_address <= 11'd0;
                	tx_wea <= 1'b0;
                	tx_cea <= 1'b0;
        			tx_ocea <= 1'b1;

					if(i_tx_readflag == 1'b1 || i_tx_writeflag == 1'b1)
					begin
        				o_tx_doneflag <= 1'b1;
					end
					else
					begin
						tx_cnt <= 8'd0;
						tx_state <= STATE_IDLE;
                    	o_tx_doneflag <= 1'b0;
					end
                end
        	end
        
        endcase
    end
end

always @(posedge i_clk or negedge i_rst)
begin
	if(!i_rst)
    begin
    	rx_address <= 8'd0;
        rx_web <= 1'b0;
        rx_rstb <= 1'b1;
        rx_ceb <= 1'b0;
        rx_oceb <= 1'b1;
        o_rx_doneflag <= 1'b0;
        rx_cnt <= 8'd0;
        rx_state <= STATE_IDLE;
    end
    else
    begin
    	rx_rstb <= 1'b0;
        
    	case(rx_state)
        
        	STATE_IDLE:
        	begin
        		if(i_rx_readflag == 1'b1)
            	begin
            		rx_state <= STATE_WORK;
                	rx_address <= i_rx_readadd;
                	rx_web <= 1'b0;
                	rx_ceb <= 1'b1;
        			rx_oceb <= 1'b1;
            	end
            	else if(i_rx_writeflag == 1'b1)
            	begin
            		rx_state <= STATE_WORK;
                	rx_address <= i_rx_writeadd;
                	rx_web <= 1'b1;
                	rx_ceb <= 1'b1;
        			rx_oceb <= 1'b1;
            	end
        	end
        
        	STATE_WORK:
        	begin
        		if(rx_cnt < 8'd1)
                	rx_cnt <= rx_cnt + 8'd1;
                else
                begin
                	rx_address <= 11'd0;
                	rx_web <= 1'b0;
                	rx_ceb <= 1'b0;
        			rx_oceb <= 1'b1;

					if(i_rx_readflag == 1'b1 || i_rx_writeflag == 1'b1)
					begin
        				o_rx_doneflag <= 1'b1;
					end
					else
                    begin
						rx_cnt <= 8'd0;
						o_rx_doneflag <= 1'b0;
                    	rx_state <= STATE_IDLE;
                    end
                end
        	end
        
        endcase
    end
end


Ip_Ram_W25Q64 ram_w25q64
(
	.doa(o_tx_readdata),
    .dia(i_tx_writedata),
    .addra(tx_address),
    .cea(tx_cea),
    .clka(i_clk),
    .wea(tx_wea),
    .rsta(tx_rsta),
    .ocea(tx_ocea),
    
	.dob(o_rx_readdata),
    .dib(i_rx_writedata),
    .addrb(rx_address),
    .ceb(rx_ceb),
    .clkb(i_clk),
    .web(rx_web),
    .rstb(rx_rstb),
    .oceb(rx_oceb)
);

endmodule
