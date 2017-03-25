
include_directories(
  model
)

add_executable(RNNGreedyBothResponseGen RNNGreedyBothResponseGen.cpp)
target_link_libraries(RNNGreedyBothResponseGen ${LIBS})

