
include_directories(
  model
)

add_executable(GNNGreedyStanceResponseGen GNNGreedyStanceResponseGen.cpp)
target_link_libraries(GNNGreedyStanceResponseGen ${LIBS})

