
include_directories(
  model
)

add_executable(NNGreedyEmotResponseGen NNGreedyEmotResponseGen.cpp)
target_link_libraries(NNGreedyEmotResponseGen ${LIBS})

