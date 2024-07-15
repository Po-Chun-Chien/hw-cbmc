module main;

  wire [7:0] implicit;
  wire bit [7:0] some_bits;
  wire logic [7:0] some_logic;
  wire unsigned [7:0] implicit_unsigned;
  wire bit unsigned [7:0] some_bits_unsigned;
  wire logic unsigned [7:0] some_logic_unsigned;
  wire signed [7:0] implicit_signed;
  wire bit signed [7:0] some_bits_signed;
  wire logic signed [7:0] some_logic_signed;

  // expected to pass
  p0: assert final ($bits(implicit) == 8);
  p1: assert final ($bits(some_bits) == 8);
  p2: assert final ($bits(some_logic) == 8);
  p3: assert final ($bits(implicit_unsigned) == 8);
  p4: assert final ($bits(some_bits_unsigned) == 8);
  p5: assert final ($bits(some_logic_unsigned) == 8);
  p6: assert final ($bits(implicit_signed) == 8);
  p7: assert final ($bits(some_bits_signed) == 8);
  p8: assert final ($bits(some_logic_signed) == 8);

endmodule
