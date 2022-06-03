create_clock -name CLOCK_50 -period 50MHz [get_ports CLOCK_50]
create_clock -name EXT_CLK -period 17MHz [get_ports GPIO_0[11]]

create_generated_clock -name CD_SINGLE_SPEED -source [get_ports {GPIO_0[11]}] -divide_by 8 [get_registers {CLK_SINGLE_SPEED}]
create_generated_clock -name CD_DOUBLE_SPEED -source [get_ports {GPIO_0[11]}] -divide_by 4 [get_registers {CLK_DOUBLE_SPEED}]
#create_generated_clock -name CD_CLK 			-source [get_ports {GPIO_0[11]}] -divide_by 4 [get_registers {CD_CLK}]


set_false_path -from [get_clocks {CLOCK_50}] -to [get_clocks {CD_SINGLE_SPEED}]
set_false_path -from [get_clocks {CLOCK_50}] -to [get_clocks {CD_DOUBLE_SPEED}]

#set_false_path -from [get_clocks {CLOCK_50}] -to [get_registers {SENS_OUT[*]}]
#set_false_path -from [get_clocks {SYS_CLK}] -to [get_registers {track_count_r[*]}]

#set_false_path -from [get_clocks {CLOCK_50}] -to [get_registers {trc_cnt_en_reg}]

#set_false_path -from [all_clocks] -to [get_registers {GPIO_01[*]}]
