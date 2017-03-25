
include_directories(
  model
)

add_executable(GRNNGreedyResponseGen GRNNGreedyResponseGen.cpp)
target_link_libraries(GRNNGreedyResponseGen ${LIBS})

