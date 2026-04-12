cmake_minimum_required(VERSION 3.15)

project(GNSS_Speedometer LANGUAGES C CXX ASM)

find_bsp(ID com.sysprogs.project_importers.stm32.cubemx SOURCE_PROJECT GNSS_Speedometer.ioc)

add_bsp_based_executable(NAME GNSS_Speedometer
	GENERATE_BIN
	GENERATE_MAP)

