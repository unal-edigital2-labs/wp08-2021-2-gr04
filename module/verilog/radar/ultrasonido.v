`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10.02.2022 12:09:12
// Design Name: 
// Module Name: Ultra
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module ultrasonido(clk, trig, echo, distancia, init, done);
input init;
input clk;
input echo;
output reg trig;
output reg [8:0] distancia;
output reg done;
reg [26:0] cont;
reg [26:0] cont_t;
reg [8:0] dist;


always @(posedge clk) begin
//if(!init) begin
  //  cont = 0;
	//dist = 0;
	//cont_t = 0;
//	done = 0;
//end
	cont = cont + 1;
	
	if (cont<1024)
		trig = 1'b1;
	else begin
		trig = 1'b0;
		
		if (echo == 1)begin
			
			cont_t = cont_t + 1;
			dist = (cont_t*34/200_000);
			
			
		
			end
		else if (echo == 0)begin
			done = 1;
			cont_t = 0;
			distancia <= dist;
			end
		end

		
	
end

endmodule
