
`timescale 1 ns / 1 ps

	module SNIFFER_v1_0 #
	(
		// Users to add parameters here
		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S00_AXI
		parameter integer C_S00_AXI_DATA_WIDTH	= 32,
		parameter integer C_S00_AXI_ADDR_WIDTH	= 7
	)
	(
		// Users to add ports here
        input wire [39:0] my_raddress,
        input wire [39:0] my_waddress,
        input wire f_my_arready,
        input wire f_my_arvalid,
        input wire f_my_awready,
        input wire f_my_awvalid,
        input wire b_my_arready,
        input wire b_my_arvalid,
        input wire b_my_awready,
        input wire b_my_awvalid,
        input wire [1:0] my_arburst,
        input wire [7:0] my_arlen,
        input wire [2:0] my_arsize,
        input wire f_my_rready,
        input wire f_my_rvalid,
        input wire f_my_wready,
        input wire f_my_wvalid,
        input wire b_my_rready,
        input wire b_my_rvalid,
        input wire b_my_wready,
        input wire b_my_wvalid,
        input wire [127:0] my_rdata,
        input wire my_wlast_f,
        input wire my_rlast_f,
        input wire my_wlast_b,
        input wire my_rlast_b,
		// User ports ends
		// Do not modify the ports beyond this line


		// Ports of Axi Slave Bus Interface S00_AXI
		input wire  s00_axi_aclk,
		input wire  s00_axi_aresetn,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_awaddr,
		input wire [2 : 0] s00_axi_awprot,
		input wire  s00_axi_awvalid,
		output wire  s00_axi_awready,
		input wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_wdata,
		input wire [(C_S00_AXI_DATA_WIDTH/8)-1 : 0] s00_axi_wstrb,
		input wire  s00_axi_wvalid,
		output wire  s00_axi_wready,
		output wire [1 : 0] s00_axi_bresp,
		output wire  s00_axi_bvalid,
		input wire  s00_axi_bready,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_araddr,
		input wire [2 : 0] s00_axi_arprot,
		input wire  s00_axi_arvalid,
		output wire  s00_axi_arready,
		output wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_rdata,
		output wire [1 : 0] s00_axi_rresp,
		output wire  s00_axi_rvalid,
		input wire  s00_axi_rready
	);
// Instantiation of Axi Bus Interface S00_AXI
	SNIFFER_v1_0_S00_AXI # ( 
		.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
	) SNIFFER_v1_0_S00_AXI_inst (
	    .MY_RADDRESS(my_raddress),
	    .MY_WADDRESS(my_waddress),
	    .F_MY_ARREADY(f_my_arready),
	    .F_MY_ARVALID(f_my_arvalid),
	    .F_MY_AWREADY(f_my_awready),
	    .F_MY_AWVALID(f_my_awvalid),
	    .B_MY_ARREADY(b_my_arready),
	    .B_MY_ARVALID(b_my_arvalid),
	    .B_MY_AWREADY(b_my_awready),
	    .B_MY_AWVALID(b_my_awvalid),
	    .MY_ARBURST(my_arburst),
	    .MY_ARLEN(my_arlen),
	    .MY_ARSIZE(my_arsize),
	    .F_MY_RVALID(f_my_rvalid),
	    .F_MY_RREADY(f_my_rready),
	    .F_MY_WVALID(f_my_wvalid),
	    .F_MY_WREADY(f_my_wready),
	    .B_MY_RVALID(b_my_rvalid),
	    .B_MY_RREADY(b_my_rready),
	    .B_MY_WVALID(b_my_wvalid),
	    .B_MY_WREADY(b_my_wready),
	    .MY_RDATA(my_rdata),
	    .MY_WLAST_F(my_wlast_f),
	    .MY_RLAST_F(my_rlast_f),
	    .MY_WLAST_B(my_wlast_b),
	    .MY_RLAST_B(my_rlast_b),
		.S_AXI_ACLK(s00_axi_aclk),
		.S_AXI_ARESETN(s00_axi_aresetn),
		.S_AXI_AWADDR(s00_axi_awaddr),
		.S_AXI_AWPROT(s00_axi_awprot),
		.S_AXI_AWVALID(s00_axi_awvalid),
		.S_AXI_AWREADY(s00_axi_awready),
		.S_AXI_WDATA(s00_axi_wdata),
		.S_AXI_WSTRB(s00_axi_wstrb),
		.S_AXI_WVALID(s00_axi_wvalid),
		.S_AXI_WREADY(s00_axi_wready),
		.S_AXI_BRESP(s00_axi_bresp),
		.S_AXI_BVALID(s00_axi_bvalid),
		.S_AXI_BREADY(s00_axi_bready),
		.S_AXI_ARADDR(s00_axi_araddr),
		.S_AXI_ARPROT(s00_axi_arprot),
		.S_AXI_ARVALID(s00_axi_arvalid),
		.S_AXI_ARREADY(s00_axi_arready),
		.S_AXI_RDATA(s00_axi_rdata),
		.S_AXI_RRESP(s00_axi_rresp),
		.S_AXI_RVALID(s00_axi_rvalid),
		.S_AXI_RREADY(s00_axi_rready)
	);

	// Add user logic here

	// User logic ends

	endmodule

