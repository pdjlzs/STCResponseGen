
include_directories(
  model
)

add_executable(RNNGreedyEmotResponseGen RNNGreedyEmotResponseGen.cpp)
target_link_libraries(RNNGreedyEmotResponseGen ${LIBS})

