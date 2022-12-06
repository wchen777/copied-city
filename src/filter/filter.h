#include <glm/glm.hpp>


static const glm::mat3 SHARPEN_FILTER = (1.f / 9.f) * glm::mat3(-1.f, -1.f, -1.f,
                                                   -1.f, 17.f, -1.f,
                                                   -1.f, -1.f, -1.f);


static const glm::mat3 LAPLACIAN_FILTER = glm::mat3(0, 1.f, 0,
                                                   1.f, -4.f, 1.f,
                                                   0, 1.f, 0);


static const glm::mat3 GRADIENT_FILTER = glm::mat3(5.f, 5.f, 5.f,
                                                   -3.f, 0, -3.f,
                                                   -3.f, -3.f, -3.f);
