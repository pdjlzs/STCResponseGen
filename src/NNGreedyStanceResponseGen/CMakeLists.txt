
include_directories(
  model
)

add_executable(NNGreedyStanceResponseGen NNGreedyStanceResponseGen.cpp)
target_link_libraries(NNGreedyStanceResponseGen ${LIBS})

