
include_directories(
  model
)

add_executable(GRNNGreedyBothResponseGen GRNNGreedyBothResponseGen.cpp)
target_link_libraries(GRNNGreedyBothResponseGen ${LIBS})

