
include_directories(
  model
)

add_executable(GRNNGreedyEmotResponseGen GRNNGreedyEmotResponseGen.cpp)
target_link_libraries(GRNNGreedyEmotResponseGen ${LIBS})

