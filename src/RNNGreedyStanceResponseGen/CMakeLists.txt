
include_directories(
  model
)

add_executable(RNNGreedyStanceResponseGen RNNGreedyStanceResponseGen.cpp)
target_link_libraries(RNNGreedyStanceResponseGen ${LIBS})

