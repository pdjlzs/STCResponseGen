
include_directories(
  model
)

add_executable(NNGreedyBothResponseGen NNGreedyBothResponseGen.cpp)
target_link_libraries(NNGreedyBothResponseGen ${LIBS})

