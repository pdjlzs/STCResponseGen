
include_directories(
  model
)

add_executable(RNNGreedyResponseGen RNNGreedyResponseGen.cpp)
target_link_libraries(RNNGreedyResponseGen ${LIBS})

