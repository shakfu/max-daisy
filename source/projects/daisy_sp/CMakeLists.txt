cmake_minimum_required(VERSION 3.14)

project(DAISYSP VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(DaisySP STATIC 
    ${SOURCE_DIR}/Control/adenv.cpp
    ${SOURCE_DIR}/Control/adsr.cpp
    ${SOURCE_DIR}/Control/line.cpp
    ${SOURCE_DIR}/Control/phasor.cpp
    ${SOURCE_DIR}/Drums/analogbassdrum.cpp
    ${SOURCE_DIR}/Drums/analogsnaredrum.cpp
    ${SOURCE_DIR}/Drums/hihat.cpp
    ${SOURCE_DIR}/Drums/synthbassdrum.cpp
    ${SOURCE_DIR}/Drums/synthsnaredrum.cpp
    ${SOURCE_DIR}/Dynamics/balance.cpp
    ${SOURCE_DIR}/Dynamics/compressor.cpp
    ${SOURCE_DIR}/Dynamics/crossfade.cpp
    ${SOURCE_DIR}/Dynamics/limiter.cpp
    ${SOURCE_DIR}/Effects/autowah.cpp
    ${SOURCE_DIR}/Effects/bitcrush.cpp
    ${SOURCE_DIR}/Effects/chorus.cpp
    ${SOURCE_DIR}/Effects/decimator.cpp
    ${SOURCE_DIR}/Effects/flanger.cpp
    ${SOURCE_DIR}/Effects/fold.cpp
    ${SOURCE_DIR}/Effects/overdrive.cpp
    ${SOURCE_DIR}/Effects/reverbsc.cpp
    ${SOURCE_DIR}/Effects/phaser.cpp
    ${SOURCE_DIR}/Effects/sampleratereducer.cpp
    ${SOURCE_DIR}/Effects/tremolo.cpp
    ${SOURCE_DIR}/Effects/wavefolder.cpp
    ${SOURCE_DIR}/Filters/allpass.cpp
    ${SOURCE_DIR}/Filters/atone.cpp
    ${SOURCE_DIR}/Filters/biquad.cpp
    ${SOURCE_DIR}/Filters/comb.cpp
    ${SOURCE_DIR}/Filters/mode.cpp
    ${SOURCE_DIR}/Filters/moogladder.cpp
    ${SOURCE_DIR}/Filters/nlfilt.cpp
    ${SOURCE_DIR}/Filters/svf.cpp
    ${SOURCE_DIR}/Filters/tone.cpp
    ${SOURCE_DIR}/Filters/soap.cpp
    ${SOURCE_DIR}/Noise/clockednoise.cpp
    ${SOURCE_DIR}/Noise/grainlet.cpp
    ${SOURCE_DIR}/Noise/particle.cpp
    ${SOURCE_DIR}/PhysicalModeling/drip.cpp
    ${SOURCE_DIR}/PhysicalModeling/modalvoice.cpp
    ${SOURCE_DIR}/PhysicalModeling/pluck.cpp
    ${SOURCE_DIR}/PhysicalModeling/resonator.cpp
    ${SOURCE_DIR}/PhysicalModeling/KarplusString.cpp
    ${SOURCE_DIR}/PhysicalModeling/stringvoice.cpp
    ${SOURCE_DIR}/Synthesis/blosc.cpp
    ${SOURCE_DIR}/Synthesis/fm2.cpp
    ${SOURCE_DIR}/Synthesis/formantosc.cpp
    ${SOURCE_DIR}/Synthesis/oscillator.cpp
    ${SOURCE_DIR}/Synthesis/oscillatorbank.cpp
    ${SOURCE_DIR}/Synthesis/variablesawosc.cpp
    ${SOURCE_DIR}/Synthesis/variableshapeosc.cpp
    ${SOURCE_DIR}/Synthesis/vosim.cpp
    ${SOURCE_DIR}/Synthesis/zoscillator.cpp
    ${SOURCE_DIR}/Utility/dcblock.cpp
    ${SOURCE_DIR}/Utility/jitter.cpp
    ${SOURCE_DIR}/Utility/metro.cpp
    ${SOURCE_DIR}/Utility/port.cpp
)




target_include_directories(DaisySP
    PUBLIC
    ${SOURCE_DIR}
    ${SOURCE_DIR}/Control
    ${SOURCE_DIR}/Drums
    ${SOURCE_DIR}/Dynamics
    ${SOURCE_DIR}/Effects
    ${SOURCE_DIR}/Filters
    ${SOURCE_DIR}/Noise
    ${SOURCE_DIR}/PhysicalModeling
    ${SOURCE_DIR}/Synthesis
    ${SOURCE_DIR}/Utility
  )
