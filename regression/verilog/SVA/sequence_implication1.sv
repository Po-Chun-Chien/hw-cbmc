module main(input clk);

  reg [31:0] counter = 0;

  // 0 1 2 0 1 2 ...
  always_ff @(posedge clk)
    if(counter == 2)
      counter = 0;
    else
      counter++;

  // checks that 1 2 is followed by 0
  assert property (@(posedge clk) counter == 1 ##1 counter == 2 |-> ##1 counter == 0);

  // same with non-overlapping implication
  assert property (@(posedge clk) counter == 1 ##1 counter == 2 |=> counter == 0);

  // if the LHS doesn't match the implication is vacuously true
  assert property (0 |-> 0);

  // the implication must hold for _all_ matches of the LHS, not just one
  initial assert property (1 or ##1 1 |-> counter==0);

endmodule : main
