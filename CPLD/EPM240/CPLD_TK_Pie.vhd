LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.numeric_std.all;

ENTITY CPLD_TK_Pie IS

PORT 
(
	
	BUS_A			: in     std_logic_vector(15 downto 0);
	BUS_D			: inout  std_logic_vector(7 downto 0) := (others => 'Z');
	BUS_MRQ		: in     std_logic;
	BUS_IORQ		: in     std_logic;
	BUS_RD		: in     std_logic;
	BUS_WR		: in     std_logic;
	
	BUS_RESET	: out    std_logic := 'Z';
	
	BUS_ROMCS	: out    std_logic := 'Z';

	GPIO			: inout  std_logic_vector(20 downto 0) := (others => 'Z');
	
	GPIO_21		: in std_logic;
	GPIO_22		: in std_logic;
	GPIO_23		: in std_logic;
	
	GPIO_24		: out std_logic;
	GPIO_25		: out std_logic;
	GPIO_26		: out std_logic;
	GPIO_27		: out std_logic;
	
	BURSTGATE	: out std_logic;
	M1				: in std_logic	;	
	CLKCPU_OUT	: out std_logic;
	CLK 			: in std_logic	
	
);
END CPLD_TK_Pie;
--------------------------------------
ARCHITECTURE dataflow OF CPLD_TK_Pie IS

signal border			: std_logic;
signal vram				: std_logic;
signal port_BF3B		: std_logic;
signal port_FF3B		: std_logic;

-- latchs
signal latch 			: std_logic;
signal latch_a 		: std_logic_vector(12 downto 0);
signal latch_d 		: std_logic_vector(7 downto 0);


BEGIN


	-- ULAplus
	port_BF3B <= '0' when BUS_IORQ = '0' and BUS_WR = '0' and BUS_A(15 downto 14) = "10"  and BUS_A(7 downto 6) = "00"  and BUS_A(2) = '0' and BUS_A(0) ='1' else '1';
	port_FF3B <= '0' when BUS_IORQ = '0' and BUS_WR = '0' and BUS_A(15 downto 14) = "11"  and BUS_A(7 downto 6) = "00"  and BUS_A(2) = '0' and BUS_A(0) ='1' else '1';
	
	-- borda
	border <= '0' when BUS_IORQ = '0'  and BUS_WR = '0' and BUS_A(0) = '0' else '1';
	
	-- video
	vram <= '0' when BUS_MRQ = '0' and BUS_WR = '0' and BUS_A(15 downto 13) = "010" else '1';

	-- latchs
	latch <= '1' when border = '0' or vram = '0' or port_BF3B = '0' or port_FF3B = '0' else '0';
	latch_d <= BUS_D when latch = '1';
	latch_a <= BUS_A(12 downto 0) when latch = '1';
	
	
	-- Latch para o video - tem q voltar
	GPIO(7 downto 0)  <= latch_d 					;
	GPIO(20 downto 8) <= latch_a(12 downto 0) ;
	
	GPIO_24 <= vram;
	GPIO_25 <= border;
	GPIO_26 <= port_BF3B;
	GPIO_27 <= port_FF3B;
	
END dataflow;