
include_directories(
  model
)

add_executable(NNGreedyResponseGen NNGreedyResponseGen.cpp)
target_link_libraries(NNGreedyResponseGen ${LIBS})

