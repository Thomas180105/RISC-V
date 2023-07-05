cmake_minimum_required(VERSION 3.16)
project(RISC_V)

set(CMAKE_CXX_STANDARD 14)

add_executable(code src/main.cpp src/type.h src/Instruction.cpp src/Instruction.h src/tester.h src/source.cpp src/source.h src/queue.h src/ReorderBuffer.cpp src/ReorderBuffer.h src/registerFile.cpp src/registerFile.h src/load_store_buffer.cpp src/load_store_buffer.h src/reservationStation.cpp src/reservationStation.h src/bus.cpp src/bus.h src/ALU.cpp src/ALU.h src/predictor.h)
