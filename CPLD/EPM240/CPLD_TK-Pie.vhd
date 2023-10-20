LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY CPLD_TK-Pie IS

PORT 
(
	a, b, cin: IN BIT; 
	s, cout: OUT BIT
);
END CPLD_TK-Pie;
--------------------------------------
ARCHITECTURE dataflow OF CPLD_TK-Pie IS
BEGIN
 s <= a XOR b XOR cin;
 cout <= (a AND b) OR ((a XOR b)AND cin);
END dataflow;