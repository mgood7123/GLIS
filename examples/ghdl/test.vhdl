library ieee;
use ieee.std_logic_1164.all;
entity p is port(a : in std_logic; b : out std_logic); end p;

architecture behavior of p is
begin
    process (a)
    begin
        b <= a;
    end process;
end;

library ieee;
use ieee.std_logic_1164.all;
entity main is end main;

ARCHITECTURE csa OF main IS
    signal i : std_logic := '0';
    signal o : std_logic;
    component p is port(a : in std_logic; b : out std_logic); end component;
BEGIN
    a: p port map (a => i, b => o);
    process
    begin
        report "i is" & std_logic'image(i) ;
        report "o is" & std_logic'image(o) ;
        wait;
    end process;
end csa;
