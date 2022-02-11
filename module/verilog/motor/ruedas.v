`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 14.01.2022 17:40:50
// Design Name: 
// Module Name: ruedas
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


module ruedas(input clk,
            input [2:0]move,
            output reg [1:0]right,
            output reg [1:0]left);
    
always@(posedge clk)begin	
	case(move)
        // Recto
        3'b000:  begin
                    right[0] = 1;
                    right[1] = 0;
                    left[0]  = 1;
                    left[1]  = 0;
                end

        //Derecha
        3'b001:  begin
                    right[0] = 0;
                    right[1] = 0;
                    left[0]  = 1;
                    left[1]  = 0;
                end
        //Izquierda
        3'b010:  begin
                    right[0] = 1;
                    right[1] = 0;
                    left[0]  = 0;
                    left[1]  = 0;
                end
        // Quieto
        3'b011:  begin
                    right[1] = 0;
                    right[0] = 0;
                    left[1]  = 0;
                    left[0]  = 0;
                end

        // Giro 180
        3'b100:  begin
                    right[0] = 0;
                    right[1] = 1;
                    left[0]  = 1;
                    left[1]  = 0;
                end
        // Retroceder
   	    3'b101:  begin
                    right[1] = 1;
                    right[0] = 0;
                    left[1]  = 1;
                    left[0]  = 0;
                end
         default  begin
                    right[1] = 0;
                    right[0] = 0;
                    left[1]  = 0;
                    left[0]  = 0;
                end
	endcase
end
endmodule
